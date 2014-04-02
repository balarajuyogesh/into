/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#include "PiiWebcamDriver.h"
#include <PiiAsyncCall.h>

#include <QMutexLocker>
#include <PiiDelay.h>
#include <QDir>
#include <sys/prctl.h>
#include <poll.h>
#include <PiiDelay.h>
#include <PiiBits.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

static bool xioctl(int fd, int request, void  *arg)
{
  int r;

  do r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r == 0;
}

PiiWebcamDriver::PiiWebcamDriver() :
  _bOpen(false),
  _strBaseDir(""),
  _strCameraId(""),
  _strDevice(""),
  _iFrameBufferCount(4),
  _pCaptureThread(0),
  _uiFrameIndex(-1),
  _iMaxFrames(0),
  _bCroppingSupported(false),
  _iPixelFormat(V4L2_PIX_FMT_YUYV),
  _resolution(0,0),
  _iBitsPerPixel(8)
{
  _lstCriticalProperties << "frameBufferCount"
                         << "imageFormat"
                         << "frameRect"
                         << "frameSize"
                         << "triggerMode";

  QDir dir("/dev/v4l/by-id");
  if (dir.exists())
    _strBaseDir = "/dev/v4l/by-id";
  else
    _strBaseDir = "/dev";
}

PiiWebcamDriver::~PiiWebcamDriver()
{
  close();
}


QStringList PiiWebcamDriver::cameraList() const
{
  QDir dir(_strBaseDir);
  if (_strBaseDir != "/dev")
    return dir.entryList(QDir::NoDotDot | QDir::NoDot);
  else
    {
      QStringList strVideoDevices = dir.entryList(QDir::System | QDir::NoDotDot | QDir::NoDot);
      return strVideoDevices.filter("video");
    }
}

void PiiWebcamDriver::initialize(const QString& cameraId)
{
  //piiDebug("PiiWebcamDriver::initialize(%s)", qPrintable(cameraId));

  if (_pCaptureThread)
    PII_THROW(PiiCameraDriverException, tr("Capturing is running. Stop the capture first."));

  QStringList lstCameras = cameraList();
  if (lstCameras.isEmpty())
    PII_THROW(PiiCameraDriverException, tr("Couldn't find any cameras. Cannot initialize %1").arg(cameraId));

  QString strCameraId = cameraId.isEmpty() ? lstCameras[0] : cameraId;

  if (!lstCameras.contains(strCameraId))
    PII_THROW(PiiCameraDriverException, tr("Couldn't find camera by id %1").arg(strCameraId));

  QFileInfo info(QString("%1/%2").arg(_strBaseDir).arg(strCameraId));
  _strDevice = info.isSymLink() ? info.symLinkTarget() : info.filePath();
  //piiDebug(" device = %s", qPrintable(_strDevice));

  // Check if we must close the driver
  if (strCameraId != _strCameraId)
    close();

  // Get property map
  QVariantMap& dataMap = propertyMap();

  // Check if we must open the device
  if (!_bOpen)
    {
      _fileDevice.setFileName(_strDevice);
      if (!_fileDevice.open(QFile::ReadWrite))
        PII_THROW(PiiCameraDriverException, tr("Couldn't open device '%1'.").arg(strCameraId));

      struct v4l2_capability cap;

      if (!xioctl(_fileDevice.handle(), VIDIOC_QUERYCAP, &cap))
        PII_THROW(PiiCameraDriverException, tr("%1 is not V4L2 device").arg(strCameraId));

      if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        PII_THROW(PiiCameraDriverException, tr("%1 is no video capture device").arg(strCameraId));

      if (!(cap.capabilities & V4L2_CAP_STREAMING))
        PII_THROW(PiiCameraDriverException, tr("%1 does not support streaming i/o").arg(strCameraId));

      // Test cropping support
      struct v4l2_cropcap cropcap;
      struct v4l2_crop crop;
      CLEAR(cropcap);
      CLEAR(crop);
      cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      _bCroppingSupported = false;
      if (xioctl (_fileDevice.handle(), VIDIOC_CROPCAP, &cropcap))
        {
          crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
          crop.c = cropcap.defrect; /* reset to default */

          if (xioctl (_fileDevice.handle(), VIDIOC_S_CROP, &crop))
            _bCroppingSupported = true;
        }

      if (!dataMap.contains("triggerMode") &&
          !setTriggerMode(PiiCameraDriver::FreeRun))
        PII_THROW(PiiCameraDriverException, tr("Could not set default triggerMode."));
    }
  else if (!deregisterFrameBuffers())
    PII_THROW(PiiCameraDriverException, tr("Could not deregister frame buffers"));

  _strCameraId = strCameraId;

  // Set image format
  if (!setImageFormat(dataMap.contains("imageFormat") ?
                      dataMap.take("imageFormat").toInt() : (int)PiiCamera::Yuv422Format))
    PII_THROW(PiiCameraDriverException, tr("Couldn't set image format"));

  // Update frameRect
  if (dataMap.contains("frameRect") && !setFrameRect(dataMap.take("frameRect").toRect()))
    PII_THROW(PiiCameraDriverException, tr("Couldn't set frameRect."));

  // Write all configuration values from the map
  for (QVariantMap::iterator i=dataMap.begin(); i != dataMap.end(); ++i)
    {
      if (!QObject::setProperty(qPrintable(i.key()), i.value()))
        PII_THROW(PiiCameraDriverException, tr("Couldn't write the configuration value '%1'").arg(i.key()));
    }
  dataMap.clear();

  // Register frame buffers
  if (!registerFrameBuffers(_fileDevice.handle()))
    PII_THROW(PiiCameraDriverException, tr("Could not register frame buffers"));

  _vecBufferPointers.fill(0,_iFrameBufferCount);

  _bOpen = true;
}

