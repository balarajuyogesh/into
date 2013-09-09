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

#include <PiiYdinTypes.h>
#include "PiiCornerDetector.h"
#include "PiiImage.h"

PiiCornerDetector::Data::Data() :
  dThreshold(25)
{
}

PiiCornerDetector::PiiCornerDetector() : PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("corners"));
}

void PiiCornerDetector::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(detectCorners, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiCornerDetector::detectCorners(const PiiVariant& obj)
{
  emitObject(PiiImage::detectFastCorners(obj.valueAs<PiiMatrix<T> >(),
                                         T(_d()->dThreshold)));
}

void PiiCornerDetector::setThreshold(double threshold) { _d()->dThreshold = threshold; }
double PiiCornerDetector::threshold() const { return _d()->dThreshold; }
