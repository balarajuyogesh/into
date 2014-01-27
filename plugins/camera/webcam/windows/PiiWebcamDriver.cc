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

template <class T> void safeRelease(T **ppT)
{
  if (*ppT)
    {
      (*ppT)->Release();
      *ppT = NULL;
    }
}

IPin* getPin(IBaseFilter *filter, PIN_DIRECTION direction, int num)
{
  IPin *pRetPin = NULL;
  IEnumPins *pEnumPins;
  if (FAILED(filter->EnumPins(&pEnumPins)))
    {
      qDebug("Error getting pin enumerator");
      return NULL;
    }

  ULONG found;
  IPin *pPin;
  while (pEnumPins->Next(1, &pPin, &found) == S_OK)
    {
      PIN_DIRECTION pinDirection = (PIN_DIRECTION)(-1);
      pPin->QueryDirection(&pinDirection);
      if (pinDirection == direction)
        {
          if (num == 0)
            {
              pRetPin = pPin;
              break;
            }
          num--;
        }
    }
  return pRetPin;
}

PiiWebcamDriver::PiiWebcamDriver() : _bInitialized(false),
                                     _pGraphBuilder(0),
                                     _pMediaControl(0),
                                     _pMediaEvent(0),
                                     _pSourceFilter(0),
                                     _pSourceFilterOut(0),
                                     _pStreamConfig(0),
                                     _pCallback(0),
                                     _resolution(0,0),
                                     _frameSize(0,0),
                                     _imageFormat(PiiCamera::InvalidFormat),
                                     _bOpen(false),
                                     _bCapturingRunning(false),
                                     _strCameraId(""),
                                     _pCapturingThread(0),
                                     _iFrameIndex(-1),
                                     _iMaxFrames(0),
                                     _iBitsPerPixel(8)
{
  _lstCriticalProperties << "imageFormat"
                         << "frameRect"
                         << "frameSize"
                         << "triggerMode";
}


PiiWebcamDriver::~PiiWebcamDriver()
{
  close();
  //delete[] _pBuffer;
  CoUninitialize();
}

QStringList PiiWebcamDriver::cameraList() const
{
  const_cast<PiiWebcamDriver*>(this)->initialize();

  if (!_bInitialized)
    return QStringList();

  ICreateDevEnum *pSysDevEnum = NULL;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                                IID_ICreateDevEnum, (void **)&pSysDevEnum);
  if (FAILED(hr))
    {
      piiWarning(tr("Couldn't get device list."));
      return QStringList();
    }

  QStringList lstCameras;

  // Obtain a class enumerator for the video compressor category.
  IEnumMoniker *pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

  if (hr == S_OK && SUCCEEDED(pEnumCat->Reset()))
    {
      // Enumerate the monikers.
      IMoniker *pMoniker = NULL;
      ULONG cFetched;
      while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
        {
          IPropertyBag *pPropBag;
          hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
          if (SUCCEEDED(hr))
            {
              // To retrieve the filter's friendly name, do the following:
              VARIANT varName, varId;
              VariantInit(&varName);
              VariantInit(&varId);
              if (SUCCEEDED(pPropBag->Read(L"FriendlyName", &varName, 0)) &&
                  SUCCEEDED(pPropBag->Read(L"DevicePath", &varId, 0)))
                {
                  lstCameras << QString("%1|%2")
                    .arg(QString::fromStdWString(varId.bstrVal))
                    .arg(QString::fromStdWString(varName.bstrVal));
                }
              VariantClear(&varName);
              VariantClear(&varId);
            }
          safeRelease(&pPropBag);
          safeRelease(&pMoniker);
        }
    }
  safeRelease(&pEnumCat);
  safeRelease(&pSysDevEnum);
  return lstCameras;
}

