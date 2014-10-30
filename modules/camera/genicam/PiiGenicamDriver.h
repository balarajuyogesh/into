/* This file is part of Into.
 * Copyright (C) Intopii
 * All rights reserved.
 */

#ifndef _PIIGENICAMDRIVER_H
#define _PIIGENICAMDRIVER_H

//#define BUILD_GENICAM_WAPI
#include "genicam_wrapper.h"

extern "C"
{
  GENICAM_TYPEDEF(int, GenicamIntVoidFunc)();
  GENICAM_TYPEDEF(int, GenicamIntDevicepFunc)(genicam_device*);
  GENICAM_TYPEDEF(int, GenicamIntCharppIntpFunc)(char**,int*);
  GENICAM_TYPEDEF(const char*, GenicamCCharpCCharpFunc)(const char*);
  GENICAM_TYPEDEF(void, GenicamVoidCharpFunc)(char*);
  GENICAM_TYPEDEF(int, GenicamIntCCharpDeviceppFunc)(const char*,genicam_device**);
  GENICAM_TYPEDEF(int, GenicamIntVoidpFunc)(void*);
  GENICAM_TYPEDEF(int, GenicamIntDevicepCCharpIntFunc)(genicam_device*,const char*,int);
  GENICAM_TYPEDEF(int, GenicamIntDevicepCCharpIntpFunc)(genicam_device*,const char*,int*);
  GENICAM_TYPEDEF(int, GenicamIntDevicepUCharppIntFunc)(genicam_device*,unsigned char**,int);
  GENICAM_TYPEDEF(int, GenicamIntDevicepUCharpIntFunc)(genicam_device*,unsigned char*,int);
}

#include <PiiWaitCondition.h>
#include <QThread>
#include <QMutex>
#include <QLibrary>
#include <PiiCameraDriver.h>

/// @internal
class PII_CAMERA_EXPORT PiiGenicamDriver : public PiiCameraDriver
{
  Q_OBJECT

  Q_PROPERTY(int frameBufferCount READ frameBufferCount WRITE setFrameBufferCount);
  Q_PROPERTY(double frameRate READ frameRate WRITE setFrameRate);
  Q_PROPERTY(int exposureTime READ exposureTime WRITE setExposureTime);
  Q_PROPERTY(int exposurePeriod READ exposurePeriod WRITE setExposurePeriod);
  Q_PROPERTY(double inputPulseFrequency READ inputPulseFrequency);
  Q_PROPERTY(double gain READ gain WRITE setGain);
  Q_PROPERTY(QRect frameRect READ frameRect WRITE setFrameRect);
  Q_PROPERTY(int packetSize READ packetSize WRITE setPacketSize);
  Q_PROPERTY(QRect autoExposureArea READ autoExposureArea WRITE setAutoExposureArea);
  Q_PROPERTY(int autoExposureTarget READ autoExposureTarget WRITE setAutoExposureTarget);
  Q_PROPERTY(bool flipHorizontally READ flipHorizontally WRITE setFlipHorizontally);
  Q_PROPERTY(int maxHeight READ maxHeight);

  /**
   * Read the size of the sensor.
   */
  Q_PROPERTY(QSize sensorSize READ sensorSize);

  /**
   * We will convert encoder-frequency by the triggerRate.
   */
  Q_PROPERTY(double triggerRate READ triggerRate WRITE setTriggerRate);

  /**
   * -1 means that we will use ShaftEncoder -mode, where Pulse A is
   * the first input line and Pulse B is the second input line. If
   * triggerLine >= 0 we will receive trigger-signals from the input
   * 'triggerLine'.
   */
  Q_PROPERTY(int triggerLine READ triggerLine WRITE setTriggerLine);

