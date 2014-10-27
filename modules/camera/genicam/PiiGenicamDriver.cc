/* This file is part of Into.
 * Copyright (C) Intopii
 * All rights reserved.
 */

#include "PiiGenicamDriver.h"
#include <PiiAsyncCall.h>
#include <PiiDelay.h>

#include <QMutexLocker>
#include <QLibrary>
#include <PiiTimer.h>

PiiGenicamDriver::PiiGenicamDriver(const QString& wrapperLibrary) :
  _strWrapperLibrary(wrapperLibrary),
  _bWrapperFunctionsInitialized(false),
  _pDevice(0),
  _pBuffer(0),
  _bOpen(false),
  _bCapturingRunning(false),
  _pCapturingThread(0),
  _iFrameIndex(-1),
  _iMaxFrames(0),
  _iHandledFrameCount(0),
  _iFrameBufferCount(10)
{
  _lstCriticalProperties << "frameBufferCount"
                         << "frameRect"
                         << "frameSize"
                         << "imageFormat"
                         << "packetSize"
                         << "triggerMode";
}

PiiGenicamDriver::~PiiGenicamDriver()
{
  if (_bOpen)
    close();

  if (_bWrapperFunctionsInitialized && genicamTerminate != 0)
    genicamTerminate();

  delete[] _pBuffer;
}

template <class T> T PiiGenicamDriver::resolveLib(QLibrary& lib, const QString& name)
{
  T func = T(lib.resolve(qPrintable(name)));
  if (func != 0)
    return func;

  // Find symbolic name as _NAME@(0-64)
  for (int i=0; i<=64; i+=4)
  {
    func = T(lib.resolve(qPrintable(QString("_%1@%2").arg(name).arg(i))));
    if (func != 0)
      return func;
  }

  return 0;
}

void PiiGenicamDriver::initializeWrapperFunctions()
{
  if (_bWrapperFunctionsInitialized)
    return;

  QLibrary lib(_strWrapperLibrary);
  if (!lib.load())
    {
      piiWarning(tr("Failed to load wrapper library %1").arg(_strWrapperLibrary));
      return;
    }

  genicamInitialize = resolveLib<GenicamIntVoidFunc>(lib, "genicam_initialize");
  genicamTerminate = resolveLib<GenicamIntVoidFunc>(lib, "genicam_terminate");
  genicamListCameras = resolveLib<GenicamIntCharppIntpFunc>(lib, "genicam_list_cameras");
  genicamNextCamera = resolveLib<GenicamCCharpCCharpFunc>(lib, "genicam_next_camera");
  genicamLastError = resolveLib<GenicamVoidCharpFunc>(lib, "genicam_last_error");
  genicamOpenDevice = resolveLib<GenicamIntCCharpDeviceppFunc>(lib, "genicam_open_device");
  genicamCloseDevice = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_close_device");
  genicamReconnectDevice = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_reconnect_device");
  genicamFree = resolveLib<GenicamIntVoidpFunc>(lib, "genicam_free");
  genicamSetProperty = resolveLib<GenicamIntDevicepCCharpIntFunc>(lib, "genicam_set_property");
  genicamGetProperty = resolveLib<GenicamIntDevicepCCharpIntpFunc>(lib, "genicam_get_property");
  genicamRegisterFramebuffers = resolveLib<GenicamIntDevicepUCharpIntFunc>(lib, "genicam_register_framebuffers");
  genicamDeregisterFramebuffers = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_deregister_framebuffers");
  genicamGrabFrame = resolveLib<GenicamIntDevicepUCharppIntFunc>(lib, "genicam_grab_frame");
  genicamRequeueBuffers = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_requeue_buffers");
  genicamStartCapture = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_start_capture");
  genicamStopCapture = resolveLib<GenicamIntDevicepFunc>(lib, "genicam_stop_capture");

  if (genicamInitialize == 0 ||
      genicamTerminate == 0 ||
      genicamListCameras == 0 ||
      genicamNextCamera == 0 ||
      genicamLastError == 0 ||
      genicamOpenDevice == 0 ||
      genicamCloseDevice == 0 ||
      genicamReconnectDevice == 0 ||
      genicamFree == 0 ||
      genicamSetProperty == 0 ||
      genicamGetProperty == 0 ||
      genicamRegisterFramebuffers == 0 ||
      genicamDeregisterFramebuffers == 0 ||
      genicamGrabFrame == 0 ||
      genicamRequeueBuffers == 0 ||
      genicamStartCapture == 0 ||
      genicamStopCapture == 0)
    {
      piiWarning(tr("Not all required functions were found in the wrapper library."));
      return;
    }

  if (genicamInitialize() != 0)
    {
      piiWarning(lastError());
      _bWrapperFunctionsInitialized = false;
    }
  else
    _bWrapperFunctionsInitialized = true;
}

