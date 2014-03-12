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

#include "PiiHistogramEqualizer.h"
#include <PiiYdinTypes.h>
#include "PiiHistogram.h"

PiiHistogramEqualizer::Data::Data() :
  iLevels(256)
{
}

PiiHistogramEqualizer::PiiHistogramEqualizer() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("image"));
}

void PiiHistogramEqualizer::setLevels(int levels)
{
  if (levels >= 0 && levels < 65536)
    _d()->iLevels = levels;
}

void PiiHistogramEqualizer::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_INT_GRAY_IMAGE_CASES(equalize, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiHistogramEqualizer::equalize(const PiiVariant& obj)
{
  const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
  emitObject(PiiImage::equalize(img, (unsigned)_d()->iLevels));
}

int PiiHistogramEqualizer::levels() const
{
  return _d()->iLevels;
}
