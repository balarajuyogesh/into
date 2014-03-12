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

#ifndef _PIIWEBCAMDRIVER_H
#define _PIIWEBCAMDRIVER_H

#include <PiiCameraDriver.h>
#include <PiiWaitCondition.h>
#include <PiiWebcamDriverGlobal.h>
#include <PiiTimer.h>

#include <QThread>
#include <QMutex>

//For DirectShow
#include <ocidl.h>
#include <dshow.h>

#ifdef __MINGW32__
DEFINE_GUID( CLSID_VideoInputDeviceCategory, 0x860BB310, 0x5D01,
             0x11d0, 0xBD, 0x3B, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);
DEFINE_GUID( CLSID_SampleGrabber, 0xc1f400a0, 0x3f08, 0x11d3,
             0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37 );
DEFINE_GUID( IID_ISampleGrabber, 0x6b652fff, 0x11fe, 0x4fce,
             0x92, 0xad, 0x02, 0x66, 0xb5, 0xd7, 0xc7, 0x8f );
DEFINE_GUID( IID_IAMStreamConfig, 0xc6e13340, 0x30ac, 0x11d0,
             0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56 );
DEFINE_GUID( MEDIASUBTYPE_YV12, 0x32315659, 0x0000, 0x0010,
             0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( MEDIASUBTYPE_IYUV, 0x56555949, 0x0000, 0x0010,
             0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( MEDIASUBTYPE_YUYV, 0x56595559, 0x0000, 0x0010,
             0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( MEDIASUBTYPE_YUY2, 0x32595559, 0x0000, 0x0010,
             0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( MEDIASUBTYPE_UYVY, 0x59565955, 0x0000, 0x0010,
             0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( MEDIASUBTYPE_RGB24, 0xe436eb7d, 0x524f, 0x11ce,
             0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70 );

typedef interface IBaseFilter IBaseFilter;
typedef interface IReferenceClock IReferenceClock;
typedef interface IFilterGraph IFilterGraph;

#define INTERFACE IVideoProcAmp
DECLARE_INTERFACE_(IVideoProcAmp,IUnknown)
{
  STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
  STDMETHOD_(ULONG,AddRef)(THIS) PURE;
  STDMETHOD_(ULONG,Release)(THIS) PURE;
};
#undef INTERFACE

#define INTERFACE IAMStreamConfig
DECLARE_INTERFACE_(IAMStreamConfig,IUnknown)
{
  STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
  STDMETHOD_(ULONG,AddRef)(THIS) PURE;
  STDMETHOD_(ULONG,Release)(THIS) PURE;
  STDMETHOD(SetFormat)(THIS_ AM_MEDIA_TYPE*) PURE;
  STDMETHOD(GetFormat)(THIS_ AM_MEDIA_TYPE**) PURE;
  STDMETHOD(GetNumberOfCapabilities)(THIS_ int*,int*) PURE;
  STDMETHOD(GetStreamCaps)(THIS_ int,AM_MEDIA_TYPE**,BYTE*) PURE;
};
#undef INTERFACE

#define INTERFACE ISampleGrabberCB
DECLARE_INTERFACE_(ISampleGrabberCB,IUnknown)
{
  STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
  STDMETHOD_(ULONG,AddRef)(THIS) PURE;
  STDMETHOD_(ULONG,Release)(THIS) PURE;
  STDMETHOD(SampleCB)(THIS_ double,IMediaSample*) PURE;
  STDMETHOD(BufferCB)(THIS_ double,BYTE*,long) PURE;
};
#undef INTERFACE

#define INTERFACE ISampleGrabber
DECLARE_INTERFACE_(ISampleGrabber,IUnknown)
{
  STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
  STDMETHOD_(ULONG,AddRef)(THIS) PURE;
  STDMETHOD_(ULONG,Release)(THIS) PURE;
  STDMETHOD(SetOneShot)(THIS_ BOOL) PURE;
  STDMETHOD(SetMediaType)(THIS_ const AM_MEDIA_TYPE*) PURE;
  STDMETHOD(GetConnectedMediaType)(THIS_ AM_MEDIA_TYPE*) PURE;
  STDMETHOD(SetBufferSamples)(THIS_ BOOL) PURE;
  STDMETHOD(GetCurrentBuffer)(THIS_ long*,long*) PURE;
  STDMETHOD(GetCurrentSample)(THIS_ IMediaSample**) PURE;
  STDMETHOD(SetCallBack)(THIS_ ISampleGrabberCB *,long) PURE;
};
#undef INTERFACE
#endif

class PiiWebcamDriver;

class Callback: public ISampleGrabberCB
{
public:
  Callback(PiiWebcamDriver *parent);
  virtual ~Callback(void);

  STDMETHODIMP QueryInterface( REFIID riid, void **ppv );
  STDMETHODIMP_(ULONG) AddRef(void);
  STDMETHODIMP_(ULONG) Release(void);
  STDMETHODIMP SampleCB(double,IMediaSample*);
  STDMETHODIMP BufferCB(double,BYTE*,long);

protected:
  long m_refCount;

private:
  PiiWebcamDriver *_pParent;
};

class PII_WEBCAMDRIVER_EXPORT PiiWebcamDriver : public PiiCameraDriver
{
  Q_OBJECT

  /**
   * Set frameRect. QRect(x y width height).
   */
  Q_PROPERTY(QRect frameRect READ frameRect WRITE setFrameRect);

  /**
   * frameSizes description
   */
  Q_PROPERTY(QVariantList frameSizes READ frameSizes);

public:
  /**
   * Construct a new PiiWebcamDriver.
   */
  PiiWebcamDriver();

  /**
   * Destroy the PiiWebcamDriver.
   */
  ~PiiWebcamDriver();

  QStringList cameraList() const;
  void initialize(const QString& cameraId);
  bool close();
  bool startCapture(int frames);
  bool stopCapture();
  void* frameBuffer(int frameIndex) const;
  bool isOpen() const;
  bool isCapturing() const;
  bool triggerImage();
  bool setTriggerMode(PiiCameraDriver::TriggerMode mode);
  PiiCameraDriver::TriggerMode triggerMode() const;
  int bitsPerPixel() const;
  QSize frameSize() const;
  bool setFrameSize(const QSize& frameSize);
  int imageFormat() const;
  bool setImageFormat(int format);
  QSize resolution() const;
  QRect frameRect() const;
  bool setFrameRect(const QRect& frameRect);
  QVariantList frameSizes() const;

private:
  friend class Callback;

  int frameIndex(int frameIndex) const;
  void capture();
  void stopCapturing();

  bool requiresInitialization(const char* name) const;

  // DirectShow functions
  void initialize();
  bool initSupportedValues();
  bool initSelectedValues();
  bool initGraphBuilder();
  bool initCamera(const QString& id);
  bool startAcquisition();
  bool stopAcquisition();

  // General variables
  bool _bInitialized;
  IGraphBuilder *_pGraphBuilder;
  IMediaControl *_pMediaControl;
  IMediaEvent *_pMediaEvent;
  IBaseFilter *_pGrabberBase;
  ISampleGrabber *_pSampleGrabber;
  IPin *_pGrabberIn;
  unsigned char *_pFrame;

  // Device variables
  IBaseFilter *_pSourceFilter;
  IPin *_pSourceFilterOut;
  IAMStreamConfig *_pStreamConfig;
  Callback *_pCallback;

  QVariantList _lstFrameSizes;
  QList<PiiCamera::ImageFormat> _lstImageFormats;
  QSize _resolution, _frameSize;
  PiiCamera::ImageFormat _imageFormat;

  QStringList _lstCriticalProperties;
  bool _bOpen, _bCapturingRunning;
  QString _strCameraId;

  QThread *_pCapturingThread;
  QMutex _frameBufMutex;
  unsigned int _iFrameIndex;
  int _iMaxFrames;
  PiiWaitCondition _triggerWaitCondition;
  PiiCameraDriver::TriggerMode _triggerMode;
  int _iBitsPerPixel;
  int _iFrameSizeInBytes;
};

#endif //_PIIWEBCAMDRIVER_H
