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

#ifndef _PIIKDTREE_H
#define _PIIKDTREE_H

#include <QLinkedList>
#include <QPair>
#include <PiiProgressController.h>
#include "PiiSampleSet.h"
#include "PiiClassification.h"
#include "PiiSquaredGeometricDistance.h"
#include <PiiSerialization.h>
#include <PiiNameValuePair.h>
#include <PiiSharedD.h>

/**
 * K-dimensional tree. The kd-tree is a binary tree in which every
 * node is a k-dimensional point. Each non-leaf node in the tree
 * splits the k-dimensional hyperspace with a hyperplane that is
 * aligned to one of the axes and passes through the point in the
 * node.
 *
 * The kd-tree can be used to quickly look up nearest neighbors in
 * large databases. For randomly distributed data, the complexity of
 * the algorithm is `O(log N)`, which is much better than
 * the `O(N)` of exhaustive search. The advantage however
 * quickly diminishes with growing feature space dimensionality. As a
 * general rule, *exact* NN search using the kd-tree is advantageous
 * if and only if \(N >> 2^k\), where N is the number of samples and
 * k is the feature space dimensionality.
 *
 * PiiKdTree includes a variant of the basic NN look-up algorithm
 * that performs approximate NN search. (k-NN search is also
 * supported.) Instead of recursively checking all possible branches
 * of the tree the approximate algorithm orders the look-ups so that
 * the most likely ones come first. The algorithm stops when the exact
 * nearest neighbor has been found or a predefined maximum number of
 * look-ups have been performed. This makes it possible to set a hard
 * upper bound for the search time while still returning the nearest
 * neighbor(s) with a high probability.
 *
 * PiiKdTree only works with geometric distances. Thus, there is no
 * option to use user-defined distance measures. If you need a special
 * distance measure, exhaustive search is currently the only viable
 * option.
 *
 */
template <class SampleSet> class PiiKdTree
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int)
  {
    archive & PII_NVP("root", d->pRoot);
    archive & PII_NVP("features", d->iFeatureCount);
    archive & PII_NVP("models", d->modelSet);
  }
  
public:
  typedef typename PiiSampleSet::Traits<SampleSet>::ConstFeatureIterator Sample;

  /**
   * Constructs an empty kd-tree.
   */
  PiiKdTree();
  /**
   * Constructs a deep copy of another kd-tree.
   */
  PiiKdTree(const PiiKdTree& other);
  
  /**
   * Constructs a kd-tree out of the given model samples.
   */
  PiiKdTree(const SampleSet& modelSet);
  /**
   * Destroys the kd-tree.
   */
  ~PiiKdTree();

  /**
   * Assigns *other* to `this`.
   */
  PiiKdTree& operator= (const PiiKdTree& other);
  
  /**
   * Deletes the old kd-tree (if any) and rebuilds a new one based on
   * the given model samples.
   *
   * @param modelSet model samples
   *
   * @param controller an optional external controller that can be
   * used to stop building the tree on user request.
   *
   * @exception PiiClassificationException& if the algorithm was
   * interrupted.
   */
  void buildTree(const SampleSet& modelSet, PiiProgressController* controller = 0);
  
  /**
   * Returns the index of the nearest neighbor in the model set.
   *
   * @param sample input feature vector
   *
   * @param distance an optional output-value argument that will store
   * the *squared* geometric distance to the closest neighbor of 
   * *sample*.
   *
   * @return the index of the closest sample in the model set, or -1
   * if the set is empty.
   */
  int findClosestMatch(Sample sample,
                       double* distance = 0) const;
  
  /**
   * Returns the index of a probably nearest neighbor in the model
   * set. This version of the look-up algorithm may not return the
   * exact nearest neighbor, but it does so with a high probability. 
   * Approximate NN search is useful in high-dimensional spaces where
   * the exact algorithm may be slower than exhaustive search.
   *
   * @param sample input feature vector
   *
   * @param maxEvaluations the maximum number of node look-ups to be
   * done. If you set this value to `log`(N), the algorithm will do a
   * simple best first search to the first leaf node. Usually, it is a
   * good idea to give the algorithm a bit more time to find a good
   * match. If you set this value to the size of the model set, the
   * exact nearest neighbor will be returned.
   *
   * @param distance an optional output-value argument that will store
   * the *squared* geometric distance to the closest neighbor of 
   * *sample*.
   *
   * @return the index of the closest sample in the model set, or -1
   * if the set is empty.
   */
  int findClosestMatch(Sample sample,
                       int maxEvaluations,
                       double* distance = 0) const;

  /**
   * Returns the *n* closest matches of *sample*. This function is
   * equivalent to PiiClassification::findClosestMatches(). It returns
   * an exact answer and may not perform well in high-dimensional
   * spaces.
   *
   * @return the *n* closest matches. Note that if the model data set
   * is smaller than *n*, less than *n* matches may be returned.
   */
  PiiClassification::MatchList findClosestMatches(Sample sample,
                                                  int n) const;
  
  /**
   * Returns *n* matches that are probably the closest of *sample*. 
   * This function stops after *maxEvaluations* most probable nodes
   * have been checked and may not return the exact nearest neighbors.
   *
   * @param sample input feature vector
   *
   * @param n the number of closest matches to return.
   *
   * @param maxEvaluations the maximum number of node look-ups to be
   * done. A suitable value is about *n* * `log`(N), where N is the
   * number of samples in the model set.
   *
   * @return the *n* closest matches. Note that if either the model
   * data set or *maxEvaluations* is smaller than *n*, less than 
   * *n* matches may be returned.
   */
  PiiClassification::MatchList findClosestMatches(Sample sample,
                                                  int n,
                                                  int maxEvaluations) const;
  /**
   * Returns the model sample set that was used to construct the
   * kd-tree.
   */
  SampleSet modelSet() const { return d->modelSet; }

  /**
   * Prints the structure of the k-d tree to *stream*. This function
   * is mainly for informational and debugging purposes.
   */
  template <class Stream> void print(Stream& stream) { d->pRoot->print(stream); }

