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

#ifndef _PIICLASSIFICATIONPLUGIN_H
#define _PIICLASSIFICATIONPLUGIN_H

/**
 * @defgroup PiiClassificationPlugin
 *
 * @brief Pattern classification and other machine learning
 * algorithms.
 *
 * The classification plug-ins contains implementations of many
 * learning and classification techniques. This chapter describes the
 * key concepts common to many of them.
 *
 * Samples and features
 * --------------------
 * @anchor classification_samples_and_features
 *
 * *Samples* are abstract entities represented by N > 0 *features*. 
 * Typically, features are represented as N-dimensional real-valued
 * vectors. A feature can however be a text string, an object boundary
 * represented as a list of coordinates, a graph, or a composition of
 * all of these. Independent of the type of the features, each sample
 * represents a vector in an N-dimensional *input space*. In
 * documentation, a sample is typically denoted by **x**.
 *
 * In Into, features are represented with random-access iterators. 
 * That is, the feature vector type must have an iterator that is
 * indexable using operator[](int). Index 0 is the first feature and
 * index N-1 the last one. Valid feature vector types include 
 * `double`*, std::vector<int>::iterator, and QVariantList::iterator.
 *
 * Sample sets
 * -----------
 * @anchor classification_sample_sets
 *
 * To actually use samples with learning machines, one needs more than
 * one of them. In Into, a *sample set* is a randomly
 * accessible collection of samples with functions for querying the
 * size of the set and the number of features. Each sample in a sample
 * set must have an equal number of features. Formally, @f$\mathbf{X}
 * = \{\mathbf{x}_i\}_{i=1}^M@f$, where M is the number of samples in
 * the set.
 *
 * To be able to use a data structure as a sample set Into needs to be
 * able to query and modify it in various ways. The required
 * operations are defined in PiiSampleSet that wraps the actual data
 * type used to store the samples. The default implementation works
 * with Qt container types (QList, QVector) and a specialization is
 * provided for PiiMatrix. If other types are used, the structure must
 * be specialized correspondingly.
 *
 *
 * Labels
 * ------
 * @anchor classification_labels
 *
 * A *label* indicates the class to which a sample belongs. In the
 * literature, a class label is typically represented by an integer
 * denoting the index of a class within a discrete set of classes. 
 * Into uses QVector<double> as the container for class labels. This
 * allows one to use the same label type to both classification and
 * regression ("continuous classification") tasks. Whenever a class
 * index is needed instead of a continuous output value, the `double`
 * is simply casted to an `int`. An unknown value is denoted by 
 * `NaN`.
 *
 * With most algorithms, the size of the label set must match that of
 * the corresponding sample set. That is, each sample must have an
 * associated label. In documentation, labels are usually denoted by
 * *c* (for `class`). Sometimes, a sample set is defined as a set of
 * (feature vector, label) pairs. For example, a set of samples with
 * binary classifications can be formally defined as @f$\mathbf{S} =
 * \{ (\mathbf{x}_i, c_i)|\mathbf{x}_i \in \mathbb{R}^N, c_i \in
 * \{0,1\}\}_{i=1}^M@f$. In code, however, sample and label sets are
 * treated as distinct entities.
 *
 *
 * Distance measures
 * -----------------
 * @anchor classification_distance_measures
 *
 * As the name implies, distance measures are used to measure the
 * dissimilarity or distance between two samples. A distance measure
 * is a function that maps two feature vectors into a real number:
 * \(d = f(\mathbf{x}_1, \mathbf{x}_2)\). The definition of a
 * distance is quite relaxed: it is sufficient that the function
 * returns a larger value as the diversity between feature vectors
 * grows. The distance can be negative.
 *
 * In code, distance measures are function objects that take three
 * arguments: the feature vector of a sample, that of a model, and the
 * number of features to consider. The following two declarations are
 * valid distance measures:
 *
 * ~~~(c++)
 * double myDistance(double* sample, double* model, int len);
 *
 * struct MyDistance
 * {
 *   double operator() (QVector<double>::const_iterator sample,
 *                      QVector<double>::const_iterator& model,
 *                      int len) const;
 * };
 * ~~~
 *
 * Distance measures are used by algorithms such as 
 * [NN](PiiClassification::findClosestMatch()), @ref
 * PiiClassification::knnClassify() "k-NN" and [SOM](PiiSom) to
 * measure the dissimilarity between code vectors. PiiDistanceMeasure
 * is a polymorphic implementation of the concept and used when
 * run-time changes to distance measures are needed.
 *
 * Kernels
 * -------
 * @anchor classification_kernels
 *
 * Kernels are relatives to distance measures in that they share the
 * same interface. Their meaning in mathematical sense is however
 * quite different.
 *
 * The *kernel trick* is a method of converting a hyperplane
 * (linear) classifier into a non-linear one. A kernel funtion is used
 * in converting a linear *input space* non-linearly into a
 * high-dimensional *feature space*, in which a linear
 * classifier can find a solution. This is done using Mercer's
 * theorem, which states (approximately) that any continuous,
 * symmetric, positive semi-definite function \(k(x,y)\) can be
 * expressed as a dot product in a high-dimensional space. It follows
 * that @f$k(\mathbf{x}_i, \mathbf{x}_j) = \Phi(\mathbf{x}_i)\cdot
 * \Phi(\mathbf{x}_j)\(, where \)\Phi(x)@f$ is the non-linear
 * mapping function.
 *
 * An interesting thing about kernels is that one does not need to
 * actually know the mapping function or even the dimensionality of
 * the feature space; they are implicitly defined by the
 * kernel. Practically, replacing dot products in a linear algorithm
 * with a kernel function results in a non-linear variation of the
 * algorithm. To stay linear, one can always use PiiLinearKernel.
 *
 * Many linear classifiers use a bias term to move the hyperplane off
 * the coordinate system's origin. In into, the bias term is blatantly
 * ignored with kernel methods. The penalty? Practically none. While
 * it is required for the low-dimensional case, the practical effect
 * of the missing bias in a high-dimensional space is to decrease the
 * degree of freedom by one. With kernels such as the 
 * [Gaussian kernel](PiiGaussianKernel) the bias term would have no
 * effect anyway. The upside is that neither feature vectors nor
 * kernel functions need to take the possible existence of an extra
 * term into account.
 */

#endif //_PIICLASSIFICATIONPLUGIN_H
