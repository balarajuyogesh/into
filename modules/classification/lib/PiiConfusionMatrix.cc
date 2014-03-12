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

#include "PiiConfusionMatrix.h"

PiiConfusionMatrix::PiiConfusionMatrix(int size) :
  PiiMatrix<int>(size,size)
{
}

PiiConfusionMatrix::PiiConfusionMatrix(const PiiConfusionMatrix& mat) :
  PiiMatrix<int>(mat)
{
}

PiiConfusionMatrix::PiiConfusionMatrix(const PiiMatrix<int>& mat) :
  PiiMatrix<int>(mat(0, 0, qMin(mat.rows(), mat.columns()), qMin(mat.rows(), mat.columns())))
{
}

PiiConfusionMatrix& PiiConfusionMatrix::operator=(const PiiConfusionMatrix& other)
{
  PiiMatrix<int>::operator=(other);
  return *this;
}


double PiiConfusionMatrix::mixup(int column) const
{
  int wrong = 0, correct = 0;

  for (int i=rows(); i--; )
    {
      if (i != column)
        wrong += (*this)(i,column);
      else
        correct = (*this)(i,column);
    }

  return double(wrong)/double(wrong+correct);
}

double PiiConfusionMatrix::error(int row) const
{
  int wrong = 0, correct = 0;

  for (int i=columns(); i--; )
    {
      if (i != row)
        wrong += (*this)(row, i);
      else
        correct = (*this)(row, i);
    }

  return double(wrong)/double(wrong+correct);
}

double PiiConfusionMatrix::error() const
{
  int wrong = 0, correct = 0;
  for (int r=rows(); r--; )
    {
      for (int c=columns(); c--; )
        {
          if (r != c)
            wrong += (*this)(r,c);
          else
            correct += (*this)(r,c);
        }
    }
  return double(wrong)/double(wrong+correct);
}

void PiiConfusionMatrix::addEntry(int correctClass, int classification)
{
  if (correctClass < 0 || classification < 0)
    return;

  int max = qMax(correctClass, classification) + 1;
  if (max > rows())
    resize(max,max);
  ++(*this)(correctClass,classification);
}