QString PiiGenicamDriver::lastError() const
{
  char bfr[GENICAM_ERROR_MSG_SIZE];
  genicamLastError(bfr);
  return QString(bfr);
}

QVariant PiiGenicamDriver::property(const char* name) const
{
  QMutexLocker lock(&_reconnectMutex);
  return PiiCameraDriver::property(name);
}

bool PiiGenicamDriver::setProperty(const char* name, const QVariant& value)
{
  QMutexLocker lock(&_reconnectMutex);
  return PiiCameraDriver::setProperty(name, value);
}

QStringList PiiGenicamDriver::cameraList() const
{
  const_cast<PiiGenicamDriver*>(this)->initializeWrapperFunctions();

  if (!_bWrapperFunctionsInitialized)
    return QStringList();

  int iCount = 0;
  char* pCameras = 0;
  if (genicamListCameras(&pCameras, &iCount) != 0)
    piiWarning(lastError());

  QStringList lstCameras;
  const char* pCameraSerial = pCameras;
  for (int i=0; i<iCount; ++i)
    {
      lstCameras << QString(pCameraSerial);
      pCameraSerial = genicamNextCamera(pCameraSerial);
    }
  genicamFree(pCameras);

  //piiDebug("Found cameras: " + lstCameras.join(", "));
  return lstCameras;
}

void PiiGenicamDriver::initializeGenicamDevice(const QString& camId)
{
  QVariantMap& mapProperties = propertyMap();

  if (_pDevice == 0)
    {
      if (genicamOpenDevice(piiPrintable(camId), &_pDevice) != 0 || _pDevice == 0)
        PII_THROW(PiiCameraDriverException, tr("Could not open camera device: %1").arg(lastError()));
    }
  else
    {
      if (genicamDeregisterFramebuffers(_pDevice) != 0)
        PII_THROW(PiiCameraDriverException, tr("Could not deregister frame buffers: %1").arg(lastError()));
    }

  // triggerMode and imageFormat must be set first
  if (mapProperties.contains("triggerMode") &&
      !setTriggerMode((PiiCameraDriver::TriggerMode)mapProperties.take("triggerMode").toInt()))
    PII_THROW(PiiCameraDriverException, tr("Could not set trigger mode."));

  if (mapProperties.contains("imageFormat") &&
      !setImageFormat(mapProperties.take("imageFormat").toInt()))
    PII_THROW(PiiCameraDriverException, tr("Could not set image format."));

  // Write the rest of the configuration values from the map
  for (QVariantMap::iterator i = mapProperties.begin(); i != mapProperties.end(); ++i)
    {
      if (!QObject::setProperty(qPrintable(i.key()), i.value()))
        PII_THROW(PiiCameraDriverException, tr("Could not write the configuration value '%1'").arg(i.key()));
    }
  mapProperties.clear();

  // Create an image buffer
  int iImageSize = 0;
  if (genicamGetProperty(_pDevice, "payloadSize", &iImageSize) != 0)
    PII_THROW(PiiCameraDriverException, tr("Could not read payload size."));
  unsigned char* pBuffer = new unsigned char[_iFrameBufferCount * iImageSize];
  if (genicamRegisterFramebuffers(_pDevice, pBuffer, _iFrameBufferCount) != 0)
    {
      delete[] pBuffer;
      PII_THROW(PiiCameraDriverException, tr("Could not register frame buffers: %1").arg(lastError()));
    }

  _vecBufferPointers.fill(0,_iFrameBufferCount);

  delete[] _pBuffer;
  _pBuffer = pBuffer;
}

