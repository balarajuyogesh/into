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

#include "PiiRegExpMatcher.h"

#include <PiiYdinTypes.h>

PiiRegExpMatcher::PiiRegExpMatcher() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output0"));
}

void PiiRegExpMatcher::setPattern(const QString& pattern)
{
  PII_D;
  d->re.setPattern(pattern);
  // One output for each subexpression plus one for the whole match.
#if QT_VERSION >= 0x050000
  setNumberedOutputs(d->re.captureCount()+1);
#else
  setNumberedOutputs(d->re.numCaptures()+1);
#endif
}

void PiiRegExpMatcher::process()
{
  PII_D;
  QString strInput = PiiYdin::convertToQString(inputAt(0));
  if (d->re.indexIn(strInput) > -1)
    {
      for (int i=outputCount(); i--; )
        emitObject(d->re.cap(i), i);
    }
  else
    {
      for (int i=outputCount(); i--; )
        emitObject(QString(""), i);
    }
}

QString PiiRegExpMatcher::pattern() const { return _d()->re.pattern(); }
