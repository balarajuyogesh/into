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

#include "PiiPeakDetector.h"
#include "PiiDsp.h"

#include <PiiYdinTypes.h>

PiiPeakDetector::Data::Data() :
  dLevelThreshold(0),
  dSharpnessThreshold(0.001),
  iSmoothWidth(5),
  iWindowWidth(7),
 iLevelCorrectionWindow(0)
{
}

PiiPeakDetector::PiiPeakDetector() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("signal"));
  addSocket(new PiiOutputSocket("peaks"));
  addSocket(new PiiOutputSocket("indices"));
}

void PiiPeakDetector::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_NUMERIC_MATRIX_CASES(findPeaks, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiPeakDetector::findPeaks(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> matrix = obj.valueAs<PiiMatrix<T> >();
  QList<PiiDsp::Peak> lstPeaks = PiiDsp::findPeaks(adjustLevel(matrix),
                                                   d->dLevelThreshold,
                                                   d->dSharpnessThreshold,
                                                   d->iSmoothWidth,
                                                   d->iWindowWidth);
  
  PiiMatrix<double> matPeaks(lstPeaks.size(), 3);
  PiiMatrix<int> matIndices(lstPeaks.size(), 1);
  for (int i=0; i<lstPeaks.size(); ++i)
    {
      matPeaks(i,0) = lstPeaks[i].position;
      matPeaks(i,1) = lstPeaks[i].height;
      matPeaks(i,2) = lstPeaks[i].width;
      matIndices(i,0) = lstPeaks[i].dataIndex;
    }
  emitObject(matPeaks, 0);
  emitObject(matIndices, 1);
}

template <class T> PiiMatrix<double> PiiPeakDetector::adjustLevel(const PiiMatrix<T>& matrix)
{
  PII_D;
  if (d->iLevelCorrectionWindow <= 1)
    return PiiMatrix<double>(matrix);

  // Calculate moving average of the input matrix
  PiiMatrix<double> matAvg =  Pii::movingAverage<double>(matrix, d->iLevelCorrectionWindow,
                                                         Pii::Horizontally, Pii::ShrinkWindow);
  // Turn zeros to ones
  std::replace_if(matAvg.rowBegin(0), matAvg.rowEnd(0), std::bind2nd(std::equal_to<double>(), 0.0), 1.0);
  
  // Divide each value by the local average
  return Pii::matrix(Pii::divided(PiiMatrix<double>(matrix), matAvg));
}

void PiiPeakDetector::setLevelThreshold(double levelThreshold) { _d()->dLevelThreshold = levelThreshold; }
double PiiPeakDetector::levelThreshold() const { return _d()->dLevelThreshold; }
void PiiPeakDetector::setSharpnessThreshold(double sharpnessThreshold) { _d()->dSharpnessThreshold = sharpnessThreshold; }
double PiiPeakDetector::sharpnessThreshold() const { return _d()->dSharpnessThreshold; }
void PiiPeakDetector::setSmoothWidth(int smoothWidth) { _d()->iSmoothWidth = smoothWidth; }
int PiiPeakDetector::smoothWidth() const { return _d()->iSmoothWidth; }
void PiiPeakDetector::setWindowWidth(int windowWidth) { _d()->iWindowWidth = windowWidth; }
int PiiPeakDetector::windowWidth() const { return _d()->iWindowWidth; }
void PiiPeakDetector::setLevelCorrectionWindow(int levelCorrectionWindow) { _d()->iLevelCorrectionWindow = levelCorrectionWindow; }
int PiiPeakDetector::levelCorrectionWindow() const { return _d()->iLevelCorrectionWindow; }
