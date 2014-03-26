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

#include "PiiQmlImageProvider.h"

#include <PiiVariant.h>
#include <PiiAbstractOutputSocket.h>
#include <PiiProbeInput.h>
#include <PiiYdinTypes.h>
#include <PiiColor.h>
#include <PiiQImage.h>

#include <QMap>
#include <QMutexLocker>

#define PII_ALL_IMAGE_TYPES                     \
  (PiiYdin::UnsignedCharMatrixType,             \
   PiiYdin::IntMatrixType,                      \
   PiiYdin::FloatMatrixType,                    \
   PiiYdin::UnsignedCharColor4MatrixType,       \
   PiiYdin::UnsignedCharColorMatrixType)

struct PiiQmlImageProvider::Slot
{
  Slot() :
    pProbe(0),
    pListener(0),
    iMethodIndex(-1)
  {}
  Slot(const Slot& other) :
    varImage(other.varImage),
    pProbe(0),
    pListener(other.pListener),
    iMethodIndex(other.iMethodIndex)
  {}

  Slot& operator= (const Slot& other)
  {
    varImage = other.varImage;
    pProbe = other.pProbe;
    pListener = other.pListener;
    iMethodIndex = other.iMethodIndex;
    return *this;
  }

  PiiVariant varImage;
  PiiProbeInput* pProbe;
  QObject* pListener;
  int iMethodIndex;
};

class PiiQmlImageProvider::Data
{
public:
  typedef QMap<QString,Slot> SlotMap;

  Data() {}
  ~Data()
  {
    for (SlotMap::iterator it = mapSlots.begin(); it != mapSlots.end(); ++it)
      delete it->pProbe;
  }

  SlotMap mapSlots;
  QMutex slotMutex;
};

PiiQmlImageProvider::PiiQmlImageProvider() :
  QQuickImageProvider(Image),
  d(new Data)
{}

PiiQmlImageProvider::~PiiQmlImageProvider()
{
  delete d;
}

QImage PiiQmlImageProvider::requestImage(const QString& slot, QSize* size, const QSize& requestedSize)
{
  int iHashIndex = slot.lastIndexOf('#');
  // HACK: The Image QML element doesn't provide a way of updating
  // image data. The only way to force an update is to change the
  // source URL. To facilitate this, everything after a hash (#) in
  // the slot name will be ignored.
  QString strSlot(iHashIndex == -1 ? slot : slot.left(iHashIndex));
  d->slotMutex.lock();
  Data::SlotMap::iterator it = d->mapSlots.find(strSlot);
  if (it == d->mapSlots.end())
    {
      d->slotMutex.unlock();
      return QImage();
    }
  PiiVariant varImage = it->varImage;
  d->slotMutex.unlock();

  QImage qImage;
  switch (varImage.type())
    {
      PII_ALL_IMAGE_CASES(qImage = matrixToQImage, varImage);
    default:
      {
        ConverterMap* pMap = converterMap();
        ConverterMap::iterator it = pMap->find(varImage.type());
        if (it != pMap->end())
          return (*it)(varImage);
      }
      return QImage();
    }

  *size = qImage.size();

  if (requestedSize.isValid() && requestedSize != qImage.size())
    return qImage.scaled(requestedSize);
  return qImage;
}

template <class T> QImage PiiQmlImageProvider::matrixToQImage(const PiiVariant& image)
{
  return Pii::matrixToQImage(image.valueAs<PiiMatrix<T> >());
}

void PiiQmlImageProvider::removeSlot(const QString& slot)
{
  disconnectOutput(slot);
  synchronized (d->slotMutex) d->mapSlots.remove(slot);
}

bool PiiQmlImageProvider::setListener(const QString& slot, QObject* listener)
{
  if (listener == 0)
    removeListener(slot);
  else
    {
      int iMethodIndex = listener->metaObject()->indexOfMethod("updateImage(QVariant)");
      if (iMethodIndex == -1)
        iMethodIndex = listener->metaObject()->indexOfMethod("updateImage()");
      if (iMethodIndex == -1)
        return false;
      synchronized (d->slotMutex)
        {
          Slot& s = d->mapSlots[slot];
          s.pListener = listener;
          s.iMethodIndex = iMethodIndex;
        }
      connect(listener, SIGNAL(destroyed(QObject*)), SLOT(removeListener(QObject*)));
    }
  return true;
}

