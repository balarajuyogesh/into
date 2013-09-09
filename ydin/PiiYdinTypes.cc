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

// matrices
PII_REGISTER_VARIANT_TYPE(PiiMatrix<char>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<short>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<int>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<qint64>);
//PII_REGISTER_VARIANT_TYPE(PiiMatrix<long long>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<unsigned char>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<unsigned short>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<unsigned int>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<quint64>);
//PII_REGISTER_VARIANT_TYPE(PiiMatrix<unsigned long long>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<float>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<double>);
//PII_REGISTER_VARIANT_TYPE(PiiMatrix<long double>);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<bool>);

// color images
PII_REGISTER_VARIANT_TYPE(PiiMatrix<PiiColor<unsigned char> >);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<PiiColor4<unsigned char> >);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<PiiColor<unsigned short> >);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<PiiColor<float> >);

// complex matrices
PII_REGISTER_VARIANT_TYPE(PiiMatrix<std::complex<int> >);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<std::complex<float> >);
PII_REGISTER_VARIANT_TYPE(PiiMatrix<std::complex<double> >);
//PII_REGISTER_VARIANT_TYPE(PiiMatrix<std::complex<long double> >);

// colors
PII_REGISTER_VARIANT_TYPE(PiiColor<unsigned char>);
PII_REGISTER_VARIANT_TYPE(PiiColor4<unsigned char>);
PII_REGISTER_VARIANT_TYPE(PiiColor<unsigned short>);
PII_REGISTER_VARIANT_TYPE(PiiColor<float>);

// complex numbers
PII_REGISTER_VARIANT_TYPE(std::complex<int>);
PII_REGISTER_VARIANT_TYPE(std::complex<float>);
PII_REGISTER_VARIANT_TYPE(std::complex<double>);
//PII_REGISTER_VARIANT_TYPE(std::complex<long double>);

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