bool PiiWebcamDriver::initCamera(const QString& cameraId)
{
  ICreateDevEnum *pSysDevEnum = NULL;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                                IID_ICreateDevEnum, (void **)&pSysDevEnum);
  if (FAILED(hr))
    {
      piiWarning(tr("Couldn't ini device enumerator."));
      return false;
    }

  // Obtain a class enumerator for the video compressor category.
  IEnumMoniker *pEnumCat = NULL;
  hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
  safeRelease(&pSysDevEnum);

  if (hr == S_OK && SUCCEEDED(pEnumCat->Reset()))
    {
      // Enumerate the monikers.
      IMoniker *pMoniker = NULL;
      ULONG cFetched;
      while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
        {
          IPropertyBag *pPropBag;
          hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
          if (SUCCEEDED(hr))
            {
              // To retrieve the filter's friendly name, do the following:
              VARIANT varId;
              VariantInit(&varId);
              if (SUCCEEDED(pPropBag->Read(L"DevicePath", &varId, 0)))
                {
                  QString strId = QString::fromStdWString(varId.bstrVal);
                  VariantClear(&varId);

                  if (cameraId.contains(strId))
                    {
                      safeRelease(&pPropBag);
                      safeRelease(&pEnumCat);

                      if (FAILED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&_pSourceFilter)))
                        {
                          safeRelease(&pMoniker);
                          return false;
                        }
                      safeRelease(&pMoniker);

                      if (FAILED(_pGraphBuilder->AddFilter(_pSourceFilter, L"Source Filter")) ||
                          ((_pSourceFilterOut = getPin(_pSourceFilter, PINDIR_OUTPUT, 0)) == NULL) ||
                          FAILED(_pSourceFilterOut->QueryInterface(IID_IAMStreamConfig, (void**)&_pStreamConfig)))
                        {
                          safeRelease(&_pSourceFilterOut);
                          safeRelease(&_pSourceFilter);
                          return false;
                        }

                      if (FAILED(_pGraphBuilder->Connect(_pSourceFilterOut, _pGrabberIn)))
                        {
                          safeRelease(&_pStreamConfig);
                          safeRelease(&_pSourceFilterOut);
                          safeRelease(&_pSourceFilter);
                          return false;
                        }


                      return true;
                    }
                }
            }
          safeRelease(&pPropBag);
          safeRelease(&pMoniker);
        }
    }
  safeRelease(&pEnumCat);

  return false;
}

void PiiWebcamDriver::initialize(const QString& cameraId)
{
  piiDebug("PiiWebcamDriver::initialize(%s)", qPrintable(cameraId));

  if (_bCapturingRunning)
    PII_THROW(PiiCameraDriverException, tr("Capturing is running. Stop the capture first."));

  QStringList lstCameras = cameraList();
  if (!_bInitialized)
    PII_THROW(PiiCameraDriverException, tr("Couldn't initialize camera driver."));
  if (lstCameras.isEmpty())
    PII_THROW(PiiCameraDriverException, tr("Couldn't find any cameras. Cannot initialize %1.").arg(cameraId));

  QString strCameraId = cameraId.isEmpty() ? lstCameras[0] : cameraId;
  bool bCameraOk = false;
  for (int i=0; i<lstCameras.size(); i++)
    {
      if (lstCameras[i].contains(strCameraId))
        {
          bCameraOk = true;
          break;
        }
    }

  if (!bCameraOk)
    PII_THROW(PiiCameraDriverException, tr("Couldn't find camera by id %1").arg(strCameraId));

  // Check if we must close the driver
  if (strCameraId != _strCameraId)
    close();

  // Get property map
  QVariantMap& dataMap = propertyMap();

  // Check if we must open the device
  if (!_bOpen)
    {
      if (!initGraphBuilder())
        PII_THROW(PiiCameraDriverException, tr("Couldn't initialize graph builder."));

      if (!initCamera(strCameraId))
        PII_THROW(PiiCameraDriverException, tr("Couldn't initialize camera by id '%1'.").arg(strCameraId));

      if (!initSupportedValues())
        PII_THROW(PiiCameraDriverException, tr("Coulnd't initialize supported values."));

      qDebug() << _lstImageFormats;
      qDebug() << _lstFrameSizes;
      qDebug() << _resolution;

      if (!dataMap.contains("triggerMode") && !setTriggerMode(PiiCameraDriver::FreeRun))
        PII_THROW(PiiCameraDriverException, tr("Could not set default triggerMode."));
    }

  _strCameraId = strCameraId;

  // Write all configuration values from the map
  for (QVariantMap::iterator i=dataMap.begin(); i != dataMap.end(); ++i)
    {
      if (!QObject::setProperty(qPrintable(i.key()), i.value()))
        PII_THROW(PiiCameraDriverException, tr("Couldn't write the configuration value '%1'").arg(i.key()));
    }
  dataMap.clear();

  if (!initSelectedValues())
    PII_THROW(PiiCameraDriverException, tr("Coulnd't initialize selected values."));

  _pFrame = 0;
  _bOpen = true;
}


