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

#ifndef _PIISERIALIZATIONUTIL_H
# error "Never use <PiiSerializationUtil-templates.h> directly; include <PiiSerializationUtil.h> instead."
#endif

#include "PiiSerializationException.h"
#include "PiiSmartPtr.h"
#include "PiiQVariantWrapper.h"
#include "PiiNameValuePair.h"
#include "PiiBinaryObject.h"
#include "PiiBaseClass.h"
#include "PiiSerializationFactory.h"
#include "PiiConfigurable.h"
#include <QBuffer>

namespace PiiSerialization
{
  template <class Archive, class T> inline void save(Archive& archive, const std::complex<T>& value, const unsigned int /*version*/)
  {
    T real = value.real(), imag = value.imag();
    PII_SERIALIZE(archive, real);
    PII_SERIALIZE(archive, imag);
  }

  template <class Archive, class T> inline void load(Archive& archive, std::complex<T>& value, const unsigned int /*version*/)
  {
    T real, imag;
    PII_SERIALIZE(archive, real);
    PII_SERIALIZE(archive, imag);
    value = std::complex<T>(real, imag);
  }

  template <class Archive, class T> inline void serialize(Archive& archive, std::complex<T>& value, const unsigned int version)
  {
    separateFunctions(archive, value, version);
  }

  // ********** Any random-access collection ***********

  template <class T, class Archive, class Collection> void save(Archive& archive, const Collection& lst, const unsigned int /*version*/)
  {
    int size = lst.size();
    archive << PII_NVP("size", size);
    for (int i=0; i<size; i++)
      archive << PII_NVP("li", lst[i]);
  }

  template <class T, class Archive, class Collection> void load(Archive& archive, Collection& lst, const unsigned int /*version*/)
  {
    int size;
    archive >> PII_NVP("size", size);
    lst.clear();
    for (int i=0; i<size; i++)
      {
        T element;
        archive >> PII_NVP("li", element);
        lst.append(element);
        // This is needed because elements inserted to the list
        // always have the same memory address (&element). We need
        // to tell the archive the final location of the object.
        archive.objectMoved(element, lst[i]);
      }
  }

  template <class T, class Archive, class Size> inline void saveSize(Archive& archive, const Size& size)
  {
    T width = size.width(), height = size.height();
    archive & PII_NVP("width", width);
    archive & PII_NVP("height", height);
  }

  template <class T, class Archive, class Size> inline void loadSize(Archive& archive, Size& size)
  {
    T width, height;
    archive & PII_NVP("width", width);
    archive & PII_NVP("height", height);
    size.setWidth(width);
    size.setHeight(height);
  }

  template <class Archive> inline void save(Archive& archive, const QSize& size, const unsigned int /*version*/)
  {
    saveSize<int>(archive, size);
  }

  template <class Archive> inline void load(Archive& archive, QSize& size, const unsigned int /*version*/)
  {
    loadSize<int>(archive, size);
  }

  template <class Archive> inline void save(Archive& archive, const QSizeF& size, const unsigned int /*version*/)
  {
    saveSize<double>(archive, size);
  }

  template <class Archive> inline void load(Archive& archive, QSizeF& size, const unsigned int /*version*/)
  {
    loadSize<double>(archive, size);
  }

  template <class T, class Archive, class Point> inline void savePoint(Archive& archive, const Point& point)
  {
    T x = point.x(), y = point.y();
    archive & PII_NVP("x", x);
    archive & PII_NVP("y", y);
  }

  template <class T, class Archive, class Point> inline void loadPoint(Archive& archive, Point& point)
  {
    T x, y;
    archive & PII_NVP("x", x);
    archive & PII_NVP("y", y);
    point.setX(x);
    point.setY(y);
  }

  template <class Archive> inline void save(Archive& archive, const QPoint& point, const unsigned int /*version*/)
  {
    savePoint<int>(archive, point);
  }

  template <class Archive> inline void load(Archive& archive, QPoint& point, const unsigned int /*version*/)
  {
    loadPoint<int>(archive, point);
  }

  template <class Archive> inline void save(Archive& archive, const QPointF& point, const unsigned int /*version*/)
  {
    savePoint<double>(archive, point);
  }

  template <class Archive> inline void load(Archive& archive, QPointF& point, const unsigned int /*version*/)
  {
    loadPoint<double>(archive, point);
  }