void PiiGenicamDriver::initialize(const QString& cameraId)
{
  //piiDebug("PiiGenicamDriver::initialize(%s)", qPrintable(cameraId));

  QStringList lstCameras = cameraList();
  if (!_bWrapperFunctionsInitialized)
    PII_THROW(PiiCameraDriverException, tr("Couldn't initialize wrapper."));
  if (lstCameras.isEmpty())
    PII_THROW(PiiCameraDriverException, tr("Couldn't find any cameras. Cannot initialize %1.").arg(cameraId));

  QString camId = cameraId.isEmpty() ? lstCameras[0] : cameraId;

  if (_bCapturingRunning)
    PII_THROW(PiiCameraDriverException, tr("Capturing is running. Stop the capture first."));

  // We must close the device if camera id has changed
  if (camId != _strCameraId)
    close();

  try
    {
      // Then initialize camera device with camId
      initializeGenicamDevice(camId);
      _strCameraId = camId;
      _bOpen = true;
    }
  catch (...)
    {
      // Close the device if something goes wrong
      if (_pDevice != 0)
        genicamCloseDevice(_pDevice);
      _pDevice = 0;
      _bOpen = false;

      throw;
    }
}

bool PiiGenicamDriver::close()
{
  if (!_bOpen)
    return false;

  if (_pDevice != 0)
    {
      if (genicamCloseDevice(_pDevice) != 0)
        piiWarning(lastError());
      _pDevice = 0;
    }
  _bOpen = false;

  return true;
}

bool PiiGenicamDriver::setTriggerMode(PiiCameraDriver::TriggerMode mode)
{
  if (writeIntValue("triggerMode", (int)mode))
    {
      _triggerMode = mode;
      return true;
    }

  return false;
}

PiiCameraDriver::TriggerMode PiiGenicamDriver::triggerMode() const
{
  return _triggerMode;
}

bool PiiGenicamDriver::triggerImage()
{
  _triggerWaitCondition.wakeOne();
  return true;
}

bool PiiGenicamDriver::startCapture(int frames)
{
  if (!_bOpen || listener() == 0 || _bCapturingRunning || _pDevice == 0)
    return false;

  // Create and start the capturing thread
  if (_pCapturingThread == 0)
    _pCapturingThread = Pii::createAsyncCall(this, &PiiGenicamDriver::capture);

  _bCapturingRunning = true;
  _iFrameIndex = -1;
  _iHandledFrameCount = 0;
  _iMaxFrames = _triggerMode == PiiCameraDriver::SoftwareTrigger ? 0 : frames;

  // Let the camera acquire
  if (genicamStartCapture(_pDevice) != 0)
    {
      piiWarning(lastError());
      return false;
    }

  _pCapturingThread->start();

  return true;
}

bool PiiGenicamDriver::stopCapture()
{
  if (!_bCapturingRunning)
    return false;

  // Stop the capturing thread
  stopCapturing();

  return true;
}

void PiiGenicamDriver::stopCapturing()
{
  _bCapturingRunning = false;
  _triggerWaitCondition.wakeAll();
  _pCapturingThread->wait();
}

bool PiiGenicamDriver::reconnect()
{
  QMutexLocker lock(&_reconnectMutex);

  if (genicamReconnectDevice(_pDevice) == 0)
    return true;

  piiWarning(tr("Couldn't reconnect device: %1").arg(lastError()));
  _bCapturingRunning = false;
  _pDevice = 0;
  _bOpen = false;

  return false;
}