private:
  typedef typename PiiSampleSet::Traits<SampleSet>::FeatureType T;

  struct Node
  {
    template <class Archive> void serialize(Archive& archive, const unsigned int)
    {
      archive & PII_NVP("index", sampleIndex);
      archive & PII_NVP("dim", splitDimension);
      archive & PII_NVP("value", featureValue);
      archive & PII_NVP("smaller", smaller);
      archive & PII_NVP("larger", larger);
    }
    
    Node(int index=0, int dim = 0, T value = 0, Node* s = 0, Node* l = 0) :
      sampleIndex(index), splitDimension(dim), featureValue(value), smaller(s), larger(l)
    {}

    Node(const Node& other) :
      sampleIndex(other.sampleIndex),
      splitDimension(other.splitDimension),
      featureValue(other.featureValue),
      smaller(other.smaller != 0 ? new Node(*other.smaller) : 0),
      larger(other.larger != 0 ? new Node(*other.larger) : 0)
    {}
    
    ~Node()
    {
      delete larger;
      delete smaller;
    }
    
    int sampleIndex, splitDimension;
    T featureValue;
    Node* smaller;
    Node* larger;

    template <class Stream> void print(Stream& stream, int level = 0) const
    {
      for (int i=level; i--; )
        stream << "  ";
      stream << "model[" << sampleIndex << "][" << splitDimension << "] = " << featureValue << "\n";
      if (smaller)
        smaller->print(stream, level+1);
      if (larger)
        larger->print(stream, level+1);
    }
  };

  // Stores feature value and the index of the sample it belongs to.
  typedef QPair<T,int> FeatureSorter;
  // Stores a pointer to a node and the distance to it.
  typedef QPair<double,Node*> BranchSorter;
  typedef QLinkedList<BranchSorter> BranchList;

  class Data : public PiiSharedD<Data>
  {
  public:
    Data() : pRoot(0), iFeatureCount(0), pMeans(0), pVars(0) {}
    Data(const Data& other) :
      pRoot(new Node(*other.pRoot)),
      iFeatureCount(other.iFeatureCount),
      pMeans(0), pVars(0),
      modelSet(other.modelSet)
    {}

    Node* pRoot;
    int iFeatureCount;
    double* pMeans, *pVars;
    SampleSet modelSet;
    PiiSquaredGeometricDistance<Sample> measure;
  } *d;

  // Tree construction
  Node* createNode(FeatureSorter* sorterArray,
                   int sampleCount,
                   int depth,
                   PiiProgressController* controller);
  int selectDimension(FeatureSorter* sorterArray,
                      int sampleCount,
                      int depth);

  // Exact (k-)NN search
  template <class MatchList>
  void findClosestMatches(Node* node,
                          Sample sample,
                          MatchList& matches) const;
  // Approximate (k-)NN search
  template <class MatchList>
  void findClosestMatches(Sample sample,
                          int maxEvaluations,
                          MatchList& matches) const;
  template <class MatchList>
  void findPossibleBranches(Node* node,
                            Sample sample,
                            int* maxEvaluations,
                            BranchList& branchList,
                            MatchList& matches) const;

  // Match list helper functions. In NN search "list" is actually a pair.
  static inline void updateLimit(double distance, int index, QPair<double,int>& pair)
  {
    pair.first = distance;
    pair.second = index;
  } 
  static inline void updateLimit(double distance, int index, PiiClassification::MatchList& matches)
  {
    matches.put(qMakePair(distance, index));
  }
 
  static inline double distanceLimit(const QPair<double,int>& pair) { return pair.first; }
  static inline double distanceLimit(const PiiClassification::MatchList& matches) { return matches[0].first; }
  
  inline int sampleCount() const { return PiiSampleSet::sampleCount(d->modelSet); }
  inline Sample sampleAt(int index) const { return PiiSampleSet::sampleAt(const_cast<const SampleSet&>(d->modelSet), index); }
};

#include "PiiKdTree-templates.h"

#endif //_PIIKDTREE_H
