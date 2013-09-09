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

#ifndef _PIICAMERAOPERATION_H
#define _PIICAMERAOPERATION_H

#include <PiiImageReaderOperation.h>
#include <PiiCameraDriver.h>
#include <PiiWaitCondition.h>
#include <PiiTimer.h>

/**
 * PiiCameraOperation description
 *
 * @ingroup PiiCameraPlugin
 */
class PII_CAMERA_EXPORT PiiCameraOperation : public PiiImageReaderOperation, public PiiCameraDriver::Listener
{
  Q_OBJECT

  /**
   * The name of the camera driver to be used. If the driver is not
   * available, the operation will fail to start.
   */
  Q_PROPERTY(QString driverName READ driverName WRITE setDriverName STORED false);

  /**
   * A list of available camera IDs. Note that not all camera drivers
   * provide a camera list. In such cases, the #cameraId property must
   * be set according to the camera driver. Some drivers provide a
   * list of accessible cameras, but still support other cameras as
   * well.
   */
  Q_PROPERTY(QStringList cameraList READ cameraList);

  /**
   * The ID of the selected camera. A list of selectable cameras can
   * be read from #cameraList. The ID of a camera can be any string. 
   * For example, IP camera drivers use a URL to identify the camera.
   */
  Q_PROPERTY(QString cameraId READ cameraId WRITE setCameraId);

  /**
   * If this property is @p true, the operation will make a deep copy
   * of each captured frame. Otherwise, it is up to the driver how the
   * memory is allocated. This mode is usually faster. However,
   * drivers that use a circular frame buffer, will silently overwrite
   * image data if the frame buffer is not big enough. The default
   * value is @p false.
   */
  Q_PROPERTY(bool copyImage READ copyImage WRITE setCopyImage);

  friend struct PiiSerialization::Accessor;
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;
  template <class Archive> void serialize(Archive& archive, const unsigned int)
  {
    PII_SERIALIZE_BASE(archive, PiiOperation);
    PiiSerialization::serializeProperties(archive, *this);
    PII_D;
    archive & PII_NVP("driver", d->pCameraDriver);
    if (Archive::InputArchive && d->pCameraDriver)
      {
        d->pCameraDriver->setParent(this);
        d->pCameraDriver->setListener(this);
      }
  }
public:
  PiiCameraOperation();
  ~PiiCameraOperation();
  
  QVariant property(const char* name) const;
  bool setProperty(const char* name, const QVariant& value);

  void check(bool reset);
  void start();
  void interrupt();
  void pause();
  void stop();
  
  // Listener functions
  void frameCaptured(int frameIndex, void *frameBuffer, qint64 elapsedTime);
  void framesMissed(int startIndex, int endIndex);
  void captureFinished(bool state);
  void captureError(const QString& message);

public slots:
  /**
   * Loads the camera configuration from @a file. There might be the
   * configuration values which needs to start the processing again.
   * If no driver has been loaded yet, this function does nothing.
   * Returns @p true on success, @p false otherwise.
   */
  bool loadCameraConfig(const QString& file);

  /**
   * Saves the camera configuration into @a file. If no driver has
   * been loaded yet, this function does nothing. Returns @p true on
   * success, @p false otherwise.
   */
  bool saveCameraConfig(const QString& file);

signals:
  void framesPerSecond(double frames);

protected:
  void process();

  void setDriverName(const QString& driverName);
  QString driverName() const;
  
  QObject* driver() const;
  QStringList cameraList() const;
  
  void setCameraId(const QString& cameraId);
  QString cameraId() const;

  void setCopyImage(bool copy);
  bool copyImage() const;

  /**
   * Processes an image before delivery. The default implementation
   * returns @a image. Subclasses may add custom functionality by
   * overriding this function.
   */
  virtual PiiVariant processImage(const PiiVariant& image, int frameIndex, qint64 elapsedTime);

  void timerEvent(QTimerEvent*);

private:
  template <class T> void convert(void *frameBuffer, Pii::PtrOwnership ownership, int frameIndex, qint64 elapsedTime);
  template <class T> void emitImage(const PiiMatrix<T>& image, Pii::PtrOwnership ownership, int frameIndex, qint64 elapsedTime);

  template <class T> PiiMatrix<PiiColor<T> > yuv411toRgb(void *frameBuffer, int width, int height);
  template <class T> PiiMatrix<PiiColor<T> > yuv422toRgb(void *frameBuffer, int width, int height);
  template <class T> void yuvToRgb(PiiColor<T> *data, int y, int u, int v);
  
  void init();
protected:
  /// @internal
  class PII_CAMERA_EXPORT Data : public PiiImageReaderOperation::Data
  {
  public:
    Data();
    PiiCameraDriver *pCameraDriver;
    QString strCameraId;
    bool bTriggered;
    PiiWaitCondition waitCondition;
    int iImageWidth;
    int iImageHeight;
    PiiCamera::ImageFormat imageFormat;
    int iBitsPerPixel;
    bool bCopyImage;
    QAtomicInt iFrameCount;
    PiiTimer frameTimer;
    bool bWaitPause, bMissedFrames;
    PiiWaitCondition pauseWaitCondition;
    int iMaxMissedIndex;
    QMutex pauseMutex;
    
  };
  PII_D_FUNC;

  PiiCameraOperation(Data* data);
};


#endif //_PIICAMERAOPERATION_H
