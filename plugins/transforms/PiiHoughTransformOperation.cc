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

#include "PiiHoughTransformOperation.h"
#include "PiiTransforms.h"
#include <PiiMath.h>


PiiHoughTransformOperation::Data::Data() :
  iMaxPeakCount(1),
  dMinPeakMagnitude(0),
  bPeaksConnected(false),
  dMinPeakDistance(1)
{
}

PiiHoughTransformOperation::PiiHoughTransformOperation() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("accumulator"));
  addSocket(new PiiOutputSocket("peaks"));
  addSocket(new PiiOutputSocket("coordinates"));
}

PiiHoughTransformOperation::~PiiHoughTransformOperation()
{}

void PiiHoughTransformOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);

  if (startAngle() >= endAngle())
    PII_THROW(PiiExecutionException, tr("Start angle must be smaller than end angle."));
  if (startDistance() >= endDistance())
    PII_THROW(PiiExecutionException, tr("Start distance must be smaller than end distance."));

  d->bPeaksConnected = outputAt(1)->isConnected() || outputAt(2)->isConnected();
}

void PiiHoughTransformOperation::process()
{
  PiiVariant obj = readInput();
   
   switch (obj.type())
     {
       // For gray images do hough transform directly
       PII_GRAY_IMAGE_CASES(transform, obj);
       // For color images throw unknow type
      default:
       PII_THROW_UNKNOWN_TYPE(inputAt(0));
     }
}


template <class T> void PiiHoughTransformOperation::transform(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> image = obj.valueAs<PiiMatrix<T> >();
  typedef typename TransformTraits<T>::Type ResultType;
  PiiMatrix<ResultType> accumulator;
   
  accumulator = d->hough.transform<ResultType>(image, Pii::Identity<T>());
  
  if (d->bPeaksConnected)
    findPeaks(accumulator);
 
  emitObject(accumulator);
}
  
template <class T> void PiiHoughTransformOperation::findPeaks(const PiiMatrix<T>& accumulator)

{
  PII_D;
  QList<PiiMatrixValue<T> > lstPeaks(PiiTransforms::findPeaks(accumulator,
                                                              d->dMinPeakDistance,
                                                              d->iMaxPeakCount,
                                                              T(d->dMinPeakMagnitude)));
  
  PiiMatrix<double> matPeaks(0,2);
  PiiMatrix<int> matCoordinates(0,4);
   
  for (int i=0; i<lstPeaks.size(); ++i)
    {
      matPeaks.appendRow(d->hough.distance(lstPeaks[i].row), d->hough.angle(lstPeaks[i].column));
      matCoordinates.appendRow(d->hough.lineEnds(lstPeaks[i].row, lstPeaks[i].column));
    }

  outputAt(1)->emitObject(matPeaks);
  outputAt(2)->emitObject(matCoordinates);
}

void PiiHoughTransformOperation::setMaxPeakCount(int value) { _d()->iMaxPeakCount = value; }
int PiiHoughTransformOperation::maxPeakCount() const { return _d()->iMaxPeakCount; }

void PiiHoughTransformOperation::setAngleResolution(double resolution) { _d()->hough.setAngleResolution(resolution); }
double PiiHoughTransformOperation::angleResolution() const { return _d()->hough.angleResolution(); }
void PiiHoughTransformOperation::setDistanceResolution(double resolution) { _d()->hough.setDistanceResolution(resolution); }
double PiiHoughTransformOperation::distanceResolution() const { return _d()->hough.distanceResolution(); }
void PiiHoughTransformOperation::setStartAngle(int startAngle) { _d()->hough.setStartAngle(startAngle); }
int PiiHoughTransformOperation::startAngle() const { return _d()->hough.startAngle(); }
void PiiHoughTransformOperation::setEndAngle(int endAngle) { _d()->hough.setEndAngle(endAngle); }
int PiiHoughTransformOperation::endAngle() const { return _d()->hough.endAngle(); }
void PiiHoughTransformOperation::setStartDistance(int startDistance) { _d()->hough.setStartDistance(startDistance); }
int PiiHoughTransformOperation::startDistance() const { return _d()->hough.startDistance(); }
void PiiHoughTransformOperation::setEndDistance(int endDistance) { _d()->hough.setEndDistance(endDistance); }
int PiiHoughTransformOperation::endDistance() const { return _d()->hough.endDistance(); }
void PiiHoughTransformOperation::setMinPeakMagnitude(double minPeakMagnitude) { _d()->dMinPeakMagnitude = minPeakMagnitude; }
double PiiHoughTransformOperation::minPeakMagnitude() const { return _d()->dMinPeakMagnitude; }
void PiiHoughTransformOperation::setMinPeakDistance(double minPeakDistance) { _d()->dMinPeakDistance = qMax(1.0, minPeakDistance); }
double PiiHoughTransformOperation::minPeakDistance() const { return _d()->dMinPeakDistance; }
