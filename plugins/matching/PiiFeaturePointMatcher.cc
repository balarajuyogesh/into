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

#include "PiiFeaturePointMatcher.h"

namespace PiiMatching
{
  Match::Data::Data() :
    iRefCount(1),
    iModelIndex(-1)
  {}

  Match::Data::Data(int modelIndex,
                    const PiiMatrix<double> transformParams,
                    const QList<QPair<int,int> >& matchedPoints) :
    iRefCount(1),
    iModelIndex(modelIndex),
    matTransformParams(transformParams),
    lstMatchedPoints(matchedPoints)
  {}
  
  Match::Match() :
    d(new Data)
  {}
  
  Match::Match(int modelIndex,
               const PiiMatrix<double> transformParams,
               const QList<QPair<int,int> >& matchedPoints) :
    d(new Data(modelIndex, transformParams, matchedPoints))
  {}
      
  Match::Match(const Match& other) :
    d(other.d)
  {
    d->iRefCount.ref();
  }

  Match::~Match()
  {
    if (!d->iRefCount.deref())
      delete d;
  }

  Match& Match::operator= (const Match& other)
  {
    other.d->iRefCount.ref();
    if (!d->iRefCount.deref())
      delete d;
    d = other.d;
    return *this;
  }

  int Match::modelIndex() const { return d->iModelIndex; }
  PiiMatrix<double> Match::transformParams() const { return d->matTransformParams; }
  QList<QPair<int,int> > Match::matchedPoints() const { return d->lstMatchedPoints; }
  int Match::matchedPointCount() const { return d->lstMatchedPoints.size(); }
}
