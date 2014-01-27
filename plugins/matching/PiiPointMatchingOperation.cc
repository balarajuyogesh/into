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

#include "PiiPointMatchingOperation.h"

#include <PiiClassification.h>
#include <PiiSquaredGeometricDistance.h>
#include <PiiYdinTypes.h>
#include <PiiRigidPlaneRansac.h>

#include <PiiMatrixUtil.h>

PiiPointMatchingOperation::Data::Data(int pointDimensions) :
  PiiClassifierOperation::Data(PiiClassification::NonSupervisedLearner),
  matIdentity(PiiMatrix<float>::identity(pointDimensions+1)),
  matEmptyLocation(1, pointDimensions*2),
  matEmptyPoint(1, pointDimensions),
  pMatcher(new Matcher), pNewMatcher(0),
  iModelCount(0),
  iPointDimensions(pointDimensions),
  matchingMode(PiiMatching::MatchAllModels),
  bMustSendPoints(false),
  iClosestMatchCount(pMatcher->closestMatchCount())
{
}

PiiPointMatchingOperation::Data::~Data()
{
  delete pMatcher;
  delete pNewMatcher;
}

PiiPointMatchingOperation::PiiPointMatchingOperation(int pointDimensions) :
  PiiClassifierOperation(new Data(pointDimensions))
{
  init();
}

PiiPointMatchingOperation::PiiPointMatchingOperation(Data* data) :
  PiiClassifierOperation(data)
{
  init();
}

void PiiPointMatchingOperation::init()
{
  PII_D;
  addSocket(d->pPointsInput = new PiiInputSocket("points"));
  addSocket(d->pLocationInput = new PiiInputSocket("location"));
  d->pLocationInput->setOptional(true);

  addSocket(d->pModelIndexOutput = new PiiOutputSocket("model index"));
  addSocket(d->pLocationOutput = new PiiOutputSocket("location"));
  addSocket(d->pTransformOutput = new PiiOutputSocket("transform"));
  addSocket(d->pQueryPointsOutput = new PiiOutputSocket("query points"));
  addSocket(d->pModelPointsOutput = new PiiOutputSocket("model points"));

  setProtectionLevel("matchingMode", WriteWhenStoppedOrPaused);
}

PiiPointMatchingOperation::~PiiPointMatchingOperation()
{}

void PiiPointMatchingOperation::check(bool reset)
{
  PII_D;

  PiiClassifierOperation::check(reset);

  if (!d->pMatcher->modelPoints().isEmpty())
    {
      if (d->matLocations.isEmpty())
        PII_THROW(PiiExecutionException, tr("Point matcher has at least one model but no model locations specified."));
      if (!d->lstLabels.isEmpty() && d->lstLabels.size() != d->matLocations.rows())
        PII_THROW(PiiExecutionException, tr("There must be an equal number of model locations and labels."));
    }

  d->pMatcher->setMatchingMode(d->matchingMode);
  d->bMustSendPoints = d->pModelPointsOutput->isConnected() ||
    d->pQueryPointsOutput->isConnected();
}

PiiPointMatchingOperation::Matcher* PiiPointMatchingOperation::createMatcher()
{
  Matcher* pMatcher = new Matcher;
  pMatcher->setClosestMatchCount(_d()->iClosestMatchCount);
  return pMatcher;
}

bool PiiPointMatchingOperation::learnBatch()
{
  PII_D;
  try
    {
      d->pNewMatcher = createMatcher();
      d->pNewMatcher->buildDatabase(d->matNewPoints,
                                    d->matNewFeatures,
                                    d->vecNewModelIndices,
                                    this);
    }
  catch (PiiClassificationException& ex)
    {
      delete d->pNewMatcher;
      d->pNewMatcher = 0;
      setLearningError(ex.message());
      return false;
    }
  return true;
}

void PiiPointMatchingOperation::replaceClassifier()
{
  PII_D;
  delete d->pMatcher;
  d->pMatcher = d->pNewMatcher;
  d->pNewMatcher = 0;
  d->lstLabels = d->lstNewLabels;
  d->matLocations = d->matNewLocations;

  resizeBatch(0);
}

void PiiPointMatchingOperation::resetClassifier()
{
  PII_D;
  delete d->pMatcher;
  d->pMatcher = createMatcher();
  d->lstLabels.clear();
  d->matLocations.clear();
}

