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

#include "PiiShapeContextOperation.h"

#include <PiiYdinTypes.h>
#include <PiiGeometry.h>
#include <PiiMatching.h>

#include <PiiMatrixUtil.h>
#include <QtDebug>


PiiShapeContextOperation::Data::Data() :
  iSamplingStep(5),
  dTolerance(1),
  iAngles(16),
  iDistances(5),
  dMinDistance(5),
  dDistanceScale(2.2),
  bCollectDistantPoints(false),
  invariance(PiiMatching::RotationInvariant),
  keyPointSelectionMode(SelectEveryNthPoint),
  shapeJoiningMode(JoinNestedShapes),
  iLastImageRows(0),
  iLastImageColumns(0),
  iMaxPoints(0)
{
}

PiiShapeContextOperation::PiiShapeContextOperation() : PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;
  addSocket(d->pBoundariesInput = new PiiInputSocket("boundaries"));
  addSocket(d->pLimitsInput = new PiiInputSocket("limits"));
  addSocket(d->pImageInput = new PiiInputSocket("image"));
  addSocket(d->pDirectionInput = new PiiInputSocket("direction"));
  d->pBoundariesInput->setOptional(true);
  d->pLimitsInput->setOptional(true);
  d->pImageInput->setOptional(true);
  d->pDirectionInput->setOptional(true);
  
  addSocket(d->pPointsOutput = new PiiOutputSocket("points"));
  addSocket(d->pFeaturesOutput = new PiiOutputSocket("features"));
  addSocket(d->pBoundariesOutput = new PiiOutputSocket("boundaries"));
  addSocket(d->pLimitsOutput = new PiiOutputSocket("limits"));

  setProtectionLevel("samplingStep", WriteWhenStoppedOrPaused);
  
}

void PiiShapeContextOperation::check(bool reset)
{
  PII_D;

  int iConnectedFlags = 0;
  if (d->pBoundariesInput->isConnected())
    iConnectedFlags |= 1;
  if (d->pLimitsInput->isConnected())
    iConnectedFlags |= 2;
  if (d->pImageInput->isConnected())
    iConnectedFlags |= 4;
  if (d->pDirectionInput->isConnected())
    iConnectedFlags |= 8;

  if (iConnectedFlags not_member_of (1, 3, 4, 12))
    PII_THROW(PiiExecutionException, tr("Either boundaries or image must be connected. "
                                        "Additionally, either limits or direction may be connected."));

  // Create vecDistances
  d->vecDistances = QVector<double>(d->iDistances);
  d->vecDistances[0] = d->dMinDistance;
  for (int i=1; i<d->vecDistances.count(); i++)
    d->vecDistances[i] = d->vecDistances[i-1] * d->dDistanceScale;
  if (d->bCollectDistantPoints)
    d->vecDistances[d->vecDistances.count()-1] = INFINITY;

  // Use squared distances
  for (int i=0; i<d->vecDistances.count(); i++)
    d->vecDistances[i] *= d->vecDistances[i];
  
  PiiDefaultOperation::check(reset);
}

PiiMatrix<int> PiiShapeContextOperation::readIntMatrix(PiiInputSocket* input)
{
  PiiVariant obj = input->firstObject();
  if (obj.type() != PiiYdin::IntMatrixType)
    PII_THROW_UNKNOWN_TYPE(input);
  return obj.valueAs<PiiMatrix<int> >();
}

template <class T> PiiMatrix<int> PiiShapeContextOperation::createBoundaries(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T>& img(obj.valueAs<PiiMatrix<T> >());

  // These are checked by pickAngles()
  d->iLastImageRows = img.rows();
  d->iLastImageColumns = img.columns();
  
  PiiMatrix<int> matResult(0,2);
  matResult.reserve(512);

  for (typename PiiMatrix<T>::const_iterator i = img.begin(); i != img.end(); ++i)
    if (*i != 0)
      matResult.appendRow(i.column(), i.row());

  return matResult;
}

PiiMatrix<int> PiiShapeContextOperation::createBoundaries()
{
  PII_D;
  PiiVariant obj = d->pImageInput->firstObject();
  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(return createBoundaries, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(d->pImageInput);
    }
  // Suppress spurious compiler warning
  return PiiMatrix<int>();
}

