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

#include "PiiBoostClassifierOperation.h"

#include <PiiYdinTypes.h>

PiiBoostClassifierOperation::Data::Data() :
  PiiClassifierOperation::Data(PiiClassification::WeightedLearner),
  algorithm(PiiClassification::RealBoost),
  iMaxClassifiers(100),
  dMinError(0)
{
}

PiiBoostClassifierOperation::PiiBoostClassifierOperation(Data* d) :
  PiiClassifierOperation(d)
{
}

PiiBoostClassifierOperation::~PiiBoostClassifierOperation()
{}

PiiClassification::BoostingAlgorithm PiiBoostClassifierOperation::algorithm() const { return _d()->algorithm; }
void PiiBoostClassifierOperation::setAlgorithm(PiiClassification::BoostingAlgorithm algorithm) { _d()->algorithm = algorithm; }
void PiiBoostClassifierOperation::setMaxClassifiers(int maxClassifiers) { _d()->iMaxClassifiers = maxClassifiers; }
int PiiBoostClassifierOperation::maxClassifiers() const { return _d()->iMaxClassifiers; }
void PiiBoostClassifierOperation::setMinError(double minError) { _d()->dMinError = minError; }
double PiiBoostClassifierOperation::minError() const { return _d()->dMinError; }