  PII_DEFAULT_SERIALIZATION_FUNCTION(PiiCameraDriver)
protected:
  PiiGenicamDriver(const QString& wrapperLibrary);
  ~PiiGenicamDriver();

public:
  QStringList cameraList() const;
  void initialize(const QString& cameraId);
  bool close();
  bool startCapture(int frames);
  bool stopCapture();
  void* frameBuffer(uint frameIndex) const;
  bool isOpen() const;
  bool isCapturing() const;
  bool triggerImage();
  bool requiresInitialization(const char* name) const;
  bool setTriggerMode(PiiCameraDriver::TriggerMode mode);
  PiiCameraDriver::TriggerMode triggerMode() const;
  int bitsPerPixel() const;
  int cameraType() const;
  QSize frameSize() const;
  bool setFrameSize(const QSize& frameSize);
  int imageFormat() const;
  bool setImageFormat(int format);
  QSize resolution() const;

  int frameBufferCount() const;
  double frameRate() const;
  int exposureTime() const;
  int exposurePeriod() const;
  double inputPulseFrequency() const;
  double gain() const;
  QRect frameRect() const;
  int packetSize() const;
  QRect autoExposureArea() const;
  int autoExposureTarget() const;
  bool flipHorizontally() const;
  int maxHeight() const;
  QSize sensorSize() const;
  double triggerRate() const;
  int triggerLine() const;

  bool setFrameBufferCount(int frameBufferCount);
  bool setFrameRate(double frameRate);
  bool setExposureTime(int exposureTime);
  bool setExposurePeriod(int exposurePeriod);
  bool setGain(double gain);
  bool setFrameRect(const QRect& frameRect);
  bool setPacketSize(int packetSize);
  bool setAutoExposureArea(const QRect& autoExposureArea);
  bool setAutoExposureTarget(int autoExposureTarget);
  bool setFlipHorizontally(bool flipHorizontally);
  bool setTriggerRate(double triggerRate);
  bool setTriggerLine(int triggerLine);

  QVariant property(const char* name) const;
  bool setProperty(const char* name, const QVariant& value);

protected:
  template <class T> T resolveLib(QLibrary& lib, const QString& name);
  void initializeWrapperFunctions();
  void initializeGenicamDevice(const QString& camId);

  QString _strWrapperLibrary;
  bool _bWrapperFunctionsInitialized;

  GenicamIntVoidFunc genicamInitialize;
  GenicamIntVoidFunc genicamTerminate;
  GenicamIntCharppIntpFunc genicamListCameras;
  GenicamCCharpCCharpFunc genicamNextCamera;
  GenicamVoidCharpFunc genicamLastError;
  GenicamIntCCharpDeviceppFunc genicamOpenDevice;
  GenicamIntDevicepFunc genicamCloseDevice;
  GenicamIntDevicepFunc genicamReconnectDevice;
  GenicamIntVoidpFunc genicamFree;
  GenicamIntDevicepCCharpIntFunc genicamSetProperty;
  GenicamIntDevicepCCharpIntpFunc genicamGetProperty;
  GenicamIntDevicepUCharpIntFunc genicamRegisterFramebuffers;
  GenicamIntDevicepFunc genicamDeregisterFramebuffers;
  GenicamIntDevicepUCharppIntFunc genicamGrabFrame;
  GenicamIntDevicepFunc genicamRequeueBuffers;
  GenicamIntDevicepFunc genicamStartCapture;
  GenicamIntDevicepFunc genicamStopCapture;

  void capture();
  bool reconnect();

  genicam_device* _pDevice;
  unsigned char* _pBuffer;

  QStringList _lstCriticalProperties;
  bool _bOpen, _bCapturingRunning;
  QString _strCameraId;

  QVector<unsigned char*> _vecBufferPointers;

  QThread *_pCapturingThread;
  unsigned int _iFrameIndex;
  int _iMaxFrames, _iHandledFrameCount;
  PiiWaitCondition _triggerWaitCondition;
  TriggerMode _triggerMode;
  int _iFrameBufferCount;
  mutable QMutex _reconnectMutex;

private:
  QString lastError() const;
  int readIntValue(const char* name, int defaultValue = 0, bool *ok = 0) const;
  bool writeIntValue(const char* name, int value);
};

#endif //_PIIGENICAMDRIVER_H
