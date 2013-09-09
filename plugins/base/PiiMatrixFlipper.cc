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

#include "PiiMatrixFlipper.h"

#include <PiiYdinTypes.h>
#include <PiiMatrixUtil.h>

PiiMatrixFlipper::Data::Data() :
  flipMode(FlipHorizontally)
{
}

PiiMatrixFlipper::PiiMatrixFlipper() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("input"));
  addSocket(new PiiOutputSocket("output"));
}

void PiiMatrixFlipper::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_ALL_MATRIX_CASES(flip, obj);
      PII_COLOR_IMAGE_CASES(flip, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiMatrixFlipper::flip(const PiiVariant& obj)
{
  PII_D;
  int direction;
  switch (d->flipMode)
    {
    case FlipBoth:
      direction = Pii::Horizontally | Pii::Vertically;
      break;
    case FlipHorizontally:
      direction = Pii::Horizontally;
      break;
    default:
      direction = Pii::Vertically;
      break;
    }
  emitObject(Pii::flipped(obj.valueAs<PiiMatrix<T> >(), (Pii::MatrixDirections)direction));
}

void PiiMatrixFlipper::setFlipMode(const FlipMode& flipMode) { _d()->flipMode = flipMode; }
PiiMatrixFlipper::FlipMode PiiMatrixFlipper::flipMode() const { return _d()->flipMode; }