  template <class T, class Archive, class Rect> inline void saveRect(Archive& archive, const Rect& rect)
  {
    T x = rect.x(), y = rect.y();
    T width = rect.width(), height = rect.height();
    archive & PII_NVP("x", x);
    archive & PII_NVP("y", y);
    archive & PII_NVP("width", width);
    archive & PII_NVP("height", height);
  }

  template <class T, class Archive, class Rect> inline void loadRect(Archive& archive, Rect& rect)
  {
    T x, y, width, height;
    archive & PII_NVP("x", x);
    archive & PII_NVP("y", y);
    archive & PII_NVP("width", width);
    archive & PII_NVP("height", height);
    rect.setRect(x, y, width, height);
  }

  template <class Archive> inline void save(Archive& archive, const QRect& rect, const unsigned int /*version*/)
  {
    saveRect<int>(archive, rect);
  }

  template <class Archive> inline void load(Archive& archive, QRect& rect, const unsigned int /*version*/)
  {
    loadRect<int>(archive, rect);
  }

  template <class Archive> inline void save(Archive& archive, const QRectF& rect, const unsigned int /*version*/)
  {
    saveRect<double>(archive, rect);
  }

  template <class Archive> inline void load(Archive& archive, QRectF& rect, const unsigned int /*version*/)
  {
    loadRect<double>(archive, rect);
  }

  template <class Archive, class Key, class Val> void save(Archive& archive, const QMap<Key,Val>& map, const unsigned int /*version*/)
  {
    int cnt = map.size();
    PII_SERIALIZE(archive, cnt);
    QMapIterator<Key,Val> i(map);
    while (i.hasNext())
      {
        i.next();
        PII_SERIALIZE_NAMED(archive, i.key(), "key");
        PII_SERIALIZE_NAMED(archive, i.value(), "val");
      }
  }

  template <class Archive, class Key, class Val> void load(Archive& archive, QMap<Key,Val>& map, const unsigned int /*version*/)
  {
    int cnt;
    PII_SERIALIZE(archive, cnt);
    for (int i=cnt; i--; )
      {
        Key key;
        Val val;
        PII_SERIALIZE(archive, key);
        PII_SERIALIZE(archive, val);
        map.insert(key, val);
      }
  }

  struct CollectionLoader
  {
    template <class Archive, class Collection, class T> static void serialize(Archive& archive, Collection& lst, const unsigned int version, T*)
    {
      ::PiiSerialization::load<T>(archive, lst, version);
    }
  };

  struct CollectionSaver
  {
    template <class Archive, class Collection, class T> static void serialize(Archive& archive, const Collection& lst, const unsigned int version, T*)
    {
      ::PiiSerialization::save<T>(archive, lst, version);
    }
  };

  template <class Archive, class Collection, class T> inline void serialize(Archive& archive, Collection& lst, const unsigned int version)
  {
    Pii::If<Archive::InputArchive, CollectionLoader, CollectionSaver>::Type::serialize(archive, lst, version, (T*)0);
  }

  template <class Archive, class T, class U> inline void serialize(Archive & ar, QPair<T,U> & pair, const unsigned int /*version*/)
  {
    ar & PII_NVP("_1", pair.first);
    ar & PII_NVP("_2", pair.second);
  }

  template <class Archive> inline void save(Archive& archive, const QColor& color, const unsigned int /*version*/)
  {
    QString clr = color.name();
    PII_SERIALIZE(archive, clr);
  }

  template <class Archive> inline void load(Archive& archive, QColor& color, const unsigned int /*version*/)
  {
    QString clr;
    PII_SERIALIZE(archive, clr);
    color.setNamedColor(clr);
  }

  template <class Archive> inline void save(Archive& archive, const QByteArray& array, const unsigned int /*version*/)
  {
    const char *data = array.data();
    unsigned int size = array.size();
    archive << Pii::binaryObject(data, size);
  }

  template <class Archive> inline void load(Archive& archive, QByteArray& array, const unsigned int /*version*/)
  {
    char* data;
    unsigned int readBytes;
    archive >> Pii::binaryObject(data, readBytes);
    array = QByteArray(data, readBytes);
    delete[] data;
  }