bool PiiWebcamDriver::initSupportedValues()
{
  _lstFrameSizes.clear();
  _lstImageFormats.clear();
  _resolution = QSize(0,0);

  if (_pStreamConfig != 0)
    {
      int count, size;
      if (FAILED(_pStreamConfig->GetNumberOfCapabilities( &count, &size)))
        return false;

      // Search MEDIATYPE_Video and get supported formats and sizes
      VIDEO_STREAM_CONFIG_CAPS videoConfig;
      AM_MEDIA_TYPE *pMediaType;
      for (int i=0; i<count; i++)
        {
          if (FAILED(_pStreamConfig->GetStreamCaps(i, &pMediaType, (BYTE *)&videoConfig)))
            {
              _lstFrameSizes.clear();
              _lstImageFormats.clear();
              _resolution = QSize(0,0);
              return false;
            }

          if (pMediaType->majortype == MEDIATYPE_Video && pMediaType->cbFormat != 0)
            {
              PiiCamera::ImageFormat format = PiiCamera::InvalidFormat;

              // Get image format
              if (pMediaType->subtype == MEDIASUBTYPE_YUY2)
                format = PiiCamera::Yuv422Format;
              else if (pMediaType->subtype == MEDIASUBTYPE_RGB24)
                format = PiiCamera::RgbFormat;
              else
                format = PiiCamera::InvalidFormat;

              if (format != PiiCamera::InvalidFormat && !_lstImageFormats.contains(format))
                _lstImageFormats << format;

              // Get frame size
              QSize size = QSize(((VIDEOINFOHEADER*)pMediaType->pbFormat)->bmiHeader.biWidth,
                                 ((VIDEOINFOHEADER*)pMediaType->pbFormat)->bmiHeader.biHeight);
              if (!_lstFrameSizes.contains(size))
                _lstFrameSizes << size;

              if (size.width() * size.height() > _resolution.width() * _resolution.height())
                _resolution = size;
            }

          if (pMediaType->cbFormat != 0)
            CoTaskMemFree((PVOID)pMediaType->pbFormat);
          if (pMediaType->pUnk != NULL )
            pMediaType->pUnk->Release();
          CoTaskMemFree((PVOID)pMediaType);
        }
    }

  if (_lstImageFormats.isEmpty())
    {
      piiWarning(tr("Doesn't support any right formats."));
      return false;
    }

  return true;
}

bool PiiWebcamDriver::initSelectedValues()
{
  if (_imageFormat == PiiCamera::InvalidFormat)
    _imageFormat = _lstImageFormats.first();

  if (_frameSize.isNull() || _frameSize.isEmpty())
    _frameSize = _resolution;

  if (!_lstImageFormats.contains(_imageFormat))
    {
      piiWarning(tr("Doesn't support image format %1. We select %1.").arg(_imageFormat).arg(_lstImageFormats.first()));
      _imageFormat = _lstImageFormats.first();
    }
  if (!_lstFrameSizes.contains(_frameSize))
    {
      piiWarning(tr("Doesn't support frame size %1x%2.").arg(_frameSize.width()).arg(_frameSize.height()));
      for (int i=_lstFrameSizes.size(); i--;)
        {
          if (_frameSize.width() > _lstFrameSizes[i].toSize().width())
            {
              _frameSize = _lstFrameSizes[i].toSize();
              break;
            }
        }

      if (!_lstFrameSizes.contains(_frameSize))
        _frameSize = _lstFrameSizes.first().toSize();
      piiWarning(tr("We select the size %1x%2.").arg(_frameSize.width()).arg(_frameSize.height()));
    }

  if (_pStreamConfig != 0)
    {
      int count, size;
      if (FAILED(_pStreamConfig->GetNumberOfCapabilities(&count, &size)))
        return false;

      // Search MEDIATYPE_Video and get supported formats and sizes
      VIDEO_STREAM_CONFIG_CAPS videoConfig;
      AM_MEDIA_TYPE *pMediaType;
      GUID subtype;
      if (_imageFormat == PiiCamera::Yuv422Format)
        {
          _iBitsPerPixel = 8;
          subtype = MEDIASUBTYPE_YUY2;
        }
      else if (_imageFormat == PiiCamera::RgbFormat)
        {
          _iBitsPerPixel = 24;
          subtype = MEDIASUBTYPE_RGB24;
        }
      else
        {
          _iBitsPerPixel = 8;
          subtype = MEDIASUBTYPE_YUY2;
        }

      int iWidth = _frameSize.width();
      int iHeight = _frameSize.height();
      bool bOk = false;
      bool bFound = false;

      for (int i=0; i<count; i++)
        {
          if (FAILED(_pStreamConfig->GetStreamCaps(i, &pMediaType, (BYTE *)&videoConfig)))
              return false;

          if (pMediaType->majortype == MEDIATYPE_Video &&
              pMediaType->cbFormat != 0 &&
              ((VIDEOINFOHEADER*)pMediaType->pbFormat)->bmiHeader.biWidth == iWidth &&
              ((VIDEOINFOHEADER*)pMediaType->pbFormat)->bmiHeader.biHeight == iHeight)
            {
              bOk = true;

              if (FAILED(_pStreamConfig->SetFormat(pMediaType)))
                {
                  piiWarning(tr("Couldn't initialize selected values."));
                  bOk = false;
                }

              bFound = true;
            }

          if (pMediaType->cbFormat != 0)
            CoTaskMemFree((PVOID)pMediaType->pbFormat);
          if (pMediaType->pUnk != NULL )
            pMediaType->pUnk->Release();
          CoTaskMemFree((PVOID)pMediaType);

          if (bFound)
            break;
        }

      return bOk;
    }

  return false;
}

