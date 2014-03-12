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

#include "PiiStringFormatter.h"
#include <PiiYdinTypes.h>


PiiStringFormatter::PiiStringFormatter() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input0"));
  addSocket(new PiiOutputSocket("output"));
  setFormat("%0");
}

void PiiStringFormatter::process()
{
  QString strResult = _d()->strFormat;
  for (int i=0; i<inputCount(); ++i)
    strResult = strResult.arg(PiiYdin::convertToQString(inputAt(i)));
  emitObject(strResult);
}

void PiiStringFormatter::setFormat(const QString& format)
{
  _d()->strFormat = format;
  QRegExp reArgument("%[0-9]+");
  QList<int> lstArguments;
  int index = 0;
  // Find distinct argument numbers
  while ((index = reArgument.indexIn(format, index)) != -1)
    {
      int iArgument = reArgument.cap(0).mid(1).toInt();
      if (!lstArguments.contains(iArgument))
        lstArguments << iArgument;
      index += reArgument.matchedLength();
    }
  setNumberedInputs(qMax(lstArguments.size(), 1));
}

QString PiiStringFormatter::format() const
{
  return _d()->strFormat;
}
