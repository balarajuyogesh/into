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

#include "PiiVariant.h"

#ifndef PII_NO_QT
#  include <PiiSerializableExport.h>
#  include <PiiSerializationTraits.h>
#  include <PiiSerialization.h>
#  include <PiiQVariantWrapper.h>
#endif

PII_REGISTER_VARIANT_TYPE(char);
PII_REGISTER_VARIANT_TYPE(bool);
PII_REGISTER_VARIANT_TYPE(short);
PII_REGISTER_VARIANT_TYPE(int);
PII_REGISTER_VARIANT_TYPE(qint64);
//PII_REGISTER_VARIANT_TYPE(long long);
PII_REGISTER_VARIANT_TYPE(float);
PII_REGISTER_VARIANT_TYPE(double);
//PII_REGISTER_VARIANT_TYPE(long double);
PII_REGISTER_VARIANT_TYPE(unsigned char);
PII_REGISTER_VARIANT_TYPE(unsigned short);
PII_REGISTER_VARIANT_TYPE(unsigned int);
PII_REGISTER_VARIANT_TYPE(quint64);
//PII_REGISTER_VARIANT_TYPE(unsigned long long);

#ifndef PII_NO_QT
// Register PiiVariant as a Qt metatype.
int iPiiVariantTypeId = qRegisterMetaType<PiiVariant>("PiiVariant");
int iPiiVariantListTypeId = qRegisterMetaType<PiiVariantList>("PiiVariantList");

PII_SERIALIZABLE_EXPORT(PiiVariant);
PII_SERIALIZABLE_EXPORT(PiiVariantList);
PII_SERIALIZABLE_EXPORT(PiiQVariantWrapper::Template<PiiVariant>);
PII_SERIALIZABLE_EXPORT(PiiQVariantWrapper::Template<PiiVariantList>);

// Save a few bytes by not repeating the string.
const char* PiiVariant::pValueStr = "value";
#endif

// Initializes default conversion functions
PiiVariant::ConvertInit PiiVariant::_convertInit;


QHash<unsigned int, PiiVariant::VTable*>* PiiVariant::hashVTables()
{
  static QHash<unsigned int, VTable*> hash;
  return &hash;
}

PiiVariant::PiiVariant() :
  _pVTable(0), _uiType(InvalidType)
{
}

PiiVariant::PiiVariant(const PiiVariant& other) :
  _pVTable(other._pVTable), _uiType(other._uiType)
{
  if (other._pVTable != 0)
    other._pVTable->constructCopy(*this, other);
  else
    _value = other._value;
}

PiiVariant& PiiVariant::operator= (const PiiVariant& other)
{
  if (&other != this)
    {
      if (_uiType == other._uiType)
        {
          if (_pVTable == 0)
            _value = other._value;
          else
            _pVTable->copy(*this, other);
        }
      else
        {
          if (_pVTable != 0)
            _pVTable->destruct(*this);
          if (other._pVTable == 0)
            _value = other._value;
          else
            other._pVTable->constructCopy(*this, other);

          _uiType = other._uiType;
          _pVTable = other._pVTable;
        }
    }
  return *this;
}

PiiVariant::~PiiVariant()
{
  if (_pVTable != 0)
    _pVTable->destruct(*this);
}

PiiVariant::VTable* PiiVariant::vTableByType(unsigned int type)
{
  return hashVTables()->value(type);
}

PiiVariant::ConverterMap* PiiVariant::converterMap()
{
  static ConverterMap converterMap;
  return &converterMap;
}

bool PiiVariant::canConvert(uint fromType, uint toType)
{
  return fromType == toType || converter(fromType, toType) != 0;
}

bool PiiVariant::canConvert(uint toType) const
{
  return canConvert(_uiType, toType);
}

PiiVariant::ConverterFunction PiiVariant::converter(uint fromType, uint toType)
{
  ConverterMap* pMap = converterMap();
  quint64 uiKey = toKey(fromType, toType);
  ConverterMap::iterator it = pMap->find(uiKey);
  if (it != pMap->end())
    return PII_ITERATOR_VALUE(it);
  return 0;
}

void PiiVariant::setConverter(uint fromType, uint toType, ConverterFunction function)
{
  quint64 uiKey = toKey(fromType, toType);
  if (function)
    converterMap()->insert(uiKey, function);
  else
    converterMap()->remove(uiKey);
}

#define CONVERT_DEFAULT(FROM_TYPE, TO_TYPE) \
  setConverter(Pii::typeId<FROM_TYPE>(),    \
               Pii::typeId<TO_TYPE>(),      \
               &PiiVariant::defaultConverter<FROM_TYPE, TO_TYPE>);

#define ADD_CONVERTERS(FROM_TYPE, N, TO_TYPES) \
  PII_FOR_N_ARG(CONVERT_DEFAULT, FROM_TYPE PII_NULL, N, TO_TYPES)

PiiVariant::ConvertInit::ConvertInit()
{
  ADD_CONVERTERS(char, 8, (short, int, qint64, uchar, uint, ushort, quint64, bool));
  ADD_CONVERTERS(short, 10, (char, int, qint64, uchar, ushort, uint, quint64, float, double, bool));
  ADD_CONVERTERS(int, 10, (char, short, qint64, uchar, ushort, uint, quint64, float, double, bool));
  ADD_CONVERTERS(qint64, 10, (char, short, int, uchar, ushort, uint, quint64, float, double, bool));
  ADD_CONVERTERS(uchar, 8, (char, short, int, qint64, uint, ushort, quint64, bool));
  ADD_CONVERTERS(ushort, 10, (char, short, int, qint64, uchar, uint, quint64, float, double, bool));
  ADD_CONVERTERS(uint, 10, (char, short, int, qint64, uchar, ushort, quint64, float, double, bool));
  ADD_CONVERTERS(quint64, 10, (char, short, int, qint64, uchar, ushort, uint, float, double, bool));
  ADD_CONVERTERS(float, 8, (short, int, qint64, ushort, uint, quint64, double, bool));
  ADD_CONVERTERS(double, 8, (short, int, qint64, ushort, uint, quint64, float, bool));
  ADD_CONVERTERS(bool, 10, (char, short, int, qint64, uchar, ushort, uint, quint64, float, double));
}
