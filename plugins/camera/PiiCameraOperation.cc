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

#include "PiiCameraOperation.h"
#include <PiiMatrix.h>
#include <PiiBayerConverter.h>
#include <PiiUtil.h>

#include <QSettings>

#include <PiiLog.h>
#include <QFile>

PiiCameraOperation::Data::Data() :
  pCameraDriver(0),
  strCameraId(""),
  bTriggered(false),
  iImageWidth(0),
  iImageHeight(0),
  iBitsPerPixel(8),
  bCopyImage(false),
  iFrameCount(-1),
  bWaitPause(false),
  bMissedFrames(false),
  iMaxMissedIndex(0)
{
}

PiiCameraOperation::PiiCameraOperation(Data* dat) : PiiImageReaderOperation(dat)
{
  init();
}

PiiCameraOperation::PiiCameraOperation() : PiiImageReaderOperation(new Data)
{
  init();
}

void PiiCameraOperation::init()
{
  setThreadingCapabilities(NonThreaded);
  startTimer(5000);
}

PiiCameraOperation::~PiiCameraOperation()
{
  PII_D;

  if (d->pCameraDriver != 0)
    {
      d->pCameraDriver->close();
      delete d->pCameraDriver;
    }
}

void PiiCameraOperation::timerEvent(QTimerEvent*)
{
  PII_D;
  double dSecs = double(d->frameTimer.restart()) / 1000000.0;
  if (dSecs != 0)
    emit framesPerSecond(d->iFrameCount.fetchAndStoreRelaxed(0) / dSecs);
}

void PiiCameraOperation::check(bool reset)
{
  PII_D;

  if (d->pCameraDriver == 0)
    PII_THROW(PiiExecutionException, tr("Camera driver has not been set."));

  if (reset)
    {
      // If the trigger input is connected, we must change the trigger
      // mode to the SoftwareTrigger
      d->bTriggered = d->pTriggerInput->isConnected();
      if (d->bTriggered)
        d->pCameraDriver->setProperty("triggerMode", PiiCameraDriver::SoftwareTrigger);
      
      try
        {
          d->pCameraDriver->initialize(d->strCameraId);
        }
      catch (PiiException& ex)
        {
          PII_THROW(PiiExecutionException, tr("Couldn't initialize driver: %1").arg(ex.message()));
        }
      
      QSize frameSize = d->pCameraDriver->frameSize();
      d->iImageWidth = frameSize.width();
      d->iImageHeight = frameSize.height();
      d->imageFormat = (PiiCamera::ImageFormat)d->pCameraDriver->imageFormat();
      d->iBitsPerPixel = d->pCameraDriver->bitsPerPixel();
      d->iFrameCount = 0;
      d->frameTimer.restart();
      d->iMaxMissedIndex = 0;
      d->bMissedFrames = false;
    }
  
  PiiImageReaderOperation::check(reset);
}

void PiiCameraOperation::process()
{
  PII_D;
  d->pCameraDriver->triggerImage();
  d->waitCondition.wait();
}

void PiiCameraOperation::start()
{
  PII_D;

  if (d->pCameraDriver == 0)
    PII_THROW(PiiExecutionException, tr("Camera driver has not been set."));

  if (!d->pCameraDriver->isCapturing() && !d->pCameraDriver->startCapture(d->iMaxImages))
    PII_THROW(PiiExecutionException, tr("Couldn't start capture"));

  PiiImageReaderOperation::start();

  if (d->bWaitPause)
    {
      d->bWaitPause = false;
      d->pauseWaitCondition.wakeAll();
    }
}

void PiiCameraOperation::interrupt()
{
  PII_D;
  d->pCameraDriver->stopCapture();
  
  PiiImageReaderOperation::interrupt();
  d->pauseWaitCondition.wakeAll();
}

void PiiCameraOperation::pause()
{
  PII_D;
  QMutexLocker lock(&d->pauseMutex);
  d->bWaitPause = true;
  PiiImageReaderOperation::pause();
}

void PiiCameraOperation::stop()
{
  PII_D;
  d->pCameraDriver->stopCapture();

  PiiImageReaderOperation::stop();
  d->pauseWaitCondition.wakeAll();
}