  template <class Archive> inline void save(Archive& archive, const QPixmap& pixmap, const unsigned int /*version*/)
  {
    if (pixmap.isNull())
      archive << QByteArray();
    else
      {
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "XPM");
        archive << bytes;
      }
  }

  template <class Archive> inline void load(Archive& archive, QPixmap& pixmap, const unsigned int /*version*/)
  {
    QByteArray array;
    archive >> array;
    if (array.isEmpty())
      pixmap = QPixmap();
    else
      pixmap.loadFromData(array, "XPM");
  }

  template <class Archive> inline void save(Archive& /*archive*/, const QGradient& /*gradient*/, const unsigned int /*version*/)
  {
    //PENDING
  }

  template <class Archive> inline void load(Archive& /*archive*/, QGradient& /*gradient*/, const unsigned int /*version*/)
  {
    //PENDING
  }

  template <class Archive> inline void save(Archive& archive, const QBrush& brush, const unsigned int /*version*/)
  {
    archive << PII_NVP("gradient", brush.gradient());
    if (!brush.gradient())
      {
        archive << PII_NVP("color", brush.color());
        archive << PII_NVP("style", (int)brush.style());
        archive << PII_NVP("texture", brush.texture());
      }
  }

  template <class Archive> inline void load(Archive& archive, QBrush& brush, const unsigned int /*version*/)
  {
    QGradient* pGradient;
    archive >> PII_NVP("gradient", pGradient);
    if (pGradient)
      {
        brush = QBrush(*pGradient);
        delete pGradient;
      }
    else
      {
        QColor clr;
        int style;
        QPixmap pixmap;
        archive >> PII_NVP("color", clr);
        archive >> PII_NVP("style", style);
        archive >> PII_NVP("texture", pixmap);
        brush.setColor(clr);
        brush.setStyle((Qt::BrushStyle)style);
        if (!pixmap.isNull())
          brush.setTexture(pixmap);
      }
  }

  template <class Archive> inline void save(Archive& archive, const QFont& font, const unsigned int /*version*/)
  {
    archive << font.toString();
  }

  template <class Archive> inline void load(Archive& archive, QFont& font, const unsigned int /*version*/)
  {
    QString desc;
    archive >> desc;
    font.fromString(desc);
  }

  template <class Archive> inline void save(Archive& archive, const QTime& time, const unsigned int /*version*/)
  {
    // 10 bits (1024 values) for millisecond, 6 bits (64 values) for
    // second and minute, the rest (10 bits) for hour.
    archive << ((time.hour() << 22) | (time.minute() << 16) | (time.second() << 10) | time.msec());
  }

  template <class Archive> inline void load(Archive& archive, QTime& time, const unsigned int /*version*/)
  {
    int i;
    archive >> i;
    time.setHMS(i >> 22, (i >> 16) & 0x3f, (i >> 10) & 0x3f, i & 0x3ff);
  }

  template <class Archive> inline void save(Archive& archive, const QDate& date, const unsigned int /*version*/)
  {
    // 5 bits (32 values) for day, 4 bits (16 values) for month, and
    // the rest (23 bits) for year.
    archive << ((date.year() << 11) | (date.month() << 5) | date.day());
  }

  template <class Archive> inline void load(Archive& archive, QDate& date, const unsigned int /*version*/)
  {
    int i;
    archive >> i;
    date.setDate(i >> 11, (i >> 5) & 0xf, i & 0x1f);
  }

  template <class Archive> inline void save(Archive& archive, const QDateTime& dateTime, const unsigned int /*version*/)
  {
    archive << PII_NVP("date", dateTime.date())
            << PII_NVP("time", dateTime.time())
            << PII_NVP("spec", int(dateTime.timeSpec()));
  }

  template <class Archive> inline void load(Archive& archive, QDateTime& dateTime, const unsigned int /*version*/)
  {
    QDate date;
    QTime time;
    int spec;
    archive >> PII_NVP("date", date)
            >> PII_NVP("time", time)
            >> PII_NVP("spec", spec);
    dateTime.setDate(date);
    dateTime.setTime(time);
    dateTime.setTimeSpec(Qt::TimeSpec(qBound(0,spec,2)));
  }

  struct PropertyLoader
  {
    template <class Archive> static void serialize(Archive& archive, QObject& obj, int /*propertyOffset*/)
    {
      loadProperties(archive, obj);
    }
  };

  struct PropertySaver
  {
    template <class Archive> static void serialize(Archive& archive, const QObject& obj, int propertyOffset)
    {
      saveProperties(archive, obj, propertyOffset);
    }
  };

  template <class Archive> inline void serializeProperties(Archive& archive, QObject& obj, int propertyOffset)
  {
    Pii::If<Archive::InputArchive, PropertyLoader, PropertySaver>::Type::serialize(archive, obj, propertyOffset);
  }

  template <class Archive> void saveProperties(Archive& archive, const QObject& obj, int propertyOffset)
  {
    // Retrieve object meta information
    const QMetaObject* metaObj = obj.metaObject();
    // Store the total number of properties
    int iTotalCnt = metaObj->propertyCount();
    int iValidCnt = 0;
    // Count stored read/write properties
    for (int i=propertyOffset; i<iTotalCnt; ++i)
      {
        QMetaProperty prop = metaObj->property(i);
        if (prop.isReadable() && prop.isWritable() && prop.isStored())
          ++iValidCnt;
      }
    QList<QByteArray> lstDynamicPropertyNames = obj.dynamicPropertyNames();
    iValidCnt += lstDynamicPropertyNames.size();
    archive << PII_NVP("propCnt", iValidCnt);

    PiiConfigurable* pConfigurable = qobject_cast<PiiConfigurable*>(&obj);
    // Loop through the properties
    for (int i=propertyOffset; i<iTotalCnt; ++i)
      {
        // Store only read/write properties flagged as "stored"
        QMetaProperty prop = metaObj->property(i);
        if (prop.isReadable() && prop.isWritable() && prop.isStored())
          {
            // Read the current value of the property
            const char* pName = prop.name();
            QVariant variant(pConfigurable ?
                             pConfigurable->property(pName) :
                             obj.property(pName));
            // Save property name and value
            //qDebug("Serializing %s.%s", obj.metaObject()->className(), pName);
            archive << PII_NVP("name", pName);
            archive << PII_NVP("value", variant);
          }
      }
    for (int i=0; i<lstDynamicPropertyNames.size(); ++i)
      {
        const char* pName = lstDynamicPropertyNames[i];
        QVariant variant(obj.property(pName));
        archive << PII_NVP("name", pName);
        archive << PII_NVP("value", variant);
      }
  }

  template <class Archive> void loadProperties(Archive& archive, QObject& obj)
  {
    // Read the number of stored properties
    int cnt;
    archive >> PII_NVP("propCnt", cnt);

    PiiConfigurable* pConfigurable = qobject_cast<PiiConfigurable*>(&obj);

    // Loop through all saved properties
    for (int i=0; i<cnt; ++i)
      {
        QString name;
        QVariant variant;

        // Load property name and value
        archive >> PII_NVP("name", name);
        archive >> PII_NVP("value", variant);

        if (pConfigurable)
          pConfigurable->setProperty(qPrintable(name), variant);
        else
          obj.setProperty(qPrintable(name), variant);
      }
  }



  // ********** QVariant ***********

  template <class Archive, class T> inline void storeQVariantValue(Archive& archive, const T& value)
  {
    archive << PII_NVP("val", value);
  }

  /*
   * A function template that serializes a QVariant. QVariants store
   * their type ID and the actual data element, in this order.
   */
  template <class Archive> void save(Archive& archive, const QVariant& variant, const unsigned int /*version*/)
  {
    int typeId = (int)variant.type();
    archive << PII_NVP("id", typeId);

    switch (typeId)
      {
      case QVariant::Invalid:
        break;
#define PII_STORE_QVARIANT_VALUE(N, TYPE)                  \
        case QVariant::TYPE:                               \
          storeQVariantValue(archive, variant.to##TYPE()); \
          break;
        PII_FOR_N(PII_STORE_QVARIANT_VALUE, 20,
                  (Int,
                   UInt,
                   LongLong,
                   ULongLong,
                   Double,
                   Bool,
                   String,
                   List,
                   StringList,
                   Size,
                   SizeF,
                   Point,
                   PointF,
                   Rect,
                   RectF,
                   Map,
                   Date,
                   Time,
                   DateTime,
                   ByteArray))

#undef PII_STORE_QVARIANT_VALUE
#define PII_STORE_QVARIANT_VALUE(N, TYPE)                     \
        case QVariant::TYPE:                                  \
          storeQVariantValue(archive, variant.value<Q##TYPE>()); \
          break;
          PII_FOR_N(PII_STORE_QVARIANT_VALUE, 4,
                    (Color,
                     Brush,
                     Font,
                     Pixmap))
      // Special case: QVariant stored as a QVariant
      case QMetaType::QVariant:
        storeQVariantValue(archive, variant.value<QVariant>());
        break;
      case QVariant::UserType:
        {
          /* Serializing user types is a bit tricky. We don't
             serialize the type directly, but create a wrapper instead
             and serialize it. The wrapper must be registered as a
             serializer with a name "PiiQVariantWrapper<MyType>".
          */
          QString typeName = QString("PiiQVariantWrapper<%1>").arg(variant.typeName());
          // Exception-safe pointer
          PiiSmartPtr<PiiQVariantWrapper> pWrapper(PiiSerializationFactory::create<PiiQVariantWrapper>(qPrintable(typeName)));
          // Wrapper is stored even if it cannot be created.
          if (pWrapper == 0)
            piiWarning("Unsupported QVariant user type in serialization: %s (ID %d)", variant.typeName(), variant.userType());
          else
            pWrapper->setVariant(variant);
          archive << PII_NVP("val", (PiiQVariantWrapper*)pWrapper);
        }
        break;
      default:
        piiWarning("Unsupported QVariant type in serialization: %s (ID %d)", variant.typeName(), typeId);
      }
  }

  template <class T, class Archive> inline void setQVariantValue(Archive& archive, QVariant& variant)
  {
    T value;
    archive >> PII_NVP("val", value);
    variant.setValue(value);
  }

  /*
   * A function that deserializes a QVariant. The deserializer first
   * reads a type ID and determines the type of the variant
   * according to it.
   */
  template <class Archive> void load(Archive& archive, QVariant& variant, const unsigned int version)
  {
    Q_UNUSED(version);

    int typeId;
    archive >> PII_NVP("id", typeId);

    switch (typeId)
      {
      case QVariant::Invalid:
        break;
#define PII_SET_QVARIANT_VALUE_(ENUM, TYPE)          \
        case QVariant::ENUM:                         \
          setQVariantValue<TYPE >(archive, variant); \
          break;
#define PII_SET_QVARIANT_VALUE(N, PARAM) PII_SET_QVARIANT_VALUE_ PARAM

        PII_FOR_N(PII_SET_QVARIANT_VALUE, 24,
                  ((Int, int),
                   (UInt, unsigned int),
                   (LongLong, long long),
                   (ULongLong, unsigned long long),
                   (Double, double),
                   (Bool, bool),
                   (String, QString),
                   (List, QVariantList),
                   (StringList, QStringList),
                   (Size, QSize),
                   (SizeF, QSizeF),
                   (Point, QPoint),
                   (PointF, QPointF),
                   (Rect, QRect),
                   (RectF, QRectF),
                   (Map, QVariantMap),
                   (Date, QDate),
                   (Time, QTime),
                   (DateTime, QDateTime),
                   (ByteArray, QByteArray),
                   (Color, QColor),
                   (Brush, QBrush),
                   (Font, QFont),
                   (Pixmap, QPixmap)))
      case QMetaType::QVariant:
        {
          QVariant varTmp;
          archive >> PII_NVP("val", varTmp);
          variant = QVariant(QMetaType::QVariant, &varTmp);
          break;
        }
      case QVariant::UserType:
        {
          /* User types are wrapped into a PiiQVariantWrapper. When we
           * read the wrapper, it deserializes the user type and
           * stores it as a QVariant.
           */
          PiiQVariantWrapper* pWrapper = 0;
          archive >> PII_NVP("val", pWrapper);
          if (pWrapper == 0)
            piiWarning("Unsupported QVariant user type in deserialization.");
          else
            pWrapper->moveTo(variant);
          // NOTE: If pWrapper cannot be deserialized, the
          // serialization library ensures no stale instances will be
          // left lying around. We don't need a smart pointer here.
          delete pWrapper;
        }
        break;
      default:
        piiWarning("Unsupported QVariant type ID in deserialization: %d", typeId);
        //PII_SERIALIZATION_ERROR(InvalidDataFormat);
      }
  }

  template <class Archive, class T> QByteArray toByteArray(const T& object)
  {
    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    Archive archive(&buffer);
    archive << object;
    return array;
  }

  template <class Archive, class T> void fromByteArray(const QByteArray& array, T& object)
  {
    QBuffer buffer(const_cast<QByteArray*>(&array));
    buffer.open(QIODevice::ReadOnly);
    Archive archive(&buffer);
    archive >> object;
  }
} //namespace PiiSerialization
