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

#ifndef _PIIFEATUREPOINTMATCHER_H
#define _PIIFEATUREPOINTMATCHER_H

#include <PiiSampleSet.h>
#include <PiiMatrix.h>
#include <PiiDistanceMeasure.h>
#include <PiiKdTree.h>
#include <PiiClassification.h>
#include <PiiSharedD.h>

#include <QList>
#include <QVector>
#include <QPair>
#include <QHash>

#include <limits>

#include "PiiMatching.h"

class PiiProgressController;

namespace PiiMatching
{
  /**
   * A class that stores information about a matched model in
   * PiiFeaturePointMatcher. Each match contains the index of the
   * model that was matched, the indices of matched points, and the
   * parameters of the mathematical model that transforms the matched
   * points to the model coordinate system.
   */
  class Match
  {
  public:
    Match();
    Match(int modelIndex,
          const PiiMatrix<double> transformParams,
          const QList<QPair<int,int> >& matchedPoints);
    Match(const Match& other);
    ~Match();

    Match& operator= (const Match& other);

    /**
     * Returns the index of the matched model in the model database.
     */
    int modelIndex() const;
    /**
     * Returns the parameters of the mathematical model that describes
     * the transformation from model coordinates to input space
     * coordinates. The parameters are represented as a 1-by-N matrix
     * where the actual number of parameters (N) depends on the type
     * of the matcher.
     */
    PiiMatrix<double> transformParams() const;
    /**
     * Returns the indices of matched point pairs. Each element in the
     * returned list is a pair (query point index, matched database
     * point index).
     */
    QList<QPair<int,int> > matchedPoints() const;
    /**
     * Returns the total number of matched points.
     */
    int matchedPointCount() const;

  private:
    class Data
    {
    public:
      Data();
      Data(int modelIndex,
           const PiiMatrix<double> transformParams,
           const QList<QPair<int,int> >& matchedPoints);

      PiiAtomicInt iRefCount;
      int iModelIndex;
      PiiMatrix<double> matTransformParams;
      QList<QPair<int,int> > lstMatchedPoints;
    } *d;
  };

  typedef QList<Match> MatchList;

  /**
   * Removes duplicate entries in a list of matched models. Duplicate
   * entries are those with the same model index and sufficiently
   * similar transformation parameters. "Sufficiently similar" depends
   * on the mathematical transformation model.
   *
   * @param matchedModels a list of matched models. Will be modified.
   *
   * @param merge a binary function object that merges two matches,
   * if possible. The function takes two matches as parameters and
   * checks if they are close enough to be merged. If the matches can
   * be merged, the merged match is stored to *match2* and the
   * function returns `true`. Otherwise, it returns `false`.
   *
   * ~~~(c++)
   * struct MyMerger
   * {
   *   bool operator() (const PiiMatching::Match& match1,
   *                    PiiMatching::Match& match2) const;
   * };
   * ~~~
   */
  template <class Merger> void removeDuplicates(MatchList& matchedModels, Merger& merge);
}

/**
 * A generic algorithm that can be used to match objects to a model
 * database using feature points. As input, a set of N-dimensional
 * points are provided with a feature vector for each. Each point is
 * also associated to one model. Queries are given in the form of a
 * point set and a feature vector for each. The matching algorithm
 * tries to find the models in the database that match the query.
 *
 * **Template parameters**
 *
 * - `T` - the type used to store point coordinates. In images, pixel
 * coordinate type is usually `int`.
 * - `SampleSet` - the type used to store the features related to the
 * points, for example PiiMatrix<float>.
 *
 * ~~~(c++)
 * // 2D pixel coordinates.
 * // Let's assume we have two models with 10 points each.
 * PiiMatrix<int> matModelPoints(20,2);
 * // The corresponding feature vectors. Same number of rows.
 * PiiMatrix<float> matModelFeatures(20,16); // 16 features per point
 * // Here, we should really store point locations and the corresponding
 * // feature vectors. There are many techniques for this, for example
 * // PiiMatching::shapeContextDescriptor().
 * // First 10 points belong to model 0, the rest are from model 1.
 * QVector<int> vecModelIndices;
 * for (int i=0; i<20; ++i)
 *   vecModelIndices << i/10;
 *
 * PiiFeaturePointMatcher<int, PiiMatrix<float> > pointMatcher.
 * pointMatcher.buildDatabase(matModelPoints,
 *                            matModelFeatures,
 *                            vecModelIndices);
 *
 * // This matcher assumes rigid objects that can only undergo scaling
 * // and rotation on a plane
 * PiiRigidPlaneRansac<int> ransac;
 * ransac.setAutoRefine(true);
 * ransac.setFittingThreshold(10);
 * ransac.setMinScale(0.5);
 * ransac.setMaxScale(3);
 *
 * // 2D pixel coordinates in an unknown query object.
 * PiiMatrix<int> matQueryPoints(8,2);
 * // The corresponding feature vectors. Same number of rows.
 * PiiMatrix<float> matQueryFeatures(8,16);
 *
 * PiiMatching::MatchList matches =
 *   pointMatcher.findMatchingModels(matQueryPoints,
 *                                   matQueryFeatures,
 *                                   ransac);
 * ~~~
 *
 */