void PiiShapeContextOperation::process()
{
  PII_D;
  bool bBoundariesConnected = d->pBoundariesInput->isConnected();
  bool bLimitsConnected = d->pLimitsInput->isConnected();
  PiiMatrix<int> matBoundaries(bBoundariesConnected ?
                               readIntMatrix(d->pBoundariesInput) :
                               createBoundaries());
  PiiMatrix<int> matLimits(bLimitsConnected ?
                           readIntMatrix(d->pLimitsInput) :
                           PiiMatrix<int>(1,1, matBoundaries.rows()));

  if (bLimitsConnected)
    startMany();

  switch (bBoundariesConnected ? d->shapeJoiningMode : JoinAllShapes)
    {
    case DoNotJoinShapes:
      {
        int iStart=0;
        for (int i=0; i<matLimits.columns(); i++)
          {
            processBoundary(matBoundaries(iStart,0,matLimits(0,i)-iStart,-1), PiiMatrix<int>(1,1,matLimits(0,i)-iStart));
            iStart = matLimits(0,i);
          }
        break;
      }
    case JoinNestedShapes:
      {
        PiiMatrix<int> matJoinedBoundaries(matBoundaries.rows(), matBoundaries.columns());
        QList<PiiMatrix<int> > lstJoinedLimits = joinNestedShapes(matBoundaries, matLimits, matJoinedBoundaries);

        int iStart=0, iRows = 0;
        for (int i=0; i<lstJoinedLimits.size(); i++)
          {
            // Get the last limit
            iRows = lstJoinedLimits[i](0,lstJoinedLimits[i].columns()-1);

            // Process boundary
            processBoundary(matJoinedBoundaries(iStart,0,iRows,-1), lstJoinedLimits[i]);

            // Save the next start point
            iStart += iRows;
          }
        break;
      }
    case JoinAllShapes:
      processBoundary(matBoundaries, PiiMatrix<int>(1,1, matBoundaries.rows()));
      break;
    }

  if (bLimitsConnected)
    endMany();
}

template <class T> QVector<double> PiiShapeContextOperation::pickAngles(const PiiVariant& obj,
                                                                        const PiiMatrix<int>& points)
{
  PII_D;
  const PiiMatrix<T>& matDirections(obj.valueAs<PiiMatrix<T> >());
  if (matDirections.rows() != d->iLastImageRows ||
      matDirections.columns() != d->iLastImageColumns)
    PII_THROW(PiiExecutionException, tr("Image and direction matrix must have equal sizes."));
  QVector<double> vecResult;
  vecResult.reserve(points.rows());
  for (int r=0; r<points.rows(); ++r)
    vecResult << matDirections(points(r,1), points(r,0));
  return vecResult;
}

QVector<double> PiiShapeContextOperation::pickAngles(const PiiMatrix<int>& points)
{
  PII_D;
  if (!d->pDirectionInput->isConnected())
    return QVector<double>(points.rows());

  PiiVariant obj = d->pDirectionInput->firstObject();
  switch (obj.type())
    {
      PII_FLOAT_MATRIX_CASES_M(return pickAngles, (obj, points));
    default:
      PII_THROW_UNKNOWN_TYPE(d->pDirectionInput);
    }
  // Suppress spurious compiler warning
  return QVector<double>();
}

