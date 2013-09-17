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

#ifndef _PIICOORDINATETRACKER_H
#define _PIICOORDINATETRACKER_H

#include "PiiMultiHypothesisTracker.h"
#include "PiiCoordinateTrackerNode.h"
#include <PiiMatrix.h>


/**
 * PiiCoordinateTracker tracks points in Cartesian coordinates.
 *
 * This class does not yet implement the measurement model. It merely
 * augments PiiMultiHypothesisTracker by using PiiTrackerPoint* as the
 * measurement type and by adding facilities for managing measurements
 * and trajectories.
 *
 * PiiCoordinateTracker tracks coordinates in a `D` -dimensional
 * space. The coordinates can be expressed as any data type, `T`. 
 * Evaluation of new measurements is based on predictions. A
 * prediction is derived from an existing trajectory, and the distance
 * between a new measurement and the prediction works as a measure of
 * goodness.
 *
 */
template <class T, int D> class PiiCoordinateTracker :
  public PiiMultiHypothesisTracker<PiiVector<T,D>, PiiCoordinateTrackerNode<T,D>*>
{
public:
  typedef PiiVector<T,D> MeasurementType;
  typedef PiiCoordinateTrackerNode<T,D> TrajectoryType;
  typedef PiiMultiHypothesisTracker<MeasurementType, TrajectoryType*> ParentType;

  /**
   * Create a new coordinate tracker.
   */
  PiiCoordinateTracker();

  /**
   * Add new measurements to the tracker. This function first creates
   * a prediction for each active trajectory and then runs the
   * tracking algorithm.
   *
   * ~~~
   * PiiCoordinateTracker<int,2> tracker;
   * typedef PiiTrackerPoint<int,2> Point;
   * tracker.addMeasurements(QList<Point>() << Point(1,2) << Point(-1, 3), 0);
   * tracker.addMeasurements(QList<Point>() << Point(4,3) << Point(-2, 5) << Point(-4, 8), 1);
   * ~~~
   */
  void addMeasurements(const QList<MeasurementType>& measurements, int t);

  /**
   * A utility function that makes it possible to use a matrix to set
   * the measurements at once.
   *
   * ~~~
   * PiiCoordinateTracker<int,2> tracker;
   * tracker.addMeasurements(PiiMatrix<int>(2, 2,
   *                                        1, 2,
   *                                       -1, 3));
   * ~~~
   *
   * @param measurements a N-by-D matrix. Each row represents a
   * measurement point. If the number of columns in the matrix does
   * not equal `D`, the function does nothing.
   */
  void addMeasurements(const PiiMatrix<T>& measurements, int t);

  /**
   * A utility function that makes it possible to use a matrix to set
   * the measurements at once and set labels or block some
   * measurements depends on given labels.
   *
   * ~~~
   * PiiCoordinateTracker<int,2> tracker;
   * tracker.addMeasurements(PiiMatrix<int>(2, 2,
   *                                        1, 2,
   *                                       -1, 3),
   *                         PiiMatrix<int>(2,1,
   *                                        0,
   *                                        1));
   * ~~~
   *
   * @param measurements a N-by-D matrix. Each row represents a
   * measurement point. If the number of columns in the matrix does
   * not equal `D`, the function does nothing.
   *
   * @param labels a N-by-1 matrix.
   */
  void addMeasurements(const PiiMatrix<T>& measurements, const PiiMatrix<T>& labels, int t);

  /**
   * Set the prediction threshold. If the squared distance to a
   * measurement is larger than or equal to this value,
   * [measureFit(TrajectoryType**,const MeasurementType&,int)] will
   * return zero. The default threshold is 1.
   */
  void setPredictionThreshold(double predictionThreshold) { _dPredictionThreshold = predictionThreshold; }
  /**
   * Get the current prediction threshold.
   */
  double predictionThreshold() const { return _dPredictionThreshold; }

  /**
   * Set the threshold for connectable measurements until there is a
   * prediction. If the squared distance between measurements is less
   * than this threshold, the measurements can be connected, and
   * [measureFit(TrajectoryType**,const MeasurementType&,int)] will
   * return 1.0. If it is more, 0.0 will be returned. The default
   * threshold is 1.
   */
  void setInitialThreshold(double initialThreshold) { _dInitialThreshold = initialThreshold; }
  /**
   * Get the initial threshold.
   */
  double initialThreshold() const { return _dInitialThreshold; }

  /**
   * Sort trajectories using the trajectory type's LessThan comparison
   * functor. If the trajectory type is PiiCoordinateTrackerNode, the
   * results is that trajectories will be sorted in descending order
   * based on the trajectory fitness. If you want to reverse the
   * order, do something like the following:
   *
   * ~~~
   * typedef PiiCoordinateTrackerNode<double,2> TrajectoryType;
   * PiiCoordinateTracker<double,2> tracker;
   * QList<TrajectoryType> trajectories = tracker.trajectories();
   * qSort(trajectories.begin(), trajectories.end(), TrajectoryType::GreaterThan());
   * ~~~
   *
   * @see PiiCoordinateTrackerNode::trajectoryFitness()
   */
  void sortTrajectories();
  
protected:
  /**
   * Extends `trajectory` by adding a new PiiCoordinateTrackerNode to
   * the end of the linked list. The measurement will be stored into
   * the new node.
   */
  TrajectoryType* createTrajectory(TrajectoryType** trajectory, const MeasurementType& measurement, double fitness, int t);

  /**
   * Measure the likelihood of `measurement` belonging to 
   * `trajectory` at time instant `t`. The default implementation
   * computes squared distance between `measurement` and the
   * trajectory's prediction ([predict()]). If they are equal, 1.0 will
   * be returned. The returned value decreases linearly towards zero
   * with (squared) distance until [setPredictionThreshold()]
   * "predictionThreshold" is reached.
   *
   * If there is no prediction, `measurement` will be compared to the
   * trajectory's last point. If the squared distance is within @ref
   * setInitialThreshold() "initialThreshold", 1.0 will be returned. 
   * Otherwise, 0.0 will be returned.
   *
   * Should `measurement` work as a starting point for a new
   * trajectory, the tracker consults the [measureFit](const
   * MeasurementType&, int) function.
   */
  double measureFit(TrajectoryType** trajectory, const MeasurementType& measurement, int t) const;

  /**
   * Evaluate the likelihood that `measurement` is a starting point
   * of a new trajectory. The default implementation returns 1.0 when
   * `t` is zero, 0.0 otherwise. This creates new trajectories on the
   * first iteration only.
   *
   * @param measurement the measurement
   *
   * @param t the time instant
   *
   * @return the likelihood (0-1) of the measurement being a new
   * starting point.
   */
  virtual double measureFit(const MeasurementType& measurement, int t) const
  {
    Q_UNUSED(measurement);
    return t > 0 ? 0.0 : 1.0;
  }

  /**
   * Create a prediction for all trajectories at time instant `t`. 
   * The default implementation loops through the trajectories and
   * calls [predict(TrajectoryType*, int)] for each trajectory.
   */
  virtual void predict(int t)
  {
    for (int i =this->count(); i--; )
      {
        TrajectoryType* trajectory = this->at(i);
        trajectory->setPrediction(predict(trajectory, t));
      }
  }
  
  /**
   * Predict the location of a measurement at the time instant `t`
   * given a trajectory. The function should return a pointer to a
   * newly allocated measurement object. The tracker assumes the
   * ownership of the pointer. If a prediction cannot be derived yet,
   * zero must be returned. You need to implement this function as
   * your movement model The default implementation returns 0.
   *
   * @param trajectory the trajectory on which the prediction will be
   * based
   *
   * @param t the time instant for which the prediction should be
   * given. The coordinate tracker always requests forward predictions,
   * but it is in principle possible to predict backwards as well.
   */
  virtual MeasurementType* predict(TrajectoryType* trajectory, int t)
  {
    Q_UNUSED(trajectory);
    Q_UNUSED(t);
    return 0;
  }
  
private:
  double _dInitialThreshold;
  double _dPredictionThreshold;

};