template <class T, class SampleSet> class PiiFeaturePointMatcher
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int /*version*/)
  {
    archive & PII_NVP("points", d->matModelPoints);
    archive & PII_NVP("kdTree", d->pKdTree);
    archive & PII_NVP("features", d->modelFeatures);
    archive & PII_NVP("indices", d->vecModelIndices);
    //archive & PII_NVP("distanceMeasure", d->pDistanceMeasure);
    archive & PII_NVP("mode", PII_ENUM(d->matchingMode));
    archive & PII_NVP("closestMatches", d->iClosestMatchCount);
    archive & PII_NVP("maxEvaluations", d->iMaxEvaluations);
  }
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator ConstFeatureIterator;

  PiiFeaturePointMatcher();
  PiiFeaturePointMatcher(const PiiFeaturePointMatcher& other);

  ~PiiFeaturePointMatcher();

  PiiFeaturePointMatcher& operator= (const PiiFeaturePointMatcher& other);

  /**
   * Builds the model database. This function either stores the
   * *features* for linear search or builds a [K-d tree](PiiKdtree),
   * which will be later used for quick queries. The most suitable
   * search technique is determined by the number of points and
   * features.
   *
   * @param points the locations of feature points with respect to the
   * model the point belongs to.
   *
   * @param features feature vectors corresponding to the feature
   * points. The number of features must match the number of points.
   *
   * @param modelIndices a model index for each point. Each model may
   * be represented by a different number of points, and this list
   * describes the relationships. It is not necessary that points
   * belonging to a model are stored consequently. If no model indices
   * are given, the database is assumed to consist of a single model
   * object, for which zero will be assigned as the model index.
   *
   * @param controller an optional progress controller that can be
   * used to get information about the tree building process, and to
   * cancel the process if needed.
   *
   * @param measure an optional distance measure that can be used if
   * the feature space is non-Euclidean. Note that the K-d tree will
   * not be used for queries if a custom distance measure is
   * provided. PiiFeaturePointMatcher takes the ownership of the
   * measure.
   *
   * @exception PiiClassificationException& if the tree building
   * process was interrupted or if there is a non-equal number of
   * points, features and model indices.
   */
  void buildDatabase(const PiiMatrix<T>& points,
                     const SampleSet& features,
                     const QVector<int>& modelIndices = QVector<int>(),
                     PiiProgressController* controller = 0,
                     PiiDistanceMeasure<ConstFeatureIterator>* measure = 0);

  /**
   * Matches a set of *points* with their corresponding feature
   * vectors in *features* to the database of models using *matcher*
   * to find the transformation between the query and the model.
   *
   * The matching algorithm:
   *
   * - Find the M closest matches of each key point (*points*) in the
   * key point database. Matches whose distance ratio to the closest
   * one is less than 0.8 will be discarded.
   *
   * - Select candidate models with a sufficient number of matches for
   * further inspection.
   *
   * - Repeat until the candidate model set is empty:
   *
   *   - Repeat for each remaining candidate model:
   *
   *     - Select the points in the query set that had the current
   *     candidate model in their list of closest matches.
   *
   *     - Run a matching algorithm such as RANSAC to see if this
   *     subset of points can be matched with the current model
   *     candidate.
   *
   *     - If a sufficiently good match is found:
   *
   *       - Store the index of the candidate model, and the
   *       transformation that relates the model to the measured point
   *       set.
   *
   *       - If matching mode is MatchAllModels, remove the matched
   *       (inlying) points from the point set.
   *
   *       - Otherwise remove the candidate model from the candidate
   *       model set.
   *
   *     - Otherwise remove the candidate model from the candidate
   *     model set.
   *
   * The matching algorithm can detect instances of multiple models in
   * a single query, as well as multiple instances of each of them. As
   * a result, each matched model will be returned.
   *
   * @param points the coordinates of the points to be matched (N x M)
   *
   * @param features the corresponding feature vectors (N x O)
   *
   * @param matcher the matching algorithm. Must provide
   * findBestModel(const PiiMatrix<T>&, const PiiMatrix<T>&),
   * inlyingPoints(), and bestModel() functions with signatures equal
   * to those found in PiiRigidPlaneRansac.
   */
  template <class Matcher>
  PiiMatching::MatchList findMatchingModels(const PiiMatrix<T>& points,
                                            const SampleSet& features,
                                            Matcher& matcher) const;

  /**
   * Sets the matching mode. If the matching mode is set to
   * `MatchOneModel`, the search for matching models will be finished
   * immediately after the best match (if any) has been found.
   */
  void setMatchingMode(PiiMatching::ModelMatchingMode matchingMode)
  {
    if (matchingMode != d->matchingMode)
      {
        detach();
        d->matchingMode = matchingMode;
      }
  }
  /**
   * Returns the matching mode. The default is `MatchAllModels`.
   */
  PiiMatching::ModelMatchingMode matchingMode() const { return d->matchingMode; }

  /**
   * Sets the number of closest matches considered for each query
   * point. If this value is set to one, only the closest match in the
   * database will be considered. A higher value means higher matching
   * accuracy but also longer query times.
   */
  void setClosestMatchCount(int closestMatchCount)
  {
    if (closestMatchCount != d->iClosestMatchCount)
      {
        detach();
        d->iClosestMatchCount = qMax(1,closestMatchCount);
      }
  }
  /**
   * Returns the number of closest matches found for each query point.
   * The default is 3.
   */
  int closestMatchCount() const { return d->iClosestMatchCount; }

  /**
   * Sets the maximum number of evaluations when searching a k-d tree.
   * This makes it possible to return correct matches for the majority
   * of feature points while making the search much faster. Setting
   * *maxEvaluations* value to a non-positive value disables the
   * approximate nearest neighbor search optimization.
   */
  void setMaxEvaluations(int maxEvaluations)
  {
    if (maxEvaluations != d->iMaxEvaluations)
      {
        detach();
        d->iMaxEvaluations = maxEvaluations;
      }
  }
  /**
   * Returns the maximum number of evaluations. The default is 0.
   */
  int maxEvaluations() const { return d->iMaxEvaluations; }

  /**
   * Returns the stored model points.
   */
  const PiiMatrix<T>& modelPoints() const { return d->matModelPoints; }
  /**
   * Returns the stored model features.
   */
  const SampleSet& modelFeatures() const { return d->matModelFeatures; }
  /**
   * Returns the stored model indices.
   */
  const QVector<int>& modelIndices() const { return d->vecModelIndices; }