void PiiGenicamDriver::capture()
{
  _pCapturingThread->setPriority(QThread::HighestPriority);

  QVector<unsigned char*> lstBuffers;
  lstBuffers.reserve(_iFrameBufferCount);

  int iHandledFrames = 0;
  bool bSoftwareTrigger = _triggerMode == PiiCameraDriver::SoftwareTrigger;
  while (_bCapturingRunning)
    {
      if (bSoftwareTrigger)
        _triggerWaitCondition.wait();
      else
        QThread::yieldCurrentThread();

      if (!_bCapturingRunning)
        break;

      do
        {
          unsigned char* pBuffer = 0;
          int iRetValue = genicamGrabFrame(_pDevice,&pBuffer,0);

          if (iRetValue != 0)
            {
              piiWarning(tr("Error in capturing: %1").arg(lastError()));

              if (iRetValue == 1)
                {
                  piiDebug(tr("Trying to reconnect"));
                  if (!reconnect())
                    {
                      // Inform listener
                      listener()->captureFinished(false);
                      return;
                    }
                  else if (genicamStartCapture(_pDevice) != 0)
                    {
                      piiWarning(tr("Couldn't restart capture."));
                      _bCapturingRunning = false;
                    }
                  else
                    {
                      piiDebug(tr("Reconnected successfully."));
                      int adder = _iFrameBufferCount - _iFrameIndex % _iFrameBufferCount;
                      if (adder > 0)
                        {
                          listener()->framesMissed(_iFrameIndex+1, _iFrameIndex+adder);
                          _iFrameIndex += adder;
                        }
                    }
                }
            }

          if (pBuffer == 0) break;
          lstBuffers << pBuffer;
        }
      while (!bSoftwareTrigger && lstBuffers.size() < _iFrameBufferCount);

      if (lstBuffers.size() > _iFrameBufferCount/2)
        {
          listener()->framesMissed(_iFrameIndex+1, _iFrameIndex+lstBuffers.size()-1);
          _iFrameIndex += lstBuffers.size();
          _vecBufferPointers[_iFrameIndex % _iFrameBufferCount] = lstBuffers.last();
          listener()->frameCaptured(_iFrameIndex, 0,0);
          iHandledFrames = 1;
        }
      else if (lstBuffers.size() > 0)
        {
          for (int i=0; i<lstBuffers.size(); ++i)
            {
              ++_iFrameIndex;
              _vecBufferPointers[_iFrameIndex % _iFrameBufferCount] = lstBuffers[i];
              listener()->frameCaptured(_iFrameIndex, 0,0);
            }
          iHandledFrames = lstBuffers.size();
        }
      else if (bSoftwareTrigger)
        listener()->frameCaptured(-1, 0,0);

      if (lstBuffers.size() > 0)
        lstBuffers.clear();

      if (genicamRequeueBuffers(_pDevice) != 0)
        {
          piiWarning(tr("Couldn't requeue buffers: %1").arg(lastError()));
          _bCapturingRunning = false;
        }

      // Check if we must stop capturing
      if (_iMaxFrames > 0)
        {
          _iHandledFrameCount += iHandledFrames;
          if (_iHandledFrameCount > _iMaxFrames)
            _bCapturingRunning = false;
        }
      iHandledFrames = 0;
    }

  if (genicamStopCapture(_pDevice) != 0)
    piiWarning(tr("Couldn't stop capture: %1").arg(lastError()));

  // Inform listener
  listener()->captureFinished(true);
}

void* PiiGenicamDriver::frameBuffer(uint frameIndex) const
{
  frameIndex %= _iFrameBufferCount;
  return _vecBufferPointers[frameIndex];
}

bool PiiGenicamDriver::isOpen() const
{
  return _bOpen;
}

bool PiiGenicamDriver::isCapturing() const
{
  return _bCapturingRunning;
}

bool PiiGenicamDriver::requiresInitialization(const char* name) const
{
  return _lstCriticalProperties.contains(QString(name));
}