void PiiCameraOperation::frameCaptured(int frameIndex, void *frameBuffer, qint64 elapsedTime)
{
  PII_D;
  
  QMutexLocker lock(&d->pauseMutex);
  if (d->bWaitPause)
    {
      d->pauseWaitCondition.wait();
      if (state() != Running)
        return;
      
      // We can't handle the frame if we missed it
      if (d->bMissedFrames &&
          (frameIndex <= d->iMaxMissedIndex || (d->iMaxMissedIndex < 0 && frameIndex > 0)))
        return;
      
      d->bMissedFrames = false;
    }
  
  if (frameIndex >= 0)
    {
      Pii::PtrOwnership ownership = frameBuffer != 0 ? Pii::ReleaseOwnership : Pii::RetainOwnership;
      void *pFrameBuffer = ownership == Pii::ReleaseOwnership ?
        frameBuffer : d->pCameraDriver->frameBuffer(frameIndex);
      d->iFrameCount.ref();
      
      if (pFrameBuffer == 0)
        ; //piiWarning("PiiCameraOperation::frameCaptured(), pFrameBuffer == 0");
      else
        {
          switch (d->iBitsPerPixel)
            {
            case 8:
              convert<unsigned char>(pFrameBuffer, ownership, frameIndex, elapsedTime);
              break;
            case 16:
              convert<unsigned short>(pFrameBuffer, ownership, frameIndex, elapsedTime);
              break;
            case 24:
              emitImage(PiiMatrix<PiiColor<unsigned char> >(d->iImageHeight, d->iImageWidth, pFrameBuffer, ownership),
                        ownership, frameIndex, elapsedTime);
              break;
              /*case 32:
                convertColor<PiiColor4<unsigned char> >(pFrameBuffer,
                ownership, frameIndex);
                break;
              */
            }

          // If we are triggered-mode we must wake one now
          if (d->bTriggered)
            d->waitCondition.wakeOne();
        }
    }
  else if (d->bTriggered)
    d->waitCondition.wakeOne();
}

template <class T> void PiiCameraOperation::convert(void *frameBuffer,
                                                    Pii::PtrOwnership ownership,
                                                    int frameIndex,
                                                    qint64 elapsedTime)
{
  PII_D;

  if (d->imageType == Original && d->imageFormat member_of (PiiCamera::MonoFormat, PiiCamera::RgbFormat)))
    {
      PiiMatrix<T> image(d->iImageHeight, d->iImageWidth, frameBuffer, ownership);
      emitImage(image, ownership, frameIndex, elapsedTime);
    }
  else
    {
      switch (d->imageFormat)
        {
        case PiiCamera::Yuv411Format:
          {
            emitImage(yuv411toRgb<T>(frameBuffer, d->iImageWidth, d->iImageHeight),
                      Pii::ReleaseOwnership, frameIndex, elapsedTime);

            // Free frameBuffer-memory if necessary
            if (ownership == Pii::ReleaseOwnership)
              free(frameBuffer);
            
            break;
          }
        case PiiCamera::Yuv422Format:
          {
            emitImage(yuv422toRgb<T>(frameBuffer, d->iImageWidth, d->iImageHeight),
                      Pii::ReleaseOwnership, frameIndex, elapsedTime);

            // Free frameBuffer-memory if necessary
            if (ownership == Pii::ReleaseOwnership)
              free(frameBuffer);

            break;
          }
        case PiiCamera::BayerBGGRFormat:
          {
            PiiMatrix<T> image(d->iImageHeight, d->iImageWidth, frameBuffer, ownership);
            emitImage(bayerToRgb(image, PiiCamera::BggrDecoder<T>(), PiiCamera::Rgb4Pixel<>()),
                      Pii::ReleaseOwnership, frameIndex, elapsedTime);
            break;
          }
        default:
          {
            PiiMatrix<T> image(d->iImageHeight, d->iImageWidth, frameBuffer, ownership);
            emitImage(image, ownership, frameIndex, elapsedTime);
          }
        }
    }
}

template <class T> void PiiCameraOperation::emitImage(const PiiMatrix<T>& image, Pii::PtrOwnership ownership, int frameIndex, qint64 elapsedTime)
{
  PII_D;

  if (d->bCopyImage && ownership == Pii::RetainOwnership)
    {
      PiiMatrix<T> img(image);
      img.detach();
      d->pImageOutput->emitObject(processImage(PiiVariant(img), frameIndex, elapsedTime));
    }
  else
    d->pImageOutput->emitObject(processImage(PiiVariant(image), frameIndex, elapsedTime));
}

PiiVariant PiiCameraOperation::processImage(const PiiVariant& image, int /*frameIndex*/, qint64 /*elapsedTime*/)
{
  return image;
}

QVariant PiiCameraOperation::property(const char* name) const
{
  const PII_D;
  
  if (strncmp(name, "driver.", 7) == 0)
    {
      if (d->pCameraDriver != 0)
        return d->pCameraDriver->property(name+7);
    }
  else
    return PiiImageReaderOperation::property(name);

  return QVariant();
}


bool PiiCameraOperation::setProperty(const char* name, const QVariant& value)
{
  PII_D;
  
  if (strncmp(name, "driver.", 7)==0)
    {
      if (d->pCameraDriver != 0)
        return d->pCameraDriver->setProperty(name+7, value);
    }
  else
    return PiiImageReaderOperation::setProperty(name,value);

  return false;
}

bool PiiCameraOperation::loadCameraConfig(const QString& fileName)
{
  PII_D;

  if (d->pCameraDriver == 0)
    {
      piiWarning(tr("Camera driver has not been set."));
      return false;
    }

  if (!QFile::exists(fileName))
    {
      piiWarning(tr("Configuration file %1 doesn't exists.").arg(fileName));
      return false;
    }
  
  QSettings configSettings(fileName, QSettings::IniFormat);
  QStringList allKeys = configSettings.allKeys();
  for (int i=0; i<allKeys.size(); i++)
    d->pCameraDriver->setProperty(qPrintable(allKeys[i]), configSettings.value(allKeys[i]));
  
  return true;
}