void PiiQmlImageProvider::removeListener(const QString& slot)
{
  QMutexLocker lock(&d->slotMutex);
  Data::SlotMap::iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end())
    it->pListener = 0;
}

void PiiQmlImageProvider::removeListener(QObject* listener)
{
  QMutexLocker lock(&d->slotMutex);
  for (Data::SlotMap::iterator it = d->mapSlots.begin(); it != d->mapSlots.end(); ++it)
    if (it->pListener == listener)
      {
        it->pListener = 0;
        return;
      }
}

QObject* PiiQmlImageProvider::listener(const QString& slot) const
{
  Data::SlotMap::const_iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end())
    return it->pListener;
  return 0;
}

bool PiiQmlImageProvider::connectOutput(PiiAbstractOutputSocket* socket, const QString& slot)
{
  if (socket == 0) return false;
  QMutexLocker lock(&d->slotMutex);
  Data::SlotMap::iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end())
    delete it->pProbe;
  PiiProbeInput* pProbe = new PiiProbeInput(socket,
                                            this, SLOT(storeImage(PiiVariant,PiiProbeInput*)),
                                            Qt::DirectConnection);
  pProbe->setDiscardControlObjects(true);
  pProbe->setObjectName(slot);
  d->mapSlots[slot].pProbe = pProbe;
  connect(socket, SIGNAL(destroyed(QObject*)), SLOT(disconnectOutput(QObject*)));
  return true;
}

void PiiQmlImageProvider::disconnectOutput(const QString& slot)
{
  QMutexLocker lock(&d->slotMutex);
  Data::SlotMap::iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end())
    {
      delete it->pProbe;
      it->pProbe = 0;
    }
}

void PiiQmlImageProvider::disconnectOutput(QObject* output)
{
  disconnectOutput(output->objectName());
}

bool PiiQmlImageProvider::setUpdateInterval(const QString& slot, int millisecs)
{
  QMutexLocker lock(&d->slotMutex);
  Data::SlotMap::iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end() && it->pProbe != 0)
    {
      it->pProbe->setSignalInterval(millisecs);
      return true;
    }
  return false;
}

int PiiQmlImageProvider::updateInterval(const QString& slot) const
{
  QMutexLocker lock(&d->slotMutex);
  Data::SlotMap::iterator it = d->mapSlots.find(slot);
  if (it != d->mapSlots.end() && it->pProbe != 0)
    return it->pProbe->signalInterval();
  return 0;
}

bool PiiQmlImageProvider::storeImage(const QString& slot, const PiiVariant& image)
{
  if (image.type() member_of<uint> PII_ALL_IMAGE_TYPES)
    {
      QMutexLocker lock(&d->slotMutex);
      Slot& s = d->mapSlots[slot];
      s.varImage = image;

      /* Always pass the signal through the event queue even if the
       * listener was in the same thread. This way the listener will
       * always call requestImage() from its own thread and we don't
       * need a recursive mutex there. In almost all cases, this
       * function will be called from a different thread, and the call
       * would have to be queued through the main thread's event loop
       * anyway.
       */
      if (s.pListener)
        s.pListener->metaObject()->method(s.iMethodIndex).invoke(s.pListener,
                                                                 Qt::QueuedConnection,
                                                                 Q_ARG(QVariant, slot));
      return true;
    }
  return false;
}

void PiiQmlImageProvider::storeImage(const PiiVariant& image, PiiProbeInput* sender)
{
  storeImage(sender->objectName(), image);
}

PiiQmlImageProvider::ConverterMap* PiiQmlImageProvider::converterMap()
{
  static ConverterMap map;
  return &map;
}

void PiiQmlImageProvider::setConverter(int id, Converter converter)
{
  if (converter)
    converterMap()->insert(id, converter);
  else
    converterMap()->remove(id);
}
