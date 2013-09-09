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

#ifndef _PIICONSTANTVELOCITYTRACKER_H
#define _PIICONSTANTVELOCITYTRACKER_H

#include "PiiCoordinateTracker.h"
#include "PiiTracking.h"

/**
 * A simple tracker that assumes movement has constant velocity. This
 * tracker is mainly for illustration purposes as it does no more than
 * implements the measurement mode by calling
 * PiiTracking::predictConstantVelocity().
 *
 * @ingroup PiiTrackingPlugin
 */
template <class T, int D> class PiiConstantVelocityTracker : public PiiCoordinateTracker<T,D>
{
protected:
  typename PiiCoordinateTracker<T,D>::MeasurementType* predict(typename PiiCoordinateTracker<T,D>::TrajectoryType* trajectory, int t);
};

template <class T, int D> typename PiiCoordinateTracker<T,D>::MeasurementType*
PiiConstantVelocityTracker<T,D>::predict(typename PiiCoordinateTracker<T,D>::TrajectoryType* trajectory, int t)
{
  return ::PiiTracking::predictConstantVelocity(trajectory, t);
}

#endif //_PIICONSTANTVELOCITYTRACKER_H