bool PiiCameraOperation::saveCameraConfig(const QString& fileName)
{
  PII_D;

  if (d->pCameraDriver == 0)
    {
      piiWarning(tr("Camera driver has not been set."));
      return false;
    }

  QSettings configSettings(fileName, QSettings::IniFormat);
  configSettings.clear();

  QList<QPair<QString, QVariant> > lstProperties = Pii::propertyList(d->pCameraDriver, 1, Pii::WritableProperties);
  for (int i=0; i<lstProperties.size(); i++)
    configSettings.setValue(lstProperties[i].first, lstProperties[i].second);
  
  return true;
}

void PiiCameraOperation::setDriverName(const QString& driverName)
{
  PII_D;
  PiiCameraDriver *pCameraDriver = PiiYdin::createResource<PiiCameraDriver>(driverName);
  if (pCameraDriver != 0)
    {
      if (d->pCameraDriver != 0)
        {
          d->pCameraDriver->close();
          delete d->pCameraDriver;
        }
      d->pCameraDriver = pCameraDriver;
      d->pCameraDriver->setObjectName("driver");
      d->pCameraDriver->setParent(this);
      d->pCameraDriver->setListener(this);
    }
  else
    piiWarning(tr("Camera driver %1 is not available.").arg(driverName));
}

QString PiiCameraOperation::driverName() const
{
  if (_d()->pCameraDriver != 0)
    return _d()->pCameraDriver->metaObject()->className();

  return QString();
}

QObject* PiiCameraOperation::driver() const
{
  return _d()->pCameraDriver;
}

QStringList PiiCameraOperation::cameraList() const
{
  try
    {
      if (_d()->pCameraDriver != 0)
        return _d()->pCameraDriver->cameraList();
    }
  catch (PiiCameraDriverException& ex)
    {
      piiWarning(ex.message());
    }

  return QStringList();
}

void PiiCameraOperation::setCameraId(const QString& cameraId)
{
  _d()->strCameraId = cameraId;
}

QString PiiCameraOperation::cameraId() const
{
  return _d()->strCameraId;
}

void PiiCameraOperation::framesMissed(int /*startIndex*/, int endIndex)
{
  PII_D;
  d->iMaxMissedIndex = endIndex;
  d->bMissedFrames = true;
}

void PiiCameraOperation::captureFinished(bool state)
{
  if (!state)
    operationStopped();
}

void PiiCameraOperation::captureError(const QString& message)
{
  piiWarning(tr("Error in capturing image: %1").arg(message));
}


void PiiCameraOperation::setCopyImage(bool copy)
{
  _d()->bCopyImage = copy;
}

bool PiiCameraOperation::copyImage() const
{
  return _d()->bCopyImage;
}

template <class T> PiiMatrix<PiiColor<T> > PiiCameraOperation::yuv411toRgb(void *frameBuffer, int width, int height)
{
  PiiMatrix<PiiColor<T> > matrix(height, width);

  int y1, y2, y3, y4, u, v;
  int length = 6*width*height/4;
  int index = 0;
  PiiColor<T> *pData = matrix.row(0);
  T *pBuffer = static_cast<T*>(frameBuffer);
  
  for (int i=0; i<length; i+=6, index+=4)
    {
      u = pBuffer[i+0] - 128;
      y1 = pBuffer[i+1];
      y2 = pBuffer[i+2];
      v = pBuffer[i+3] - 128;
      y3 = pBuffer[i+4];
      y4 = pBuffer[i+5];

      yuvToRgb<T>(&pData[index], y1,u,v);
      yuvToRgb<T>(&pData[index+1], y2,u,v);
      yuvToRgb<T>(&pData[index+2], y3,u,v);
      yuvToRgb<T>(&pData[index+3],y4,u,v);
    }
  
  return matrix;
}

template <class T> PiiMatrix<PiiColor<T> > PiiCameraOperation::yuv422toRgb(void *frameBuffer, int width, int height)
{
  PiiMatrix<PiiColor<T> > matrix(height, width);

  int y1, y2, u, v;
  int length = 2*width*height;
  int index = 0;
  PiiColor<T> *pData = matrix.row(0);
  T *pBuffer = static_cast<T*>(frameBuffer);
  
  for (int i=0; i<length; i+=4, index+=2)
    {
      y1 = pBuffer[i+0];
      u = pBuffer[i+1] - 128;
      y2 = pBuffer[i+2];
      v = pBuffer[i+3] - 128;

      yuvToRgb<T>(&pData[index], y1,u,v);
      yuvToRgb<T>(&pData[index+1],y2,u,v);
    }
  return matrix;
}

template <class T> void PiiCameraOperation::yuvToRgb(PiiColor<T>* data, int y, int u, int v)
{
  data->c0 = (T)qBound(0, int(y + 1.370705*v), 255);
  data->c1 = (T)qBound(0, int(y - 0.698001 * v - 0.337633*u), 255);
  data->c2 = (T)qBound(0, int(y + 1.732446*u), 255);
}
  