void PiiPointMatchingOperation::resizeBatch(int newSize)
{
  PII_D;
  if (newSize == 0)
    {
      d->lstNewLabels.clear();
      d->matNewPoints.clear();
      d->matNewLocations.clear();
      d->matNewFeatures.clear();
      d->vecNewModelIndices.clear();
      d->iModelCount = 0;
    }
  else
    {
      // PENDING
    }
}

int PiiPointMatchingOperation::bufferedSampleCount() const
{
  return _d()->iModelCount;
}

int PiiPointMatchingOperation::featureCount() const
{
  return _d()->matNewFeatures.columns();
}


int PiiPointMatchingOperation::checkDescriptor(const PiiMatrix<float>& points,
                                               const PiiMatrix<float>& features) const
{
  const PII_D;
  const int iPoints = points.rows();
  if (iPoints < 1)
    PII_THROW(PiiExecutionException, tr("The received descriptor contains no feature points."));
  if (points.columns() != d->iPointDimensions)
    PII_THROW(PiiExecutionException,
              tr("Input points must be %1-dimensional. Received %2-dimensional points")
              .arg(d->iPointDimensions)
              .arg(points.columns()));

  if (features.rows() != iPoints)
    PII_THROW(PiiExecutionException,
              tr("Each feature point must have a descriptor. Got %1 points and %2 features.")
              .arg(points.rows()).arg(features.rows()));

  const int iFeatures = featureCount();
  if (iFeatures != 0 && iFeatures != features.columns())
    PII_THROW(PiiExecutionException,
              tr("All feature points must be described with the same number of features. "
                 "The model database has %1 features, the current feature matrix has %2.")
              .arg(iFeatures)
              .arg(features.columns()));

  return iPoints;
}

double PiiPointMatchingOperation::classify()
{
  PII_D;

  PiiMatrix<float> matFeatures = PiiYdin::convertMatrixTo<float>(featureInput());
  PiiMatrix<float> matPoints = PiiYdin::convertMatrixTo<float>(d->pPointsInput);

  checkDescriptor(matPoints, matFeatures);

  PiiMatching::MatchList lstMatches = match(*d->pMatcher, matPoints, matFeatures);
  if (d->matchingMode != PiiMatching::MatchOneModel)
    {
      startMany();
      if (lstMatches.size() > 1)
        removeDuplicates(lstMatches);
      for (int i=0; i<lstMatches.size(); ++i)
        emitMatch(lstMatches[i], matPoints);
      endMany();
    }
  else
    {
      if (lstMatches.size() > 0)
        emitMatch(lstMatches[0], matPoints);
      else
        {
          classificationOutput()->emitObject(NAN);
          d->pModelIndexOutput->emitObject(-1);
          d->pLocationOutput->emitObject(d->matEmptyLocation);
          d->pTransformOutput->emitObject(d->matIdentity);
          d->pQueryPointsOutput->emitObject(d->matEmptyPoint);
          d->pModelPointsOutput->emitObject(d->matEmptyPoint);
        }
    }
  return NAN;
}

void PiiPointMatchingOperation::removeDuplicates(PiiMatching::MatchList&)
{}

void PiiPointMatchingOperation::emitMatch(const PiiMatching::Match& match, const PiiMatrix<float>& queryPoints)
{
  PII_D;
  int iModelIndex = match.modelIndex();

  /*qDebug("PiiPointMatchingOperation::match()\n"
         "Match { index: %d, label: %lf }",
         iModelIndex,
         iModelIndex < d->lstLabels.size() ?
         d->lstLabels[iModelIndex] : iModelIndex);*/

  classificationOutput()->emitObject(iModelIndex < d->lstLabels.size() ?
                                     d->lstLabels[iModelIndex] : iModelIndex);
  d->pModelIndexOutput->emitObject(iModelIndex);
  d->pLocationOutput->emitObject(PiiMatrix<float>(1, d->iPointDimensions*2, d->matLocations[iModelIndex]));
  d->pTransformOutput->emitObject(toTransformMatrix(match.transformParams()));

  if (d->bMustSendPoints)
    {
      QList<QPair<int,int> > lstPairs = match.matchedPoints();
      PiiMatrix<float> matQueryPoints(0, d->iPointDimensions);
      matQueryPoints.reserve(lstPairs.size());
      PiiMatrix<float> matModelPoints(matQueryPoints);
      for (int i=0; i<lstPairs.size(); ++i)
        {
          matQueryPoints.appendRow(queryPoints[lstPairs[i].first]);
          matModelPoints.appendRow(d->pMatcher->modelPoints()[lstPairs[i].second]);
        }
      d->pQueryPointsOutput->emitObject(matQueryPoints);
      d->pModelPointsOutput->emitObject(matModelPoints);
    }
}