void PiiShapeContextOperation::processBoundary(const PiiMatrix<int>& boundary, const PiiMatrix<int>& limits)
{
  PII_D;

  // Calculate vector angles
  QVector<double> vecAngles;
  PiiMatrix<int> matKeyPoints(0, boundary.columns());
  matKeyPoints.reserve(512);
  
  if (boundary.rows() <= 3)
    matKeyPoints = boundary;
  else if (d->invariance & PiiMatching::RotationInvariant)
    {
      int iStart = 0;
      for (int i=0; i<limits.columns(); i++)
        {
          int iRows = limits(0,i)-iStart;
          if (iRows > 3)
            {
              // First find the key points that need to be retained
              PiiMatrix<int> matBoundaryKeyPoints = reducePoints(boundary(iStart,0,iRows,-1));
              // If the boundaries input is connected, we can assume boundaries are chained.
              if (d->pBoundariesInput->isConnected())
                // Calculate boundary direction for each key point
                vecAngles << PiiMatching::boundaryDirections(matBoundaryKeyPoints);
              // Boundaries were picked from an image -> use direction
              // input to get the angles.
              else
                vecAngles << pickAngles(matBoundaryKeyPoints);

              matKeyPoints.appendRows(matBoundaryKeyPoints);
              
              // boundaryDirections returns one angle less if start
              // and end points are the same.
              if (vecAngles.size() < matKeyPoints.rows())
                matKeyPoints.resize(matKeyPoints.rows()-1, matKeyPoints.columns());
            }
          iStart = limits(0,i);
        }
    }
  else
    matKeyPoints = reducePoints(boundary);
      
  // Create feature matrix
  PiiMatrix<float> matFeatures = PiiMatching::shapeContextDescriptor(boundary,
                                                                     matKeyPoints,
                                                                     d->iAngles,
                                                                     d->vecDistances,
                                                                     vecAngles,
                                                                     d->invariance);

  d->pPointsOutput->emitObject(matKeyPoints);
  d->pFeaturesOutput->emitObject(matFeatures);
  d->pBoundariesOutput->emitObject(boundary);
  d->pLimitsOutput->emitObject(limits);
}

PiiMatrix<int> PiiShapeContextOperation::reducePoints(const PiiMatrix<int>& boundary) //, bool addLastPoint)
{
  PII_D;

  // Cannot select important points if boundaries were collected from
  // an image.
  switch (d->pBoundariesInput->isConnected() ? d->keyPointSelectionMode : SelectEveryNthPoint)
    {
    case SelectImportantPoints:
      {
        PiiMatrix<int> result = PiiGeometry::reduceVertices(boundary, d->dTolerance * d->dTolerance);
        //if (!addLastPoint)
        result.resize(result.rows()-1, result.columns());
        
        return result;
      }
    case SelectEveryNthPoint:
      {
        if (d->iSamplingStep == 1 && d->iMaxPoints == 0)
          return boundary;

        double dStep = 1;
        if (d->iMaxPoints > 1)
          dStep = double(boundary.rows()-1) / (d->iMaxPoints-1);
        else if (d->iMaxPoints == 1)
          dStep = boundary.rows();
        else
          dStep = d->iSamplingStep;

        // Check the sampling limits
        int iSize = int(double(boundary.rows()) / dStep + 0.5);
        
        if (iSize <= 4)
          return boundary;

        // Select every Nth point
        PiiMatrix<int> matResult(0, boundary.columns());
        matResult.reserve(iSize + 1);
        for (double i=0; i<boundary.rows(); i += dStep)
          matResult.appendRow(boundary[int(i)]);

        /*if (addLastPoint)
          {
            // Append last point if necessary
            if (boundary.rows()-1 % d->iSamplingStep != 0)
              matResult.appendRow(boundary[boundary.rows()-1]);
          }
        */

        return matResult;
      }
    }

  return boundary;
}