private:
  class Data : public PiiSharedD<Data>
  {
  public:
    Data() :
      pKdTree(0),
      pDistanceMeasure(0),
      matchingMode(PiiMatching::MatchAllModels),
      iClosestMatchCount(1),
      iMaxEvaluations(0)
    {}
    Data(const Data& other) :
      matModelPoints(other.matModelPoints),
      pKdTree(other.pKdTree ? new PiiKdTree<SampleSet>(*other.pKdTree) : 0),
      modelFeatures(other.modelFeatures),
      vecModelIndices(other.vecModelIndices),
      pDistanceMeasure(other.pDistanceMeasure ? other.pDistanceMeasure->clone() : 0),
      matchingMode(other.matchingMode),
      iClosestMatchCount(other.iClosestMatchCount),
      iMaxEvaluations(other.iMaxEvaluations)
    {}
    ~Data()
    {
      delete pKdTree;
      delete pDistanceMeasure;
    }

    Data* createCleanCopy()
    {
      Data* d = new Data;
      d->matchingMode = matchingMode;
      d->iClosestMatchCount = iClosestMatchCount;
      d->iMaxEvaluations = iMaxEvaluations;
      this->release();
      return d;
    }

    PiiMatrix<T> matModelPoints;
    PiiKdTree<SampleSet>* pKdTree;
    SampleSet modelFeatures;
    QVector<int> vecModelIndices;
    PiiDistanceMeasure<ConstFeatureIterator>* pDistanceMeasure;
    PiiMatching::ModelMatchingMode matchingMode;
    int iClosestMatchCount;
    int iMaxEvaluations;
    PiiSquaredGeometricDistance<ConstFeatureIterator> squaredGeometricDistance;
  } *d;

  void detach() { d = d->detach(); }

  void collectPoints(const QList<QPair<int,int> >& indices,
                     const PiiMatrix<T>& points,
                     PiiMatrix<T>& queryPoints,
                     PiiMatrix<T>& modelPoints) const;

  static void removePoints(const QVector<int>& indices,
                           QList<QPair<int,int> >& matches);

  static QList<QPair<int,int> > matchIndices(const QVector<int>& indices,
                                             const QList<QPair<int,int> >& matches);
};

#include "PiiFeaturePointMatcher-templates.h"

#endif //_PIIFEATUREPOINTMATCHER_H
