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

#include "PiiRansacPointMatcher.h"
#include <PiiRansac.h>

PiiRansacPointMatcher::Data::Data(int pointDimensions, PiiRansac* ransac) :
  PiiPointMatchingOperation::Data(pointDimensions),
  pRansac(ransac)
{}

PiiRansacPointMatcher::Data::~Data()
{
  delete pRansac;
}

PiiRansacPointMatcher::PiiRansacPointMatcher(Data* data) :
  PiiPointMatchingOperation(data)
{}

PiiRansacPointMatcher::PiiRansacPointMatcher(int pointDimensions, PiiRansac* ransac) :
  PiiPointMatchingOperation(new Data(pointDimensions, ransac))
{}

void PiiRansacPointMatcher::setMaxIterations(int maxIterations) { _d()->pRansac->setMaxIterations(maxIterations); }
int PiiRansacPointMatcher::maxIterations() const { return _d()->pRansac->maxIterations(); }
void PiiRansacPointMatcher::setMaxSamplings(int maxSamplings) { _d()->pRansac->setMaxSamplings(maxSamplings); }
int PiiRansacPointMatcher::maxSamplings() const { return _d()->pRansac->maxSamplings(); }
void PiiRansacPointMatcher::setMinInliers(int minInliers) { _d()->pRansac->setMinInliers(minInliers); }
int PiiRansacPointMatcher::minInliers() const { return _d()->pRansac->minInliers(); }
void PiiRansacPointMatcher::setFittingThreshold(double fittingThreshold) { _d()->pRansac->setFittingThreshold(fittingThreshold); }
double PiiRansacPointMatcher::fittingThreshold() const { return _d()->pRansac->fittingThreshold(); }
void PiiRansacPointMatcher::setSelectionProbability(double selectionProbability) { _d()->pRansac->setSelectionProbability(selectionProbability); }
double PiiRansacPointMatcher::selectionProbability() const { return _d()->pRansac->selectionProbability(); }

PiiRansac& PiiRansacPointMatcher::ransac() { return *_d()->pRansac; }
const PiiRansac& PiiRansacPointMatcher::ransac() const { return *_d()->pRansac; }