bool PiiWebcamDriver::close()
{
  qDebug("PiiWebcamDriver::close()");
  if (!_bOpen)
    return false;

  stopCapture();

  delete _pCapturingThread;
  _pCapturingThread = 0;

  _bOpen = false;

  return true;
}

bool PiiWebcamDriver::setTriggerMode(PiiCameraDriver::TriggerMode mode)
{
  _triggerMode = mode;
  _pSampleGrabber->SetOneShot(mode == PiiCameraDriver::SoftwareTrigger ? TRUE : FALSE);
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
  if (!_bOpen || listener() == 0 || _bCapturingRunning)
    return false;

  // Create and start the capturing threads
  if (_pCapturingThread == 0)
    _pCapturingThread = Pii::createAsyncCall(this, &PiiWebcamDriver::capture);

  _bCapturingRunning = true;
  _iFrameIndex = -1;
  _iMaxFrames = _triggerMode == PiiCameraDriver::SoftwareTrigger ? 0 : frames;

  // Start acquisition
  if (!startAcquisition())
    {
      piiWarning(tr("Couldn't start acquisition"));
      return false;
    }

  _pCapturingThread->start();

  return true;
}

bool PiiWebcamDriver::stopCapture()
{
  if (!_bCapturingRunning)
    return false;

  // Stop the capturing threads
  stopCapturing();

  return true;
}

void PiiWebcamDriver::stopCapturing()
{
  _bCapturingRunning = false;
  _triggerWaitCondition.wakeAll();
  _pCapturingThread->wait();
}

void PiiWebcamDriver::capture()
{
  _pCapturingThread->setPriority(QThread::HighestPriority);

  bool bSoftwareTrigger = _triggerMode == PiiCameraDriver::SoftwareTrigger;

  while (_bCapturingRunning)
    {
      if (bSoftwareTrigger)
        {
          _triggerWaitCondition.wait();
          _pMediaControl->Run();
          long evCode = 0;
          _pMediaEvent->WaitForCompletion(1000, &evCode);
        }
      else
        QThread::yieldCurrentThread();

      if (!_bCapturingRunning)
        break;

      if (_pFrame != 0)
        {
          void *pSource = _pFrame;
          int iBytes = _iFrameSizeInBytes;
          _pFrame = 0;

          //memcpy _pFrame
          void *pDestination = malloc(iBytes);
          memcpy(pDestination, pSource, iBytes);
          listener()->frameCaptured(++_iFrameIndex, pDestination, 0);
        }
      else
        listener()->frameCaptured(-1,0,0);

      // Check if we must stop capturing
      if (_iMaxFrames > 0 && _iFrameIndex >= _iMaxFrames)
        _bCapturingRunning = false;
    }

  // Stop acquisition
  if (!stopAcquisition())
    piiWarning(tr("Error in stop acquisition"));

  // Inform listener
  listener()->captureFinished(true);
}

bool PiiWebcamDriver::startAcquisition()
{
  qDebug("PiiWebcamDriver::startAcquisition()");
  if (_pMediaControl != 0 && _triggerMode != PiiCameraDriver::SoftwareTrigger)
    _pMediaControl->Run();

  return true;
}

