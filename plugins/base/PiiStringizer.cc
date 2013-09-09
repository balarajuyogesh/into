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

#include "PiiStringizer.h"

#include <PiiYdinTypes.h>
#include <PiiMatrixUtil.h>
#include <QTextStream>

PiiStringizer::Data::Data() :
  strColumnSeparator(" "),
  strRowSeparator("\n"),
  strStartDelimiter(""),
  strEndDelimiter(""),
  iPrecision(2)
{
}

PiiStringizer::PiiStringizer() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiStringizer::process()
{
  PII_D;
  PiiVariant obj = readInput();

  QString str;
  switch (obj.type())
    {
    case PiiYdin::QStringType:
      str = obj.valueAs<QString>();
      break;
      PII_INTEGER_CASES(str = intToString, obj);
      PII_FLOAT_CASES(str = floatToString, obj);
      PII_PRIMITIVE_MATRIX_CASES(str = matrixToString, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
  emitObject(QString("%1%2%3").arg(d->strStartDelimiter).arg(str).arg(d->strEndDelimiter));
}

template <class T> QString PiiStringizer::floatToString(const PiiVariant& obj)
{
  PII_D;
  return QString("%1").arg(obj.valueAs<T>(), 0, 'f', d->iPrecision);
}

template <class T> QString PiiStringizer::intToString(const PiiVariant& obj)
{
  return QString::number(obj.valueAs<T>());
}

template <class T> QString PiiStringizer::matrixToString(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> mat = obj.valueAs<PiiMatrix<T> >();
  QString strResult;
  QTextStream stream(&strResult, QIODevice::WriteOnly);
  Pii::printMatrix(stream, mat, d->strColumnSeparator, d->strRowSeparator);
  return strResult;
}

void PiiStringizer::setColumnSeparator(const QString& columnSeparator) { _d()->strColumnSeparator = columnSeparator; }
QString PiiStringizer::columnSeparator() const { return _d()->strColumnSeparator; }
void PiiStringizer::setRowSeparator(const QString& rowSeparator) { _d()->strRowSeparator = rowSeparator; }
QString PiiStringizer::rowSeparator() const { return _d()->strRowSeparator; }
void PiiStringizer::setPrecision(int precision) { _d()->iPrecision = precision; }
int PiiStringizer::precision() const { return _d()->iPrecision; }
void PiiStringizer::setStartDelimiter(const QString& startDelimiter) { _d()->strStartDelimiter = startDelimiter; }
QString PiiStringizer::startDelimiter() const { return _d()->strStartDelimiter; }
void PiiStringizer::setEndDelimiter(const QString& endDelimiter) { _d()->strEndDelimiter = endDelimiter; }
QString PiiStringizer::endDelimiter() const { return _d()->strEndDelimiter; }