void PiiPointMatchingOperation::collectSample(double label, double /*weight*/)
{
  PII_D;

  PiiMatrix<float> matFeatures = PiiYdin::convertMatrixTo<float>(featureInput());
  PiiMatrix<float> matPoints = PiiYdin::convertMatrixTo<float>(d->pPointsInput);

  const int iPoints = checkDescriptor(matPoints, matFeatures);

  if (learningBatchSize() > 0 && d->iModelCount >= learningBatchSize())
    {
      int iRemovedIndex = 0;
      switch (fullBufferBehavior())
        {
        case PiiClassification::OverwriteRandomSample:
          iRemovedIndex = rand() % d->iModelCount;
          break;
        case PiiClassification::OverwriteOldestSample:
          break;
        case PiiClassification::DiscardNewSample:
        default:
          return;
        }
      // Find the first occurrence of the model to be removed.
      for (int i=0; i<d->vecNewModelIndices.size(); ++i)
        {
          if (d->vecNewModelIndices[i] == iRemovedIndex)
            {
              // Find the last occurrence
              int iFirstIndex = i;
              for (++i; i<d->vecNewModelIndices.size() && d->vecNewModelIndices[i] == iRemovedIndex; ++i) {}
              int iPointsInModel = i - iFirstIndex;
              // We deleted one model -> must decrease larger model indices by one
              for (; i<d->vecNewModelIndices.size(); ++i)
                --d->vecNewModelIndices[i];
              // Delete everything in between
              d->vecNewModelIndices.remove(iFirstIndex, iPointsInModel);
              d->matNewFeatures.removeRows(iFirstIndex, iPointsInModel);
              d->matNewPoints.removeRows(iFirstIndex, iPointsInModel);
              if (d->lstNewLabels.size() < iRemovedIndex)
                d->lstNewLabels.removeAt(iRemovedIndex);
              d->matNewLocations.removeRow(iRemovedIndex);
              --d->iModelCount;
              break;
            }
        }
    }

  // PENDING Should we reserve some room to further avoid
  // reallocation?

  d->matNewFeatures.appendRows(matFeatures);
  d->matNewPoints.appendRows(matPoints);
  d->vecNewModelIndices.reserve(d->vecNewModelIndices.size() + iPoints);
  for (int i=0; i<iPoints; ++i)
    d->vecNewModelIndices << d->iModelCount;
  if (labelInput()->isConnected())
    d->lstNewLabels << label;

  // If location input is connected, read and store object
  // location from there.
  if (d->pLocationInput->isConnected())
    {
      PiiMatrix<float> matLocation = PiiYdin::convertMatrixTo<float>(d->pLocationInput);
      if (matLocation.rows() != 1 || matLocation.columns() != d->iPointDimensions * 2)
        PII_THROW_WRONG_SIZE(d->pLocationInput, matLocation, 1, d->iPointDimensions * 2);
      d->matNewLocations.appendRow(matLocation);
      //Pii::transformRows(d->matPoints(0,-iPoints,-1,-1), matLocation[0], std::minus<int>());
    }
  // Otherwise find bounding box automatically.
  else
    {
      if (d->matNewLocations.isEmpty())
        d->matNewLocations.resize(0, d->iPointDimensions*2);
      appendBoundingBox(d->matNewLocations, matPoints);
    }
  ++d->iModelCount;
}

// Finds the bounding box of points and appends it as a new row to
// target.
void PiiPointMatchingOperation::appendBoundingBox(PiiMatrix<float>& target,
                                                  const PiiMatrix<float>& points) const
{
  const PII_D;
  PiiMatrix<float> matMin(Pii::min(points, Pii::Vertically));
  PiiMatrix<float> matMax(Pii::max(points, Pii::Vertically));
  float* pCube = target.appendRow();
  for (int i=0; i<d->iPointDimensions; ++i)
    pCube[i+d->iPointDimensions] = matMax(0,i) - (pCube[i] = matMin(0,i));
}