bool PiiWebcamDriver::stopAcquisition()
{
  qDebug("PiiWebcamDriver::stopAcquisition()");
  if (_pMediaControl != 0)
    {
      _pMediaControl->Stop();
      long evCode = 0;
      _pMediaEvent->WaitForCompletion(INFINITE, &evCode);
    }

  return true;
}

void* PiiWebcamDriver::frameBuffer(int index) const
{
  return 0;
}

bool PiiWebcamDriver::isOpen() const
{
  return _bOpen;
}

bool PiiWebcamDriver::isCapturing() const
{
  return _bCapturingRunning;
}

bool PiiWebcamDriver::requiresInitialization(const char* name) const
{
  if (_lstCriticalProperties.contains(QString(name)))
    return true;

  return PiiCameraDriver::requiresInitialization(name);
}

QSize PiiWebcamDriver::frameSize() const
{
  return _frameSize;
}

QRect PiiWebcamDriver::frameRect() const
{
  return QRect(0,0,_frameSize.width(), _frameSize.height());
}

int PiiWebcamDriver::imageFormat() const
{
  return _imageFormat;
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
  return _lstFrameSizes;
}

bool PiiWebcamDriver::setFrameSize(const QSize& frameSize)
{
  _frameSize = frameSize;
  return true;
}

bool PiiWebcamDriver::setFrameRect(const QRect& frameRect)
{
  piiWarning(tr("Doesn't support to set frameRect"));
  return true;
}

bool PiiWebcamDriver::setImageFormat(int value)
{
  _imageFormat = PiiCamera::ImageFormat(value);
  return true;
}

/**
 * DirectShow functions
 */
void PiiWebcamDriver::initialize()
{
  HRESULT hr = CoInitialize(NULL);
  if (FAILED(hr))
    {
      piiWarning(tr("Cannot initialize COM-object system."));
      _bInitialized = false;
    }
  else
    _bInitialized = true;
}

bool PiiWebcamDriver::initGraphBuilder()
{
  _pCallback = new Callback(this);

  // Create the Filter Graph Manager.
  if (FAILED(CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_pGraphBuilder)) ||
      FAILED(_pGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&_pMediaControl)) ||
      FAILED(_pGraphBuilder->QueryInterface(IID_IMediaEvent,(void **)&_pMediaEvent)) ||
      FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&_pGrabberBase)) ||
      FAILED(_pGraphBuilder->AddFilter(_pGrabberBase, L"Grabber")) ||
      FAILED(_pGrabberBase->QueryInterface(IID_ISampleGrabber, (void**)&_pSampleGrabber)) ||
      FAILED(_pSampleGrabber->SetBufferSamples(TRUE)) ||
      FAILED(_pSampleGrabber->SetCallBack(_pCallback,0)) ||
      ((_pGrabberIn = getPin(_pGrabberBase, PINDIR_INPUT,0)) == NULL))
    {
      delete _pCallback;
      _pCallback = 0;
      safeRelease(&_pSampleGrabber);
      safeRelease(&_pGrabberBase);
      safeRelease(&_pMediaEvent);
      safeRelease(&_pMediaControl);
      safeRelease(&_pGraphBuilder);
      return false;
    }

  return true;
}



Callback::Callback(PiiWebcamDriver *parent): m_refCount(1), _pParent(parent)
{
}

Callback::~Callback(void)
{
}

STDMETHODIMP Callback::QueryInterface(REFIID riid, void **ppv)
{
  if (ppv == NULL)
    return E_POINTER;
  return E_NOINTERFACE;
};

STDMETHODIMP_(ULONG) Callback::AddRef(void)
{
  m_refCount++;
  return m_refCount;
}

STDMETHODIMP_(ULONG) Callback::Release(void)
{
  if ( !InterlockedDecrement( &m_refCount ) ) delete this;
  return m_refCount;
}

STDMETHODIMP Callback::SampleCB( double, IMediaSample *s )
{
  BYTE *pBuffer = 0;
  s->GetPointer(&pBuffer);
  _pParent->_pFrame = pBuffer;
  _pParent->_iFrameSizeInBytes = s->GetSize();
  return S_OK;
}

STDMETHODIMP Callback::BufferCB( double, BYTE *, long )
{
  return E_NOTIMPL;
}