int PiiGenicamDriver::frameBufferCount() const
{
  return _iFrameBufferCount;
}
double PiiGenicamDriver::frameRate() const
{
  return (double)readIntValue("frameRate") / 1000.0;
}
int PiiGenicamDriver::exposureTime() const
{
  return readIntValue("exposure", 4);
}
int PiiGenicamDriver::exposurePeriod() const
{
  return readIntValue("exposurePeriod", 55);
}
double PiiGenicamDriver::inputPulseFrequency() const
{
  return (double)readIntValue("inputPulseFrequency",0) / 1000.0;
}
int PiiGenicamDriver::autoExposureTarget() const
{
  return readIntValue("autoExposureTarget", 127);
}
bool PiiGenicamDriver::flipHorizontally() const
{
  return readIntValue("flipHorizontally",0) != 0;
}
int PiiGenicamDriver::maxHeight() const
{
  return readIntValue("offsetY", 0) + readIntValue("height$max",0);
}
QSize PiiGenicamDriver::sensorSize() const
{
  return QSize(readIntValue("sensorWidth", 0), readIntValue("sensorHeight", 0));
}
double PiiGenicamDriver::triggerRate() const
{
  return (double)readIntValue("triggerRate") / 10000.0;
}
int PiiGenicamDriver::triggerLine() const
{
  return readIntValue("triggerLine");
}

int PiiGenicamDriver::packetSize() const
{
  return readIntValue("packetSize", 1500);
}
double PiiGenicamDriver::gain() const
{
  int iMin = readIntValue("gain$min", 0);
  int iMax = readIntValue("gain$max", 2);
  int iGain = readIntValue("gain", 1);

  return qBound(0.0, (double)(iGain-iMin) / (double)(iMax-iMin), 1.0);
}
QRect PiiGenicamDriver::autoExposureArea() const
{
  return QRect(readIntValue("autoExposureAreaOffsetX", 0),
               readIntValue("autoExposureAreaOffsetY", 0),
               readIntValue("autoExposureAreaWidth", 0),
               readIntValue("autoExposureAreaHeight", 0));
}
QRect PiiGenicamDriver::frameRect() const
{
  return QRect(readIntValue("offsetX", 0),
               readIntValue("offsetY", 0),
               readIntValue("width", 0),
               readIntValue("height", 0));
}
QSize PiiGenicamDriver::frameSize() const
{
  return QSize(readIntValue("width", 0),
               readIntValue("height", 0));
}
int PiiGenicamDriver::imageFormat() const
{
  return readIntValue("imageFormat", (int)PiiCamera::MonoFormat);
}

QSize PiiGenicamDriver::resolution() const
{
  int width = readIntValue("sensorWidth", 0);
  int height = readIntValue("sensorHeight", 0);
  if (cameraType() == PiiCamera::LineScan)
    height = qMin(maxHeight(), 1024);

  return QSize(width,height);
}
int PiiGenicamDriver::bitsPerPixel() const
{
  return readIntValue("bitsPerPixel", 8);
}
int PiiGenicamDriver::cameraType() const
{
  return readIntValue("cameraType", 0);
}

bool PiiGenicamDriver::setFrameBufferCount(int frameBufferCount)
{
  _iFrameBufferCount = frameBufferCount;
  return true;
}
bool PiiGenicamDriver::setFrameRate(double fps)
{
  return writeIntValue("frameRate", int(fps * 1000.0));
}
bool PiiGenicamDriver::setExposureTime(int exposureTime)
{
  return writeIntValue("exposure", exposureTime);
}
bool PiiGenicamDriver::setExposurePeriod(int exposurePeriod)
{
  return writeIntValue("exposurePeriod", exposurePeriod);
}
bool PiiGenicamDriver::setGain(double gain)
{
  int iMin = readIntValue("gain$min", 0);
  int iMax = readIntValue("gain$max", 2);
  int iGain = iMin + (double)(iMax-iMin) * qBound(0.0,gain,1.0);
  return writeIntValue("gain", iGain);
}

