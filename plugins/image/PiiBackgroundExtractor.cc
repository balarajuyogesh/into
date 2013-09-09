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

#include <PiiBackgroundExtractor.h>
#include <PiiYdinTypes.h>
#include <QString>
#include <PiiMath.h>
#include <PiiImageTraits.h>

PiiBackgroundExtractor::Data::Data() :
  bFirst(true), dThreshold(25.0),
  dAlpha1(0.1), dAlpha2(0.01),
  iMaxStillTime(1000), dMovementThreshold(0.0)
{
}

PiiBackgroundExtractor::PiiBackgroundExtractor() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("image"));
  
  addSocket(new PiiOutputSocket("image"));
  addSocket(new PiiOutputSocket("movement"));
}

void PiiBackgroundExtractor::process()
{
  PiiVariant obj = readInput();
  switch (obj.type())
    {
      PII_ALL_IMAGE_CASES(operate, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiBackgroundExtractor::operate(const PiiVariant& obj)
{
  PII_D;
  PiiMatrix<float> inputMatrix(obj.valueAs<PiiMatrix<T> >());
  double dDifference = 0;
  int iValidCounter = 0;

  const int iRows = inputMatrix.rows(), iCols = inputMatrix.columns();
  
  if (d->bFirst) // Initialize.
    {
      // Init support matrices.
      d->matStillCounter = PiiMatrix<int>(iRows, iCols);
      d->matBackground = PiiMatrix<float>(inputMatrix); // Init with input image values.
      d->matForeground = PiiMatrix<float>(iRows, iCols);
      d->bFirst = false;
    }
  else // Update model.
    {
      if (iRows != d->matBackground.rows() || iCols != d->matBackground.columns())
        PII_THROW_WRONG_SIZE(inputAt(0), inputMatrix, d->matBackground.rows(), d->matBackground.columns());
      
      for (int r=0; r<iRows; ++r)
        {
          float* pInputRow = inputMatrix[r];
          float* pBackgroundRow = d->matBackground[r];
          float* pForegroundRow = d->matForeground[r];
          int* pStillCounterRow = d->matStillCounter[r];
          for (int c=0; c<iCols; ++c)
            {
              //float dInputValue = PiiImage::Traits<T>::template toFloat<float>(pInputRow[c]);
              dDifference = pInputRow[c] - pBackgroundRow[c];

              // If the difference between background model and the
              // currente frame is big enough, mark the pixel as moved.
              if (Pii::abs(dDifference) > d->dThreshold)
                {
                  // Count the number of successive frames in which
                  // the same pixel is "foreground".
                  ++pStillCounterRow[c];
                  iValidCounter++;
                  pForegroundRow[c] = 1.0f;
                }
              else
                {
                  pStillCounterRow[c] = 0;
                  pForegroundRow[c] = 0.0f;
                }

              // Now, this pixel has been "foreground" way too long.
              if (pStillCounterRow[c] > d->iMaxStillTime)
                {
                  pStillCounterRow[c] = 0;
                  pBackgroundRow[c] = pInputRow[c];
                }

              // Update background model.
              // B_t+1 = B_t + (alpha1 * (1 - M_t) + alpha2 * M_t) * D_t
              pBackgroundRow[c] += float((d->dAlpha1 * (1.0 - pForegroundRow[c]) + 
                                          d->dAlpha2 * pForegroundRow[c]) * dDifference);
            }
        }
    }

  // Too many foreground pixels -> there is something wrong
  emitObject(iValidCounter < (d->dMovementThreshold * iRows * iCols), 1);
  emitObject(d->matStillCounter, 0);
}

double PiiBackgroundExtractor::threshold() const { return _d()->dThreshold; }
void PiiBackgroundExtractor::setThreshold(double threshold) { _d()->dThreshold = threshold; }
double PiiBackgroundExtractor::alpha1() const { return _d()->dAlpha1; }
void PiiBackgroundExtractor::setAlpha1(double alpha1) { _d()->dAlpha1 = alpha1; }
double PiiBackgroundExtractor::alpha2() const { return _d()->dAlpha2; }
void PiiBackgroundExtractor::setAlpha2(double alpha2) { _d()->dAlpha2 = alpha2; }
void PiiBackgroundExtractor::setMaxStillTime(int maxStillTime) { _d()->iMaxStillTime = maxStillTime; }
int PiiBackgroundExtractor::maxStillTime() const { return _d()->iMaxStillTime; }
void PiiBackgroundExtractor::setMovementThreshold(double movementThreshold) { _d()->dMovementThreshold = movementThreshold; }
float PiiBackgroundExtractor::movementThreshold() const { return _d()->dMovementThreshold; }
