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

#include "PiiMetaTypeUtil.h"

#define PII_COPY_METATYPE_CASE(NAME, TYPE) \
  case QMetaType::NAME: *reinterpret_cast<TYPE*>(target) = *reinterpret_cast<const TYPE*>(source); break;

#define PII_COPY_METATYPE_CASE_N(N, PARAMS) PII_COPY_METATYPE_CASE PARAMS

#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QRegExp>
#include <QBitArray>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QLine>
#include <QLineF>
#include <QPoint>
#include <QPointF>
#include <QEasingCurve>
#include <QUrl>

namespace Pii
{
  void copyMetaType(int type, const void* source, void* target)
  {
    switch (type)
      {
        PII_FOR_N(PII_COPY_METATYPE_CASE_N, 37,
                  ((Long,long),
                   (Int,int),
                   (Short,short),
                   (Char,char),
                   (ULong,ulong),
                   (UInt,uint),
                   (LongLong,qlonglong),
                   (ULongLong,qulonglong),
                   (UShort,ushort),
                   (UChar,uchar),
                   (Bool,bool),
                   (Float,float),
                   (Double,double),
                   (QChar,QChar),
                   (QVariantMap,QVariantMap),
                   (QVariantHash,QVariantHash),
                   (QVariantList,QVariantList),
                   (QVariant,QVariant),
                   (QByteArray,QByteArray),
                   (QString,QString),
                   (QStringList,QStringList),
                   (QBitArray,QBitArray),
                   (QDate,QDate),
                   (QTime,QTime),
                   (QDateTime,QDateTime),
                   (QUrl,QUrl),
                   (QLocale,QLocale),
                   (QRect,QRect),
                   (QRectF,QRectF),
                   (QSize,QSize),
                   (QSizeF,QSizeF),
                   (QLine,QLine),
                   (QLineF,QLineF),
                   (QPoint,QPoint),
                   (QPointF,QPointF),
                   (QRegExp,QRegExp),
                   (QEasingCurve,QEasingCurve)));
      default:
        break;
      }
  }

  bool copyMetaType(const QVariant& source, int expectedType, void** args)
  {
    // HACK See QMetaProperty::read()/write() for details
    if (int(source.type()) == expectedType)
      {
        copyMetaType(expectedType, source.constData(), args[0]);
        return true;
      }
    else if (source.type() == QVariant::UserType &&
       source.userType() == expectedType)
      {
        *reinterpret_cast<QVariant*>(args[1]) = source;
        *reinterpret_cast<int*>(args[2]) = 1;
        return true;
      }
    else if (expectedType < int(QVariant::UserType) && source.canConvert(QVariant::Type(expectedType)))
      {
        QVariant varConverted(source);
        varConverted.convert(QVariant::Type(expectedType));
        copyMetaType(expectedType, varConverted.constData(), args[0]);
        return true;
      }
    return false;
  }

  QVariant argsToVariant(void** args, int type)
  {
    if (args[1] != 0)
      return *reinterpret_cast<QVariant*>(args[1]);
    return QVariant(type, args[0]);
  }
  
  int scoreOverload(const QVariantList& params, const QList<int>& types)
  {
    // Overload resolution 1: parameter count must match
    if (params.size() != types.size())
      return -1;
    
    // Overload resolution 2: count matches on parameter types
    int iMatchCount = 0;
    for (int j=0; j<params.size(); ++j)
      {
        if (int(params[j].type()) == types[j] ||
            (params[j].type() == QVariant::UserType && params[j].userType() == types[j]))
          ++iMatchCount;
        else if (types[j] >= int(QVariant::UserType) ||
                 !params[j].canConvert(QVariant::Type(types[j])))
          return -1;
      }
    return iMatchCount;
  }

  QVariantList argsToList(const QList<int>& types, void** args)
  {
    QVariantList lstResult;
    for (int i=0; i<types.size(); ++i)
      lstResult << QVariant(types[i], args[i]);
    return lstResult;
  }
}
