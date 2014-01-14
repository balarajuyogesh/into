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

#include "PiiGenericFunction.h"
#include "PiiMetaTypeUtil.h"
#include "PiiInvalidArgumentException.h"
#include <QCoreApplication>

PiiGenericFunction::Data::Data() :
  iReturnType(QMetaType::Void)
{}

PiiGenericFunction::Data::~Data()
{}

PiiGenericFunction::PiiGenericFunction() :
  d(new Data)
{}

PiiGenericFunction::PiiGenericFunction(Data* data) : d(data)
{}

PiiGenericFunction::~PiiGenericFunction()
{
  delete d;
}

QString PiiGenericFunction::signature(const QString& name) const
{
  if (!d->strSignature.isEmpty())
    return d->strSignature.arg(name);

  if (d->iReturnType != QMetaType::Void)
    {
      d->strSignature += QMetaType::typeName(d->iReturnType);
      d->strSignature += ' ';
    }
  d->strSignature += "%1(";
  for (int i=0; i<d->lstParamTypes.size(); ++i)
    {
      if (i > 0)
        d->strSignature += ',';
      d->strSignature += QMetaType::typeName(d->lstParamTypes[i]);
    }
  d->strSignature += ')';
  return d->strSignature.arg(name);
}

int PiiGenericFunction::scoreOverload(const QVariantList& params)
{
  return Pii::scoreOverload(params, d->lstParamTypes);
}

int PiiGenericFunction::returnType() const { return d->iReturnType; }
QList<int> PiiGenericFunction::paramTypes() const { return d->lstParamTypes; }

QVariant PiiGenericFunction::call(QVariantList& params)
{
  const int iParamCnt = params.size();
  if (iParamCnt != d->lstParamTypes.size())
    PII_THROW(PiiInvalidArgumentException,
              QCoreApplication::translate("PiiGenericFunction", "Wrong number of arguments."));
  QVector<void*> vecParams;
  vecParams << 0; // Return value
  
  // Convert parameter list to void pointers.
  for (int i=0; i<iParamCnt; ++i)
    {
      if (int(params[i].type()) != d->lstParamTypes[i] &&
          (params[i].type() != QVariant::UserType || params[i].userType() != d->lstParamTypes[i]))
        {
          if (d->lstParamTypes[i] < int(QVariant::UserType))
            params[i].convert(QVariant::Type(d->lstParamTypes[i]));
          else
            PII_THROW(PiiInvalidArgumentException,
                      QCoreApplication::translate("PiiGenericFunction", "Cannot convert %1 to %2.")
                      .arg(QMetaType::typeName(params[i].type()))
                      .arg(QMetaType::typeName(d->lstParamTypes[i])));
        }
      vecParams << const_cast<void*>(params[i].constData());
    }

  const int iReturnType = d->iReturnType;
  if (iReturnType == QMetaType::Void)
    {
      call(vecParams.data());
      return QVariant();
    }
  else
    {
      QVariant varReturn(d->iReturnType, (void*)0);
      vecParams[0] = varReturn.data();
      call(vecParams.data());
      return varReturn;
    }
}
