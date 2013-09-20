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

#include "PiiKnnClassifierOperation.h"
#include "PiiFeatureCombiner.h"
#include "PiiDistributionNormalizer.h"
#include "PiiClassIndexMapper.h"
#include "PiiClassInfoMapper.h"
#include "PiiSampleBalancer.h"
#include "PiiConfusionMatrixBuilder.h"
#include "PiiSampleRandomizer.h"
#include "PiiSomOperation.h"
#include "PiiBoostClassifierOperation.h"

// Distance measures
#include "PiiGeometricDistance.h"
#include "PiiSquaredGeometricDistance.h"
#include "PiiCosineDistance.h"
#include "PiiAbsDiffDistance.h"
#include "PiiLogLikelihood.h"
#include "PiiHistogramIntersection.h"
#include "PiiJeffreysDivergence.h"
#include "PiiMultiFeatureDistance.h"
#include "PiiHammingDistance.h"
#include "PiiChiSquaredDistance.h"

#include "PiiPlugin.h"

#define PII_REGISTER_CLASSIFIER_TEMPLATE(CLASS_NAME, PRIMITIVE) \
  PII_SERIALIZABLE_EXPORT(CLASS_NAME<PiiMatrix<PRIMITIVE> >); \
  static PiiClassInfoRegisterer PII_JOIN3(classInfoOf,CLASS_NAME,PRIMITIVE)(pluginName(), \
                                                                            PII_STRINGIZE(CLASS_NAME<PRIMITIVE>), \
                                                                            PiiYdin::resourceName<PiiClassifier<PiiMatrix<PRIMITIVE> > >())

#define PII_REGISTER_CLASSIFIERS(NAME) \
  PII_REGISTER_CLASSIFIER_TEMPLATE(NAME, int); \
  PII_REGISTER_CLASSIFIER_TEMPLATE(NAME, float); \
  PII_REGISTER_CLASSIFIER_TEMPLATE(NAME, double)

#define PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(CLASS_NAME, PRIMITIVE) \
  PII_DEFINE_SPECIALIZED_VIRTUAL_METAOBJECT_FUNCTION_NAMED(CLASS_NAME, PiiMatrix<PRIMITIVE>, PRIMITIVE); \
  PII_SERIALIZATION_NAME_CUSTOM(CLASS_NAME::Template<PiiMatrix<PRIMITIVE> >, PII_STRINGIZE(CLASS_NAME<PRIMITIVE>)); \
  PII_SERIALIZABLE_EXPORT(CLASS_NAME::Template<PiiMatrix<PRIMITIVE> >); \
  static PiiClassInfoRegisterer PII_JOIN3(classInfoOf,CLASS_NAME,PRIMITIVE)(pluginName(), \
                                                                            PII_STRINGIZE(CLASS_NAME<PRIMITIVE>), \
                                                                            PiiYdin::resourceName<PiiOperation>())
#define PII_REGISTER_CLASSIFIER_OPERATIONS(NAME) \
  PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(NAME, int); \
  PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(NAME, float); \
  PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(NAME, double)


PII_IMPLEMENT_PLUGIN(PiiClassificationPlugin);

PII_REGISTER_CLASSIFIER_OPERATIONS(PiiKnnClassifierOperation);

PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(PiiSomOperation, float);
PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(PiiSomOperation, double);

PII_REGISTER_CLASSIFIER_TEMPLATE(PiiBoostClassifier, float);
PII_REGISTER_CLASSIFIER_TEMPLATE(PiiBoostClassifier, double);
PII_REGISTER_CLASSIFIERS(PiiDecisionStump);
PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(PiiBoostClassifierOperation, float);
PII_REGISTER_CLASSIFIER_OPERATION_TEMPLATE(PiiBoostClassifierOperation, double);

PII_REGISTER_OPERATION(PiiFeatureCombiner);
PII_REGISTER_OPERATION(PiiDistributionNormalizer);
PII_REGISTER_OPERATION(PiiClassIndexMapper);
PII_REGISTER_OPERATION(PiiClassInfoMapper);
PII_REGISTER_OPERATION(PiiConfusionMatrixBuilder);
PII_REGISTER_OPERATION(PiiSampleRandomizer);
PII_REGISTER_OPERATION(PiiSampleBalancer);
PII_REGISTER_OPERATION(PiiVisualTrainer);
PII_REGISTER_OPERATION(PiiVisualSomOperation);
PII_REGISTER_OPERATION_VOIDCTOR(PiiVisualSomClassifier);


#define PII_REGISTER_DISTANCE_MEASURE(MEASURE, TYPE)  \
  PII_SERIALIZATION_NAME_CUSTOM(PiiDistanceMeasure<const TYPE*>::Impl<MEASURE<const TYPE*> >, PII_STRINGIZE(MEASURE<TYPE>)); \
  PII_INSTANTIATE_FACTORY(PiiDistanceMeasure<const TYPE*>::Impl<MEASURE<const TYPE*> >); \
  static PiiClassInfoRegisterer PII_JOIN3(classInfoOf,MEASURE,TYPE)(pluginName(), \
                                                                    PII_STRINGIZE(MEASURE<TYPE>), \
                                                                    PiiYdin::resourceName<PiiDistanceMeasure<const TYPE*> >())

#define PII_REGISTER_DISTANCE_MEASURES(MEASURE) \
  PII_REGISTER_DISTANCE_MEASURE(MEASURE, int); \
  PII_REGISTER_DISTANCE_MEASURE(MEASURE, float); \
  PII_REGISTER_DISTANCE_MEASURE(MEASURE, double)

PII_REGISTER_DISTANCE_MEASURES(PiiGeometricDistance);
PII_REGISTER_DISTANCE_MEASURES(PiiSquaredGeometricDistance);
PII_REGISTER_DISTANCE_MEASURES(PiiCosineDistance);
PII_REGISTER_DISTANCE_MEASURES(PiiAbsDiffDistance);

PII_REGISTER_DISTANCE_MEASURES(PiiLogLikelihood);
PII_REGISTER_DISTANCE_MEASURES(PiiHistogramIntersection);
PII_REGISTER_DISTANCE_MEASURES(PiiJeffreysDivergence);
PII_REGISTER_DISTANCE_MEASURES(PiiChiSquaredDistance);

PII_REGISTER_DISTANCE_MEASURES(PiiMultiFeatureDistance);

PII_REGISTER_DISTANCE_MEASURE(PiiHammingDistance, int);