bool PiiWebcamDriver::close()
{
  if (!_bOpen)
    return false;

  stopCapture();
  deregisterFrameBuffers();

  _fileDevice.close();
  _bOpen = false;

  return true;
}

bool PiiWebcamDriver::setTriggerMode(PiiCameraDriver::TriggerMode mode)
{
  QMutexLocker lock(&_captureMutex);
  if (_pCaptureThread)
    return false;
  _triggerMode = mode;
  return true;
}

PiiCameraDriver::TriggerMode PiiWebcamDriver::triggerMode() const
{
  return _triggerMode;
}

bool PiiWebcamDriver::triggerImage()
{
  _triggerWaitCondition.wakeOne();
  return true;
}

bool PiiWebcamDriver::startCapture(int frames)
{
  QMutexLocker lock(&_captureMutex);

  if (!_bOpen || listener() == 0 || _pCaptureThread)
    return false;

  _uiFrameIndex = -1;
  _iMaxFrames = _triggerMode == PiiCameraDriver::SoftwareTrigger ? 0 : frames;

  if (!startVideoStream(_fileDevice.handle()))
    {
      piiWarning(tr("Couldn't start video stream."));
      return false;
    }

  // Create and start the capturing thread
  _pCaptureThread = Pii::createAsyncCall(this, &PiiWebcamDriver::capture);
  _pCaptureThread->setObjectName("V4L2 capture");
  // Delete the thread if it stops spontaneously
  connect(_pCaptureThread, SIGNAL(finished()), this, SLOT(deleteCaptureThread()), Qt::DirectConnection);
  _pCaptureThread->start();

  return true;
}

void PiiWebcamDriver::deleteCaptureThread()
{
  QMutexLocker lock(&_captureMutex);
  if (_pCaptureThread)
    {
      _pCaptureThread->deleteLater();
      _pCaptureThread = 0;
    }
}

bool PiiWebcamDriver::stopCapture()
{
  QThread* pThread = 0;
  synchronized (_captureMutex)
    {
      if (_pCaptureThread)
        {
          pThread = _pCaptureThread;
          _pCaptureThread = 0;
          _triggerWaitCondition.wakeAll();
        }
      else
        return false;
    }
  pThread->wait();
  delete pThread;
  return true;
}

void PiiWebcamDriver::capture()
{
  QVector<void*> lstBuffers;
  lstBuffers.reserve(_iFrameBufferCount);

  bool bSoftwareTrigger = _triggerMode == PiiCameraDriver::SoftwareTrigger;
  bool bSuccess = true;

  while (_pCaptureThread)
    {
      if (bSoftwareTrigger)
        _triggerWaitCondition.wait();

      if (!_pCaptureThread)
        break;

      // Take all filled buffers from v4l
      do
        {
          void* pBuffer = 0;

          if (!grabFrame(_fileDevice.handle(), &pBuffer, 10))
            bSuccess = false;

          if (pBuffer == 0) break;
          lstBuffers << pBuffer;
        }
      while (!bSoftwareTrigger && lstBuffers.size() < _iFrameBufferCount);

      // Inform listener about each frame in turn.
      if (lstBuffers.size() > 0)
        {
          for (int i=0; i<lstBuffers.size(); ++i)
            {
              ++_uiFrameIndex;
              _vecBufferPointers[_uiFrameIndex % _iFrameBufferCount] = lstBuffers[i];
              listener()->frameCaptured(_uiFrameIndex, 0,0);
              if (_iMaxFrames > 0 && int(_uiFrameIndex) > _iMaxFrames)
                break;
            }
          lstBuffers.clear();
          lstBuffers.reserve(_iFrameBufferCount);
        }

      // Release buffers back to v4l
      if (!requeueBuffers(_fileDevice.handle()))
        {
          bSuccess = false;
          break;
        }
    }

  // Stop streaming
  if (!stopVideoStream(_fileDevice.handle()))
    piiWarning(tr("Failed to stop video stream."));

  // Inform listener
  listener()->captureFinished(bSuccess);
}