bool PiiPointMatchingOperation::startLearningThread(const PiiVariant& features,
                                                    const PiiVariant& points,
                                                    const QVariantList& modelIndices,
                                                    const QVariantList& labels,
                                                    const PiiVariant& locations)
{
  PII_D;
  if (features.type() != PiiYdin::FloatMatrixType ||
      points.type() != PiiYdin::FloatMatrixType ||
      (locations.isValid() && locations.type() != PiiYdin::FloatMatrixType))
    {
      piiWarning(tr("Features, points and locations must all be given as float matrices."));
      return false;
    }
  const PiiMatrix<float> &matFeatures = features.valueAs<PiiMatrix<float> >(),
    &matPoints = points.valueAs<PiiMatrix<float> >();
  QVector<int> vecModelIndices = Pii::variantsToVector<int>(modelIndices);

  if (matFeatures.rows() != matPoints.rows() ||
      matFeatures.rows() != vecModelIndices.size())
    {
      piiWarning(tr("There must be an equal number of features (%1), points (%2), and model indices (%3).")
                 .arg(matFeatures.rows()).arg(matPoints.rows()).arg(vecModelIndices.size()));
      return false;
    }

  int iModelCount = Pii::maxIn(vecModelIndices.constBegin(), vecModelIndices.constEnd()) + 1;

  QList<double> lstLabels;
  if (labels.size() > 0)
    {
      if (iModelCount != labels.size())
        {
          piiWarning(tr("If labels are given, there must be one for each model."));
          return false;
        }
      lstLabels = Pii::variantsToList<double>(labels);
    }

  PiiMatrix<float> matLocations(0, d->iPointDimensions*2);
  if (locations.isValid())
    {
      matLocations = locations.valueAs<PiiMatrix<float> >();
      if (iModelCount != matLocations.rows())
        {
          piiWarning(tr("If locations are given, there must be one for each model."));
          return false;
        }
      if (matLocations.columns() != d->iPointDimensions*2)
        {
          piiWarning(tr("Locations must be given as %1-dimensional hypercubes (%2 values).")
                     .arg(d->iPointDimensions).arg(2*d->iPointDimensions));
          return false;
        }
    }
  else
    {
      // If locations are not explicitly provided, automatically find
      // bounding boxes.
      matLocations.reserve(iModelCount);
      int iStartIndex = 0, iCurrentIndex = 1;
      while (iCurrentIndex < vecModelIndices.size())
        {
          if (vecModelIndices[iCurrentIndex] != vecModelIndices[iCurrentIndex-1])
            {
              appendBoundingBox(matLocations, matPoints(iStartIndex, 0, iCurrentIndex-iStartIndex, -1));
              iStartIndex = iCurrentIndex;
            }
          ++iCurrentIndex;
        }
      appendBoundingBox(matLocations, matPoints(iStartIndex, 0, iCurrentIndex-iStartIndex, -1));
    }

  d->matNewFeatures = matFeatures;
  d->matNewPoints = matPoints;
  d->matNewLocations = matLocations;
  d->vecNewModelIndices = vecModelIndices;
  d->lstNewLabels = lstLabels;
  d->iModelCount = iModelCount;

  return PiiClassifierOperation::startLearningThread();
}

PiiPointMatchingOperation* PiiPointMatchingOperation::clone() const
{
  const PII_D;
  PiiPointMatchingOperation* pNewOperation = static_cast<PiiPointMatchingOperation*>(PiiClassifierOperation::clone());
  Data* pNewData = pNewOperation->_d();
  delete pNewData->pMatcher;
  pNewData->pMatcher = d->pMatcher != 0 ? new Matcher(*d->pMatcher) : 0;
  pNewData->matLocations = d->matLocations;
  pNewData->matNewPoints = d->matNewPoints;
  pNewData->matNewLocations = d->matNewLocations;
  pNewData->matNewFeatures = d->matNewFeatures;
  pNewData->lstLabels = d->lstLabels;
  pNewData->lstNewLabels = d->lstNewLabels;
  pNewData->vecNewModelIndices = d->vecNewModelIndices;
  pNewData->iModelCount = d->iModelCount;
  pNewData->matchingMode = d->matchingMode;
  pNewData->iClosestMatchCount = d->iClosestMatchCount;

  return pNewOperation;
}

void PiiPointMatchingOperation::setMatchingMode(PiiMatching::ModelMatchingMode matchingMode) { _d()->matchingMode = matchingMode; }
PiiMatching::ModelMatchingMode PiiPointMatchingOperation::matchingMode() const { return _d()->matchingMode; }

void PiiPointMatchingOperation::setClosestMatchCount(int closestMatchCount)
{
  PII_D;
  d->pMatcher->setClosestMatchCount(d->iClosestMatchCount = closestMatchCount);
}

int PiiPointMatchingOperation::closestMatchCount() const
{
  return _d()->iClosestMatchCount;
}