bool PiiGenicamDriver::setFrameRect(const QRect& frameRect)
{
  int iWidth = frameRect.width() & ~3;
  int iHeight = frameRect.height() & ~3;
  int iOffsetX = frameRect.x() & ~3;
  int iOffsetY = frameRect.y() & ~3;
  int iMaxWidth = readIntValue("width$max", 0);
  int iMaxHeight = readIntValue("height$max", 0);
  int iMinWidth = readIntValue("width$min", 0);
  int iMinHeight = readIntValue("height$min", 0);

  if (iWidth < 0) iWidth = iMaxWidth;
  if (iHeight < 0)
    {
      iHeight = iMaxHeight;
      if (cameraType() == (int)PiiCamera::LineScan)
        iHeight = qMin(1024,iHeight);
    }

  iOffsetX = qBound(0, iOffsetX, iMaxWidth - iMinWidth);
  iOffsetY = qBound(0, iOffsetY, iMaxHeight - iMinHeight);
  iWidth = qBound(iMinWidth, iWidth, iMaxWidth - iOffsetX);
  iHeight = qBound(iMinHeight, iHeight, iMaxHeight - iOffsetY);

  bool bRet =  writeIntValue("offsetX", 0);
  bRet = writeIntValue("offsetY", 0) && bRet;
  bRet = writeIntValue("width", iWidth) && bRet;
  bRet = writeIntValue("height", iHeight) && bRet;
  bRet = writeIntValue("offsetX", iOffsetX) && bRet;
  bRet = writeIntValue("offsetY", iOffsetY) && bRet;

  return bRet;
}

bool PiiGenicamDriver::setFrameSize(const QSize& frameSize)
{
  bool retValue = writeIntValue("width", frameSize.width());
  retValue = writeIntValue("height", frameSize.height()) & retValue;

  return retValue;
}
bool PiiGenicamDriver::setImageFormat(int format)
{
  return writeIntValue("imageFormat", format);
}
bool PiiGenicamDriver::setPacketSize(int packetSize)
{
  return writeIntValue("packetSize", packetSize);
}

bool PiiGenicamDriver::setAutoExposureArea(const QRect& a)
{
  QRect area = a;

  bool ok = true;
  int maxWidth = readIntValue("width$max",0,&ok);
  if (!ok) return false;

  int maxHeight = readIntValue("height$max",0,&ok);
  if (!ok) return false;

  if (!area.isValid()) area = QRect(0,0,maxWidth, maxHeight);
  if (area.x() < 0) area.setX(0);
  if (area.y() < 0) area.setY(0);

  if (!writeIntValue("autoExposureAreaOffsetX", 0) ||
      !writeIntValue("autoExposureAreaOffsetY", 0) ||
      !writeIntValue("autoExposureAreaWidth", qMin(area.width(), maxWidth - area.x())) ||
      !writeIntValue("autoExposureAreaHeight", qMin(area.height(), maxHeight - area.y())))
    return false;

  if (area.x() > 0 && !writeIntValue("autoExposureAreaOffsetX", area.x()))
    return false;

  if (area.y() > 0 && !writeIntValue("autoExposureAreaOffsetY", area.y()))
    return false;

  return true;
}

bool PiiGenicamDriver::setAutoExposureTarget(int autoExposureTarget)
{
  return writeIntValue("autoExposureTarget", autoExposureTarget);
}

bool PiiGenicamDriver::setFlipHorizontally(bool flipHorizontally)
{
  return writeIntValue("flipHorizontally", flipHorizontally ? 1 : 0);
}

bool PiiGenicamDriver::setTriggerRate(double triggerRate)
{
  return writeIntValue("triggerRate", int(triggerRate * 10000.0));
}

bool PiiGenicamDriver::setTriggerLine(int triggerLine)
{
  return writeIntValue("triggerLine", triggerLine);
}

int PiiGenicamDriver::readIntValue(const char* name, int defaultValue, bool* ok) const
{
  int value = 0;
  if (genicamGetProperty(_pDevice, name, &value) != 0)
    {
      //piiWarning(lastError());
      if (ok)
        *ok = false;
      return defaultValue;
    }

  if (ok)
    *ok = true;
  return value;
}

bool PiiGenicamDriver::writeIntValue(const char* name, int value)
{
  if (_pDevice != 0)
    {
      if (genicamSetProperty(_pDevice, name, value) != 0)
        {
          piiWarning(lastError());
          return false;
        }
      return true;
    }
  return false;
}