bool PiiWebcamDriver::grabFrame(int fd, void **buffer, int timeout)
{
  *buffer = 0;

  pollfd pfd = { fd, POLLIN, 0 };
  int r = poll(&pfd, 1, timeout);

  if (r == -1)
    {
      QString strReason;
      switch (errno)
        {
        case EFAULT:
          strReason = tr("Illegal memory address.");
          break;
        case EINTR:
          strReason = tr("Grabbing was interrupted by a signal.");
          break;
        default:
          if (pfd.revents & POLLNVAL)
            strReason = tr("Video device is not open.");
          else
            strReason = tr("Unknown error in grabbing image");
          break;
        }

      piiWarning(tr("Couldn't grab frame: %1").arg(strReason));
      return false;
    }

  if (r == 0) // poll() timeout
    return true;

  v4l2_buffer buf;
  CLEAR (buf);

  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (xioctl(fd, VIDIOC_DQBUF, &buf))
    {
      *buffer = _vecBuffers[buf.index]->frameStart;
      _vecReservedBuffers << buf;
      return true;
    }
  piiWarning("Failed to dequeue frame buffer.");
  return false;
}

bool PiiWebcamDriver::requeueBuffers(int fd)
{
  for (int i=0; i<_vecReservedBuffers.size(); i++)
    {
      if (!xioctl(fd, VIDIOC_QBUF, &_vecReservedBuffers[i]))
        {
          piiWarning(tr("Couldn't requeue buffer %1").arg(i));
          return false;
        }
    }
  _vecReservedBuffers.clear();

  return true;
}

bool PiiWebcamDriver::startVideoStream(int fd)
{
  v4l2_buf_type type;
  for (int i=0; i<_iFrameBufferCount; ++i)
    {
      v4l2_buffer buf;

      CLEAR (buf);

      buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory      = V4L2_MEMORY_MMAP;
      buf.index       = i;

      if (!xioctl(fd, VIDIOC_QBUF, &buf))
        return false;
    }

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  return xioctl(fd, VIDIOC_STREAMON, &type);
}

bool PiiWebcamDriver::stopVideoStream(int fd)
{
  v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  return xioctl(fd, VIDIOC_STREAMOFF, &type);
}

bool PiiWebcamDriver::registerFrameBuffers(int fd)
{
  struct v4l2_requestbuffers req;
  CLEAR (req);
  req.count               = _iFrameBufferCount;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_MMAP;

  if (!xioctl (fd, VIDIOC_REQBUFS, &req))
    {
      piiWarning(tr("Kernel video driver does not support memory mapping"));
      return false;
    }

  if (req.count < 2)
    {
      piiWarning(tr("Insufficient buffer memory"));
      return false;
    }

  if (int(req.count) != _iFrameBufferCount)
    {
      piiWarning(tr("Unable to reserve %1 frame buffers").arg(_iFrameBufferCount));
      return false;
    }

  for (int i=0; i<_iFrameBufferCount; i++)
    {
      v4l2_buffer buf;

      CLEAR (buf);

      buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index  = i;

      if (!xioctl(_fileDevice.handle(), VIDIOC_QUERYBUF, &buf))
        {
          piiWarning(tr("Error in query buffers from device"));
          return false;
        }

      WebcamBuffer *wBuffer = new WebcamBuffer;
      wBuffer->frameStart = mmap(NULL /* start anywhere */,
                                 buf.length,
                                 PROT_READ | PROT_WRITE /* required */,
                                 MAP_SHARED /* recommended */,
                                 fd, buf.m.offset);
      wBuffer->v4l2Buffer = buf;

      if (wBuffer->frameStart == MAP_FAILED)
        PII_THROW(PiiCameraDriverException, tr("Error in mmap buffers"));

      _vecBuffers << wBuffer;
    }

  return true;
}

