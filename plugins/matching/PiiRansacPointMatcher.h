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

#ifndef _PIIRANSACPOINTMATCHER_H
#define _PIIRANSACPOINTMATCHER_H

#include "PiiPointMatchingOperation.h"

class PiiRansac;

/**
 * A superclass for operations that use [RANSAC](PiiRansac) for
 * matching points. This class performs no useful action of its own,
 * but merely reflects the configuration values of the RANSAC
 * algorithm as properties. See PiiRansac for documentation on the
 * properties.
 *
 */
class PII_MATCHING_EXPORT PiiRansacPointMatcher : public PiiPointMatchingOperation
{
  Q_OBJECT
  
  Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations);
  Q_PROPERTY(int maxSamplings READ maxSamplings WRITE setMaxSamplings);
  Q_PROPERTY(int minInliers READ minInliers WRITE setMinInliers);
  Q_PROPERTY(double fittingThreshold READ fittingThreshold WRITE setFittingThreshold);
  Q_PROPERTY(double selectionProbability READ selectionProbability WRITE setSelectionProbability);
  
public:
  void setMaxIterations(int maxIterations);
  int maxIterations() const;
  void setMaxSamplings(int maxSamplings);
  int maxSamplings() const;
  void setMinInliers(int minInliers);
  int minInliers() const;
  void setFittingThreshold(double fittingThreshold);
  double fittingThreshold() const;
  void setSelectionProbability(double selectionProbability);
  double selectionProbability() const;
  
protected:
  /// @internal
  class Data : public PiiPointMatchingOperation::Data
  {
  public:
    Data(int pointDimensions, PiiRansac* ransac);
    ~Data();
    
    PiiRansac* pRansac;
  };
  PII_D_FUNC;

  /// @internal
  PiiRansacPointMatcher(Data* data);

  /**
   * Constructs a new PiiRansacPointMatcher with the given number of
   * dimensions for the point space and a pointer to a newly allocated
   * RANSAC estimator. This class takes the ownership of the pointer.
   */
  PiiRansacPointMatcher(int pointDimensions, PiiRansac* ransac);

  /**
   * Returns a reference to the internal RANSAC estimator.
   */
  PiiRansac& ransac();
  /**
   */
  const PiiRansac& ransac() const;
};

#endif //_PIIRANSACPOINTMATCHER_H
