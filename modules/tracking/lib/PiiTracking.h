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

#ifndef _PIITRACKING_H
#define _PIITRACKING_H

#include "PiiCoordinateTrackerNode.h"

/**
 * This namespace contains useful functions for typical tracking
 * tasks.
 *
 */
namespace PiiTracking
{
  /**
   * Predict the location of a measurement at time instant `t` given
   * a trajectory. This function calculates the velocity of an object
   * based on two previous measurements. It then scales this vector to
   * time instant `t` and returns a new measurement at the predicted
   * position.
   *
   * @param trajectory the last node in the trajectory
   *
   * @param t the current time instant
   *
   * @param maxPredictionLength the maximum distance between
   * trajectory nodes used for the estimation. The default value is 1,
   * which means that only the two previous ones are used. If the
   * value is 2, the estimate leaves one node in between in order to
   * get a less noisy estimate.
   */
  template <class T, int D> PiiVector<T,D>* predictConstantVelocity(PiiCoordinateTrackerNode<T,D>* trajectory, int t, int maxPredictionLength = 1)
  {
    typedef PiiVector<T,D> MeasurementType;

    PiiCoordinateTrackerNode<T,D>* next = trajectory->next(), *firstEstNode = trajectory;
    int previousTimeDiff = 0, estTimeDiff = 0;

    // Find the closest previous measurement with a non-zero time
    // difference to the head of the trajectory.
    while (next)
      {
        previousTimeDiff = firstEstNode->time() - next->time();
        if (previousTimeDiff)
          {
            firstEstNode = next;
            estTimeDiff += previousTimeDiff;
            // If the counter hits zero, we are done.
            if (!--maxPredictionLength)
              break;
          }
        next = next->next();
      }

    // Can we estimate?
    if (estTimeDiff)
      {
        MeasurementType previous(trajectory->measurement());
        double currentTimeDiff = t - trajectory->time();
        MeasurementType movement(previous - firstEstNode->measurement());
        movement.map(std::multiplies<double>(), currentTimeDiff / estTimeDiff);
        // Assume constant velocity
        return new MeasurementType(previous + // previous measurement
                                   movement); // plus previous velocity times time difference
      }
    else
      return 0;
  }

  /**
   * An evaluation function that returns the minimum fitness of
   * `trajectory` and the next node in chain. Using this function as an
   * evaluator for trajectories sets the trajectory fitness to the
   * minimum of individual measurement fitnesses.
   */
  template <class T, int D> double evaluateMinimumFitness(PiiCoordinateTrackerNode<T,D>* trajectory)
  {
    // The fitness of the whole trajectory is equal to the minimun
    // fitness of a single measurement.
    double minScore = trajectory->measurementFitness();
    // If the trajectory is longer than one measurement, take the
    // minimum of the previous score and the current measurement
    // fitness.
    if (trajectory->next())
      minScore = qMin(minScore, trajectory->next()->trajectoryFitness());

    return minScore;
  }

  /**
   * An evaluation function that returns the average measurement
   * fitness of `trajectory`. The average is calculated iteratively
   * by using the fitness value of the next node in chain only.
   */
  template <class T, int D> double evaluateAverageFitness(PiiCoordinateTrackerNode<T,D>* trajectory)
  {
    double avgScore = trajectory->measurementFitness();
    double multiplier = 1.0 / trajectory->length();
    // If the trajectory is longer than one measurement, iteratively
    // calculate the mean
    if (trajectory->next())
      avgScore = multiplier * avgScore + (1.0 - multiplier) * trajectory->next()->trajectoryFitness();

    return avgScore;
  }
}

#endif //_PIITRACKING_H