bool PiiWebcamDriver::deregisterFrameBuffers()
{
  // munmap buffers
  for (int i=0; i<_vecBuffers.size(); i++)
    {
      if (munmap(_vecBuffers[i]->frameStart, _vecBuffers[i]->v4l2Buffer.length) == -1)
        piiWarning(tr("Error in unmap buffers"));
      delete _vecBuffers[i];
    }
  _vecBuffers.clear();
  _vecReservedBuffers.clear();
  _vecBufferPointers.clear();

  return true;
}

void* PiiWebcamDriver::frameBuffer(uint index) const
{
  return _vecBufferPointers[index % _iFrameBufferCount];
}

bool PiiWebcamDriver::isOpen() const
{
  return _bOpen;
}

bool PiiWebcamDriver::isCapturing() const
{
  return !!_pCaptureThread;
}

bool PiiWebcamDriver::requiresInitialization(const char* name) const
{
  if (_lstCriticalProperties.contains(QString(name)))
    return true;

  return PiiCameraDriver::requiresInitialization(name);
}

int PiiWebcamDriver::frameBufferCount() const
{
  return _iFrameBufferCount;
}

QSize PiiWebcamDriver::frameSize() const
{
  struct v4l2_format fmt;
  CLEAR(fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (!xioctl(_fileDevice.handle(), VIDIOC_G_FMT, &fmt))
    {
      piiWarning(tr("Couldn't get frame size."));
      return QSize(0,0);
    }

  return QSize(fmt.fmt.pix.width, fmt.fmt.pix.height);
}

QRect PiiWebcamDriver::frameRect() const
{
  v4l2_crop crop;
  CLEAR (crop);
  crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (!_bCroppingSupported || !xioctl(_fileDevice.handle(), VIDIOC_G_CROP, &crop))
    {
      piiWarning(tr("Cropping is not supported. We must use frameSize."));
      QSize s = frameSize();
      return QRect(0,0,s.width(), s.height());
    }
  else
    return QRect(crop.c.left, crop.c.top, crop.c.width, crop.c.height);
}

int PiiWebcamDriver::imageFormat() const
{
  struct v4l2_format fmt;
  CLEAR(fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (xioctl(_fileDevice.handle(), VIDIOC_G_FMT, &fmt))
    {
      switch(fmt.fmt.pix.pixelformat)
        {
        case V4L2_PIX_FMT_GREY: return PiiCamera::MonoFormat;
        case V4L2_PIX_FMT_SBGGR8: return PiiCamera::BayerBGGRFormat;
        case V4L2_PIX_FMT_Y41P: return PiiCamera::Yuv411Format;
        case V4L2_PIX_FMT_YUYV: return PiiCamera::Yuv422Format;
        case V4L2_PIX_FMT_RGB24: return PiiCamera::RgbFormat;
        case V4L2_PIX_FMT_BGR24: return PiiCamera::BgrFormat;
        default:
          piiWarning(tr("Unrecognized image format."));
          return (int)PiiCamera::InvalidFormat;
        }
    }
  else
    piiWarning(tr("Couldn't get imageFormat."));

  return (int)PiiCamera::InvalidFormat;
}

QSize PiiWebcamDriver::resolution() const
{
  return _resolution;
}

int PiiWebcamDriver::bitsPerPixel() const
{
  return _iBitsPerPixel;
}

QVariantList PiiWebcamDriver::frameSizes() const
{
  QVariantList lstFrameSizes;

  //List supported image formats
  int i=0;
  while (true)
    {
      v4l2_frmsizeenum frmsizes;
      CLEAR(frmsizes);
      frmsizes.pixel_format = _iPixelFormat;
      frmsizes.index = i;
      if (xioctl(_fileDevice.handle(), VIDIOC_ENUM_FRAMESIZES, &frmsizes))
        {
          if (frmsizes.type == V4L2_FRMSIZE_TYPE_DISCRETE)
            {
              lstFrameSizes << QSize(frmsizes.discrete.width, frmsizes.discrete.height);
              i++;
            }
          else if (frmsizes.type == V4L2_FRMSIZE_TYPE_STEPWISE)
            {
              /*piiDebug(tr("Supported frame sizes (StepWise):"));
              piiDebug(tr("min_width:   %1").arg(frmsizes.stepwise.min_width));
              piiDebug(tr("max_width:   %1").arg(frmsizes.stepwise.max_width));
              piiDebug(tr("step_width:  %1").arg(frmsizes.stepwise.step_width));
              piiDebug(tr("min_height:  %1").arg(frmsizes.stepwise.min_width));
              piiDebug(tr("max_height:  %1").arg(frmsizes.stepwise.max_width));
              piiDebug(tr("step_height: %1").arg(frmsizes.stepwise.step_width));*/
              lstFrameSizes << QSize(-1,-1);
              break;
            }
          else if (frmsizes.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)
            {
              /*piiDebug(tr("Supported frame sizes (Continuous):"));
              piiDebug(tr("min_width:   %1").arg(frmsizes.stepwise.min_width));
              piiDebug(tr("max_width:   %1").arg(frmsizes.stepwise.max_width));
              piiDebug(tr("step_width:  %1").arg(frmsizes.stepwise.step_width));
              piiDebug(tr("min_height:  %1").arg(frmsizes.stepwise.min_width));
              piiDebug(tr("max_height:  %1").arg(frmsizes.stepwise.max_width));
              piiDebug(tr("step_height: %1").arg(frmsizes.stepwise.step_width));*/
              lstFrameSizes << QSize(-1,-1);
            }
          else
            break;
        }
      else
        break;
    }

  return lstFrameSizes;
}

bool PiiWebcamDriver::setFrameBufferCount(int frameBufferCount)
{
  QMutexLocker lock(&_captureMutex);
  if (_pCaptureThread)
    return false;
  // Round frame buffer count up to closest power of two
  _iFrameBufferCount = frameBufferCount < 2 ? 1 :
    1 << Pii::lastOneBit((qMax(1,frameBufferCount)-1));
  return true;
}

bool PiiWebcamDriver::setFrameSize(const QSize& frameSize)
{
  struct v4l2_format fmt;
  CLEAR(fmt);
  fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.pixelformat = _iPixelFormat;
  fmt.fmt.pix.width       = frameSize.width() < 0 ? 99999 : frameSize.width();
  fmt.fmt.pix.height      = frameSize.height() < 0 ? 99999 : frameSize.height();

  if (!xioctl(_fileDevice.handle(), VIDIOC_S_FMT, &fmt))
    {
      piiWarning(tr("Couldn't set frame size"));
      return false;
    }

  return true;
}

bool PiiWebcamDriver::setFrameRect(const QRect& frameRect)
{
  if (_bCroppingSupported)
    {
      v4l2_crop crop;
      CLEAR (crop);
      crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      crop.c.left = frameRect.x();
      crop.c.top = frameRect.y();
      crop.c.width = frameRect.width();
      crop.c.height = frameRect.height();

      return xioctl(_fileDevice.handle(), VIDIOC_S_CROP, &crop);
    }
  else
    piiWarning(tr("Cropping not supported."));

  return true;
}

bool PiiWebcamDriver::setImageFormat(int value)
{
  bool bSupported = true;

  // Init image format
  struct v4l2_format fmt;
  CLEAR(fmt);
  fmt.type           = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  // Driver changes these to the nearest supported values
  fmt.fmt.pix.width  = 99999;
  fmt.fmt.pix.height = 99999;
  switch (PiiCamera::ImageFormat(value))
    {
    case PiiCamera::MonoFormat:
      _iPixelFormat = V4L2_PIX_FMT_GREY;
      _iBitsPerPixel = 8;
      break;
    case PiiCamera::BayerBGGRFormat:
      _iPixelFormat = V4L2_PIX_FMT_SBGGR8;
      _iBitsPerPixel = 8;
      break;
    case PiiCamera::Yuv411Format:
      _iPixelFormat = V4L2_PIX_FMT_Y41P;
      _iBitsPerPixel = 8;
      break;
    case PiiCamera::Yuv422Format:
      _iPixelFormat = V4L2_PIX_FMT_YUYV;
      _iBitsPerPixel = 8;
      break;
    case PiiCamera::RgbFormat:
      _iPixelFormat = V4L2_PIX_FMT_RGB24;
      _iBitsPerPixel = 24;
      break;
    case PiiCamera::BgrFormat:
      _iPixelFormat = V4L2_PIX_FMT_BGR24;
      _iBitsPerPixel = 24;
      break;
    default:
      bSupported = false;
      break;
    }
  fmt.fmt.pix.pixelformat = _iPixelFormat;

  if (!bSupported || !xioctl(_fileDevice.handle(), VIDIOC_S_FMT, &fmt))
    piiWarning(tr("Doesn't support imageFormat %1.").arg(value));

  _resolution = QSize(fmt.fmt.pix.width, fmt.fmt.pix.height);

  //List supported image formats
  int i=0;
  while (true)
    {
      v4l2_fmtdesc fmtdesc;
      CLEAR(fmtdesc);
      fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      fmtdesc.index = i;
      if (xioctl(_fileDevice.handle(), VIDIOC_ENUM_FMT, &fmtdesc))
        ++i;
      else
        break;
    }

  return true;
}
