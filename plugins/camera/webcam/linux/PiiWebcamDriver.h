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

#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>

class PII_WEBCAMDRIVER_EXPORT PiiWebcamDriver : public PiiCameraDriver
{
  Q_OBJECT

  /**
   * The size of the frame buffer.
   */
  Q_PROPERTY(int frameBufferCount READ frameBufferCount WRITE setFrameBufferCount);

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
  
  int frameBufferCount() const;
  QRect frameRect() const;

  bool setFrameBufferCount(int frameBufferCount);
  bool setFrameRect(const QRect& frameRect);
  QVariantList frameSizes() const;

private:
  int frameIndex(int frameIndex) const;
  void capture();
  void stopCapturing();

  bool requiresInitialization(const char* name) const;

  bool writeIntValue(const char* name, int value);
  int readConfigValue(const char* name, int defaultValue = 0, bool *ok = 0);
  int readIntValue(const char* name, int defaultValue = 0, bool *ok = 0) const;

  // v4l-functions
  void grabFrame(int fd, void **buffer, int timeout);
  bool requeueBuffers(int fd);
  bool startAcquisition(int fd);
  bool stopAcquisition(int fd);
  bool registerFrameBuffers(int fd);
  bool deregisterFrameBuffers();
  
  QFile _fileDevice;
  QStringList _lstCriticalProperties;
  bool _bOpen, _bCapturingRunning;
  QString _strBaseDir, _strCameraId, _strDevice;

  struct WebcamBuffer {
    v4l2_buffer v4l2Buffer;
    void *frameStart;
  };
  
  int _iFrameBufferCount;
  QVector<WebcamBuffer*> _vecBuffers;
  QVector<v4l2_buffer> _vecReservedBuffers;
  QVector<void*> _vecBufferPointers;
    
  QThread *_pCapturingThread;
  QMutex _frameBufMutex;
  unsigned int _iFrameIndex;
  int _iMaxFrames, _iHandledFrameCount;
  PiiWaitCondition _triggerWaitCondition;
  PiiCameraDriver::TriggerMode _triggerMode;
  bool _bCroppingSupported;
  int _iPixelFormat;
  QSize _resolution;
  int _iBitsPerPixel;
};

#endif //_PIIWEBCAMDRIVER_H
