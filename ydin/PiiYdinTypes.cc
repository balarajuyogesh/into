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

#include "PiiYdinTypes.h"
#include <PiiMatrixSerialization.h>
#include <PiiSerializableExport.h>
#include <PiiSerializationTraits.h>
#include <PiiSerialization.h>
#include <PiiColor.h>
#include <complex>
#include <QDate>
#include <QTime>
#include <QStringList>

#include <PiiGenericInputArchive.h>
#include <PiiGenericOutputArchive.h>

#define PII_REGISTER_QVW(TYPE) \
  PII_DEFINE_EXPORTED_CLASS_TEMPLATE(class, TYPE); \
  PII_INSTANTIATE_SERIALIZER(PiiGenericInputArchive, TYPE); \
  PII_INSTANTIATE_SERIALIZER(PiiGenericOutputArchive, TYPE); \
  PII_INSTANTIATE_FACTORY(TYPE)

#define PII_REGISTER_VARIANT_BOTH(TYPE) \
  PII_REGISTER_VARIANT_TYPE(TYPE); \
  PII_REGISTER_QVW(PiiQVariantWrapper::Template<TYPE >); \
  static int PII_JOIN(_qVariantId, __LINE__) = qRegisterMetaType<TYPE >()

// matrices
PII_REGISTER_VARIANT_BOTH(PiiMatrix<char>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<short>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<int>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<qint64>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<uchar>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<ushort>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<uint>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<quint64>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<float>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<double>);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<bool>);

// color images
PII_REGISTER_VARIANT_BOTH(PiiMatrix<PiiColor<uchar> >);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<PiiColor4<uchar> >);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<PiiColor<ushort> >);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<PiiColor<float> >);

// complex matrices
PII_REGISTER_VARIANT_BOTH(PiiMatrix<std::complex<int> >);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<std::complex<float> >);
PII_REGISTER_VARIANT_BOTH(PiiMatrix<std::complex<double> >);

// colors
PII_REGISTER_VARIANT_BOTH(PiiColor<uchar>);
PII_REGISTER_VARIANT_BOTH(PiiColor4<uchar>);
PII_REGISTER_VARIANT_BOTH(PiiColor<ushort>);
PII_REGISTER_VARIANT_BOTH(PiiColor<float>);

// complex numbers
PII_REGISTER_VARIANT_BOTH(std::complex<int>);
PII_REGISTER_VARIANT_BOTH(std::complex<float>);
PII_REGISTER_VARIANT_BOTH(std::complex<double>);

// Qt classes
PII_REGISTER_VARIANT_TYPE(QString);
PII_REGISTER_VARIANT_TYPE(QStringList);
PII_REGISTER_VARIANT_TYPE(QDate);
PII_REGISTER_VARIANT_TYPE(QTime);
PII_REGISTER_VARIANT_TYPE(QDateTime);
PII_REGISTER_VARIANT_TYPE(QImage);

PII_REGISTER_VARIANT_TYPE(PiiSocketState);

PII_MAP_VARIANT_ID_TO_TYPE(PiiYdin::ReconfigurationTagType, QString);


namespace PiiYdin
{
  QString convertToQString(const PiiVariant& variant)
  {
    QString strValue;
    switch (variant.type())
      {
      case QStringType:
        return variant.valueAs<QString>();
        PII_PRIMITIVE_CASES(return numberToQString, variant);
      }
    return QString();
  }

  QString convertToQString(PiiInputSocket* input)
  {
    QString strValue(convertToQString(input->firstObject()));
    if (strValue.isNull())
      PII_THROW_UNKNOWN_TYPE(input);
    return strValue;
  }
}
