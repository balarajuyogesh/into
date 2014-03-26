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

#ifndef _PIIQMLIMAGEPROVIDER_H
#define _PIIQMLIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <PiiGlobal.h>

class PiiVariant;
class PiiAbstractOutputSocket;
class PiiProbeInput;

/**
 * An image provider that stores images coming from operations and
 * converts PiiMatrix instances to QImages.
 *
 * ~~~(qml)
 * Image
 * {
 *   id: image
 *   cache: false
 *
 *   // See documentation of the requestImage() function below.
 *   // If you don't need id, you can leave the parameter out.
 *   function updateImage(id)
 *   {
 *     if (!source.toString())
 *       source = "image://Into/" + id + "#0";
 *     else
 *       source = source.toString().replace(/#(.*)$/, function (a,b) { return "#" + (parseInt(b)+1); });
 *   }
 * }
 *
 * // Later in code
 * Into.PiiImageProvider.connectOutput(engine.output("imageSource.image"), "imageSource.image");
 * Into.PiiImageProvider.setListener("imageSource.image", image);
 * ~~~
 */
class PiiQmlImageProvider :
  public QObject,
  public QQuickImageProvider
{
  Q_OBJECT
public:
  typedef QImage (*Converter)(const PiiVariant& variant);

  PiiQmlImageProvider();
  ~PiiQmlImageProvider();

  /**
   * Returns the last image saved to *slot*. Stores the original size
   * of the image to *size* and scales the image to *requestedSize*.
   *
   * ! The QtQuick Image component provides no way of updating a
   *   displayed image. The only way to force an update is to change
   *   the `source` property, but if one just wants to update the
   *   image to show the current contents of an updated image, the URL
   *   of the source is the same. To make it possible to update an
   *   image without actually changing its URL you can append a hash
   *   (#) and an arbitrary string to *slot*. The suffix will be
   *   ignored by this function, but forces QML to reload the image.
   */
  QImage requestImage(const QString& slot, QSize* size, const QSize& requestedSize);

  /**
   * Sets a custom converter function for the given *typeId*.
   * PiiQmlImageProvider converts [PiiMatrix] instances to QImages.
   * All other types are passed to a custom converter if such a
   * converter exists.
   */
  static void setConverter(int typeId, Converter converter);

public slots:
  /**
   * Stores *image* to the given *slot*. If the slot doesn't exist
   * yet, creates a new slot. If *image* is invalid, the slot will be
   * cleared. If the slot has an associated listener, storing an image
   * always invokes the listener's `updateImage()` function. In other
   * words, [setUpdateInterval()] has no effect when this function is
   * called directly. If the type of *image* cannot be recognized,
   * returns `false`.
   */
  bool storeImage(const QString& slot, const PiiVariant& image);

  /**
   * Removes the given storage *slot* and unregisters its listener. If
   * there was an output connected to the slot, it will be
   * disconnected.
   */
  void removeSlot(const QString& slot);

  /**
   * Connects *output* to the given storage *slot*. Every image
   * received from *output* will be stored to the slot. Returns
   * `false` if *output* is null or if the slot is already connected
   * to an output, and `true` otherwise.
   */
  bool connectOutput(PiiAbstractOutputSocket* output, const QString& slot);

  /**
   * Disconnects a previously connected output form the given
   * *slot*.
   */
  void disconnectOutput(const QString& slot);

  /**
   * Sets a *listener* object to the given *slot*. The listener's
   * `updateImage()` method will be invoked whenever a new image is
   * stored to the slot. The name of the slot will be passed as the
   * only parameter to the `updateImage()` function. If the slot
   * doesn't exist, a new slot will be created. If *listener* doesn't
   * have an `updateImage(QVariant)` function, returns
   * `false`. Otherwise returns `true`.
   */
  bool setListener(const QString& slot, QObject* listener);

  /**
   * Removes listener from *slot*. Same as `setListener(slot, 0)`.
   */
  void removeListener(const QString& slot);

  /**
   * Returns the listener in *slot* or 0 if there is no listener.
   */
  QObject* listener(const QString& slot) const;

  /**
   * Sets the minimum time between calls to a listener's updateImage()
   * in the given *slot*. Returns `true` if successful and `false`, if
   * no output has been connected to *slot*.
   */
  bool setUpdateInterval(const QString& slot, int millisecs);

  /**
   * Returns the current update interval of the listener in *slot*. If
   * there is no output connected to the slot or the interval has not
   * been set, returns 0.
   */
  int updateInterval(const QString& slot) const;

private slots:
  void storeImage(const PiiVariant& image, PiiProbeInput* sender);
  void removeListener(QObject* listener);
  void disconnectOutput(QObject* output);

private:
  template <class T> inline QImage matrixToQImage(const PiiVariant& image);

  typedef QMap<int,Converter> ConverterMap;
  static ConverterMap* converterMap();

  struct Slot;
  class Data;
  Data* d;
  PII_DISABLE_COPY(PiiQmlImageProvider);
};

#endif //_PIIQMLIMAGEPROVIDER_H