QList<PiiMatrix<int> > PiiShapeContextOperation::joinNestedShapes(const PiiMatrix<int>& boundaries,
                                                                  const PiiMatrix<int>& limits,
                                                                  PiiMatrix<int>& joinedBoundaries)
{
  // Initialize variables
  QList<PiiMatrix<int> > lstBoundaries;
  PiiMatrix<int> matLimitLengths(1,limits.columns());
  QVector<QList<int> > lstJoinedIndices(limits.columns());
  QList<int> lstMainShapes;

  int iPreviousLimit = 0;
  for (int i=0; i<limits.columns(); i++)
    {
      lstBoundaries << boundaries(iPreviousLimit,0,limits(0,i)-iPreviousLimit,-1);
      matLimitLengths(0,i) = limits(0,i) - iPreviousLimit;
      iPreviousLimit = limits(0,i);

      lstJoinedIndices[i].append(i);
      lstMainShapes << i;
    }

  // Check the nested shapes
  for (int i=0; i<lstBoundaries.size(); i++)
    {
      int x = lstBoundaries[i](0,0);
      int y = lstBoundaries[i](0,1);
      
      for (int j=0; j<lstBoundaries.size(); j++)
        {
          if (i != j && PiiGeometry::contains(lstBoundaries[j], x, y))
            {
              lstJoinedIndices[j].append(i);
              lstMainShapes.removeAll(i);
            }
        }
    }
  for (int i=0; i<lstJoinedIndices.size(); i++)
    if (!lstMainShapes.contains(i))
      lstJoinedIndices[i].clear();

  // Join boundaries and limits
  QList<PiiMatrix<int> > lstLimits;
  int iJoinedStartPoint = 0;
  for (int i=0; i<lstJoinedIndices.size(); i++)
    {
      if (lstMainShapes.contains(i))
        {
          QList<int> lstIndices = lstJoinedIndices[i];
          PiiMatrix<int> matLimits(1,lstIndices.size());
          int iStart = 0;
          for (int j=0; j<lstIndices.size(); j++)
            {
              int index = lstIndices[j];
              int iLength = matLimitLengths(0,index);
              
              // Calculate limits
              matLimits(0,j) = iLength + iStart;
              iStart = matLimits(0,j);
              
              // Reorganize boundaries
              joinedBoundaries(iJoinedStartPoint,0,iLength,-1) << boundaries(limits(0,index)-iLength,0,iLength,-1);
              iJoinedStartPoint += iLength;
            }

          // Append calculated limits to the list
          lstLimits << matLimits;
        }
    }

  return lstLimits;
}


void PiiShapeContextOperation::setSamplingStep(int samplingStep) { _d()->iSamplingStep = qMax(1,samplingStep); }
int PiiShapeContextOperation::samplingStep() const { return _d()->iSamplingStep; }
void PiiShapeContextOperation::setAngles(int angles) { _d()->iAngles = qMin(360,qMax(1,angles)); }
int PiiShapeContextOperation::angles() const { return _d()->iAngles; }
void PiiShapeContextOperation::setDistances(int distances) { _d()->iDistances = qMin(128,qMax(1,distances)); }
int PiiShapeContextOperation::distances() const { return _d()->iDistances; }
void PiiShapeContextOperation::setMinDistance(double minDistance) { _d()->dMinDistance = minDistance; }
double PiiShapeContextOperation::minDistance() const { return _d()->dMinDistance; }
void PiiShapeContextOperation::setDistanceScale(double distanceScale) { _d()->dDistanceScale = distanceScale; }
double PiiShapeContextOperation::distanceScale() const { return _d()->dDistanceScale; }
void PiiShapeContextOperation::setTolerance(double tolerance) { _d()->dTolerance = tolerance; }
double PiiShapeContextOperation::tolerance() const { return _d()->dTolerance; }
void PiiShapeContextOperation::setCollectDistantPoints(bool collectDistantPoints) { _d()->bCollectDistantPoints = collectDistantPoints; }
bool PiiShapeContextOperation::collectDistantPoints() const { return _d()->bCollectDistantPoints; }
void PiiShapeContextOperation::setInvariance(PiiMatching::InvarianceFlags invariance) { _d()->invariance = invariance; }
PiiMatching::InvarianceFlags PiiShapeContextOperation::invariance() const { return _d()->invariance; }
void PiiShapeContextOperation::setKeyPointSelectionMode(KeyPointSelectionMode keyPointSelectionMode)
{ _d()->keyPointSelectionMode = keyPointSelectionMode; }
PiiShapeContextOperation::KeyPointSelectionMode PiiShapeContextOperation::keyPointSelectionMode() const
{ return _d()->keyPointSelectionMode; }
void PiiShapeContextOperation::setShapeJoiningMode(ShapeJoiningMode shapeJoiningMode)
{ _d()->shapeJoiningMode = shapeJoiningMode; }
PiiShapeContextOperation::ShapeJoiningMode PiiShapeContextOperation::shapeJoiningMode() const
{ return _d()->shapeJoiningMode; }
void PiiShapeContextOperation::setMaxPoints(int maxPoints) { _d()->iMaxPoints = qMax(0, maxPoints); }
int PiiShapeContextOperation::maxPoints() const { return _d()->iMaxPoints; }
