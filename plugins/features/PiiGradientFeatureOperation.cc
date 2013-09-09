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

#include "PiiGradientFeatureOperation.h"

#include <PiiYdinTypes.h>
#include <PiiMath.h>

PiiGradientFeatureOperation::PiiGradientFeatureOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("features"));
}

void PiiGradientFeatureOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(calculateGradient, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiGradientFeatureOperation::calculateGradient(const PiiVariant& obj)
{
  const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();

  float fMaxVerticalGradient = 0;
  float fGradient;
  for (int r=img.rows()-1; r--; )
    {
      const T* row0 = img[r];
      const T* row1 = img[r+1];
      for (int c=img.columns(); c--; )
        {
          fGradient = Pii::abs(float(row1[c]) - float(row0[c]));
          if (fGradient > fMaxVerticalGradient)
            fMaxVerticalGradient = fGradient;
        }
    }

  float fMaxHorizontalGradient = 0;
  for (int r=img.rows(); r--; )
    {
      const T* row = img[r];
      for (int c=img.columns()-1; c--; )
        {
          fGradient = Pii::abs(float(row[c+1]) - float(row[c]));
          if (fGradient > fMaxHorizontalGradient)
            fMaxHorizontalGradient = fGradient;
        }
    }
  
  PiiMatrix<float> matFeatures(1, 2,
                               double(fMaxVerticalGradient),
                               double(fMaxHorizontalGradient));
}


QStringList PiiGradientFeatureOperation::features() const { return QStringList() << "VGradient" << "HGradient"; }
