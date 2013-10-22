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

#ifndef _PIIMULTIHYPOTHESISTRACKER_H
#define _PIIMULTIHYPOTHESISTRACKER_H

#include <QList>

/**
 * The tracking algorithm uses a greedy breadth-first search algorithm
 * to find the most likely measurement sequence, given a measurement
 * model. The algorithm works as follows:
 *
 * - Initialize an empty set of trajectories.
 *
 * - At each time step read in a set of N candidate measurements.
 *
 * - Evaluate how well each of the N measurements fits into the
 * current set of M candidate trajectories (N x M evaluations). 
 * ([measureFit()])
 *
 * - Generate a new set of candidate trajectories by extending the
 * old ones with the measurements with non-zero probabilities. This
 * may create many branches for a single trajectory. 
 * ([createTrajectory()]) Note that trajectories will be cleared
 * and the new trajectories will be added to the empty list.
 *
 * - Evaluate how likely it is that a measurement is a starting
 * point for a new trajectory (N evaluations). ([measureFit()])
 *
 * - For each measurement with a non-zero likelihood of being part
 * of a new trajectory, create a new trajectory and add it to the set
 * of candidate trajectories. ([createTrajectory()])
 *
 * The tracker is a template class that works with any type of
 * measurements (e.g. 2-D or 3-D points) and trajectories (e.g. lists
 * of points). Measurements and trajectories can even be implemented
 * as indices to external storage.
 *
 */
template <class Measurement, class Trajectory> class PiiMultiHypothesisTracker : public QList<Trajectory>
{
public:
  typedef Measurement MeasurementType;
  typedef Trajectory TrajectoryType;
  
  /**
   * Add a new set of candidate measurements to the tracker. This will
   * run one cycle of the algorithm.
   *
   * @param measurements a list of candidate measurements
   *
   * @param t the current time instant
   */
  virtual void addMeasurements(const QList<MeasurementType>& measurements, int t);

protected:
  virtual ~PiiMultiHypothesisTracker() {}
  
  /**
   * Create a new trajectory by extending an old one.
   *
   * @param trajectory the old trajectory to be extended/branched. If
   * this parameter is 0, a new trajectory should be created.
   *
   * @param measurement the measurement to add to the trajectory.
   *
   * @param fitness fit of the measurement to the trajectory. Measured
   * by the [measureFit()] function.
   *
   * @param t the current time instant
   *
   * @return a new trajectory
   */
  virtual TrajectoryType createTrajectory(TrajectoryType* trajectory, const MeasurementType& measurement, double fitness, int t) = 0;

  /**
   * Measure how well `measurement` fits into `trajectory`.
   *
   * @param trajectory the trajectory `measurement` is evaluated
   * against. If this parameter is 0, the function should tell how
   * likely the measurement is to create a new trajectory. This
   * function implements the measurement model.
   *
   * @param measurement the measurement to evaluate.
   *
   * @param t the current time instant.
   *
   * @return an evaluation. Typically a value between zero (certainly
   * not) and one (absolutely yes).
   */
  virtual double measureFit(TrajectoryType* trajectory, const MeasurementType& measurement, int t) const = 0;

  /**
   * Get the current index of the trajectory.
   */
  int currentTrajectoryIndex() const
  {
    return _iTrajectoryIndex;
  }

  /**
   * Get the current index of the measurement.
   */
  int currentMeasurementIndex() const
  {
    return _iMeasurementIndex;
  }
private:
  /**
   * The index of the measurement currently being inspected by the
   * tracking algorithm. This index can be used by subclasses to store
   * information specific to a certain sample. The index refers to the
   * `measurements` list given as a parameter to `addMeasurements`.
   */
  int _iMeasurementIndex;
  /**
   * The index of the trajectory currently being inspected by the
   * tracking algorithm. Works analogously to `_iMeasurementIndex`.
   * The index refers to trajectories.
   */
  int _iTrajectoryIndex;
};


template <class Measurement, class Trajectory>
void PiiMultiHypothesisTracker<Measurement,Trajectory>::addMeasurements(const QList<MeasurementType>& measurements, int t)
{
  QList<TrajectoryType> oldTrajectories = *this;
  this->clear();

  for (_iTrajectoryIndex=oldTrajectories.size(); _iTrajectoryIndex--; )
    {
      for (_iMeasurementIndex=measurements.size(); _iMeasurementIndex--; )
        {
          // See how well this measurement would fit into the current
          // trajectory.
          double score = measureFit(&oldTrajectories[_iTrajectoryIndex], measurements[_iMeasurementIndex], t);
          // If it fits even in principle, create a new trajectory
          if (score > 0)
            this->append(createTrajectory(&oldTrajectories[_iTrajectoryIndex], measurements[_iMeasurementIndex], score, t));
        }
    }

  for (_iMeasurementIndex=measurements.size(); _iMeasurementIndex--; )
    {
      // Is this measurement likely to create a new trajectory?
      double score = measureFit(0, measurements[_iMeasurementIndex], t);
      // If this measurement can work as a starting point with a
      // non-zero probability, create a new trajectory.
      if (score > 0)
        this->append(createTrajectory(0, measurements[_iMeasurementIndex], score, t));
    }
}

#endif //_PIIMULTIHYPOTHESISTRACKER_H