template <class T, int D> PiiCoordinateTracker<T,D>::PiiCoordinateTracker() :
  _dInitialThreshold(1), _dPredictionThreshold(1)
{
}

template <class T, int D>
void PiiCoordinateTracker<T,D>::addMeasurements(const QList<MeasurementType>& measurements, int t)
{
  // Predict the next point for each trajectory
  predict(t);

  // Run the algorithm
  ParentType::addMeasurements(measurements, t);
}

template <class T, int D>
void PiiCoordinateTracker<T,D>::addMeasurements(const PiiMatrix<T>& measurements, int t)
{
  QList<MeasurementType> lst;
  if (measurements.columns() >= D)
    {
      for (int r=0; r<measurements.rows(); r++)
        lst << MeasurementType(measurements.row(r));
    }
  addMeasurements(lst, t);
}

template <class T, int D>
void PiiCoordinateTracker<T,D>::addMeasurements(const PiiMatrix<T>& measurements,
                                                const PiiMatrix<T>& labels,
                                                int t)
{
  QList<MeasurementType> lst;
  if (measurements.columns() >= D && measurements.rows() == labels.rows())
    {
      
      for (int r=0; r<measurements.rows(); r++)
        {
          if (labels(r,0) >= 1)
            lst << MeasurementType(measurements.row(r));
        }
    }
  addMeasurements(lst, t);
}


template <class T, int D>
double PiiCoordinateTracker<T,D>::measureFit(TrajectoryType** trajectory,
                                             const MeasurementType& measurement,
                                             int t) const
{
  if (trajectory)
    {
      // The trajectory has an prediction
      if ((*trajectory)->prediction())
        {
          // Measure distance to the prediction
          double dist = (*trajectory)->prediction()->squaredDistance(measurement);
          /*qDebug("  measurement = (%d,%d), prediction = (%d,%d), distance to prediction = %lf/%lf",
                 measurement.values[0],measurement.values[1],
                 (*trajectory)->prediction()->values[0], (*trajectory)->prediction()->values[1],
                 dist, _dPredictionThreshold);*/
          if (dist >= _dPredictionThreshold)
            return 0;
          // Scale linearly
          return (_dPredictionThreshold - dist) / _dPredictionThreshold;
        }
      else // no prediction yet -> are we in proximity?
        {
          double dist = (*trajectory)->measurement().squaredDistance(measurement);
          //qDebug("  distance to start = %lf/%lf", dist, _dInitialThreshold);
          // Return zero if distance is too large, one otherwise
          return dist < _dInitialThreshold ? 1.0 : 0.0;
        }
    }
  else
    return measureFit(measurement, t);
}

template <class T, int D> void PiiCoordinateTracker<T,D>::sortTrajectories()
{
  qSort(this->begin(), this->end(), typename TrajectoryType::LessThan());
}


template <class T, int D> typename PiiCoordinateTracker<T,D>::TrajectoryType*
PiiCoordinateTracker<T,D>::createTrajectory(TrajectoryType** trajectory,
                                            const MeasurementType& measurement,
                                            double fitness, int t)
{
  // Extend/branch an existing trajectory or create a totally new one
  return new TrajectoryType(measurement, t, fitness, trajectory ? *trajectory : 0);
}

#endif //_PIICOORDINATETRACKER_H
