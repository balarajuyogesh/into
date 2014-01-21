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

#include "TestPiiTracking.h"


#include <PiiConstantVelocityTracker.h>
#include <PiiExtendedCoordinateTracker.h>
#include <QtTest>
#include <QtAlgorithms>

// Creates a branching list of nodes and deletes them.
void TestPiiTracking::testLinkedList()
{
  typedef PiiCoordinateTrackerNode<int,2> NodeType;
  NodeType
    *n1 = new NodeType(static_cast<NodeType*>(0)),
    *n2 = new NodeType(static_cast<NodeType*>(n1)),
    *n3 = new NodeType(static_cast<NodeType*>(n1)),
    *n4 = new NodeType(static_cast<NodeType*>(n2)),
    *n5 = new NodeType(static_cast<NodeType*>(n2)),
    *n6 = new NodeType(static_cast<NodeType*>(n5));

  /* Initial situation
   *
   * n1--> n2--> n4
   *  |     |
   *  |     +--> n5--> n6
   *  +--> n3
   */
  QCOMPARE(n1->branches(), 2);
  QCOMPARE(n2->branches(), 2);
  QCOMPARE(n3->branches(), 0);
  QCOMPARE(n4->branches(), 0);
  QCOMPARE(n5->branches(), 1);
  QCOMPARE(n6->branches(), 0);
  delete n4; // deletes only n4

  /* Current situation
   *
   * n1--> n2
   *  |     |
   *  |     +--> n5--> n6
   *  +--> n3
   */
  QCOMPARE(n2->branches(), 1);
  delete n6; // deletes n6, n5, and n2

  /* Current situation
   *
   * n1
   *  |
   *  +--> n3
   */
  QCOMPARE(n1->branches(), 1);
  delete n3; // deletes n3 and n1

  NodeType n7(PiiVector<int,2>(1,2));
  NodeType n8(n7);

  QCOMPARE(n8.measurement()[0],1);
  QCOMPARE(n8.measurement()[1],2);
}

void TestPiiTracking::testConstantVelocityTracker()
{
  typedef PiiVector<int,2> Point;
  typedef PiiCoordinateTrackerNode<int,2>* Trajectory;

  PiiConstantVelocityTracker<int,2> tracker;

  // Ensure that the tracker considers all alternatives
  tracker.setInitialThreshold(20);
  tracker.setPredictionThreshold(200);

  /* Measurement points
   *
   * y|
   *  |
   * 7|       .
   * 6|
   * 5|     . . .
   * 4|   .   . .
   * 3|   . .
   * 2| . .   .
   * 1| . . .   .
   * 0|__________________
   *  0 1 2 3 4 5       x
   *    0 1 2 3 4       t
   */
  tracker.addMeasurements(QList<Point>() << Point(1,1) << Point(1,2), 0);
  tracker.addMeasurements(QList<Point>() << Point(2,1) << Point(2,2) << Point(2,3) << Point(2,4), 1);
  tracker.addMeasurements(QList<Point>() << Point(3,1) << Point(3,3) << Point(3,5), 2);
  tracker.addMeasurements(QList<Point>() << Point(4,2) << Point(4,4) << Point(4,5) << Point(4,7), 3);
  tracker.addMeasurements(PiiMatrix<int>(3,2,
                                         5,1,
                                         5,3,
                                         5,4),
                          4);

  // Now we should have 2*4*3*4*3 trajectories in total
  QCOMPARE(tracker.count(), 2*4*3*4*3);
}

void TestPiiTracking::testExtendedCoordinateTracker()
{
  typedef PiiVector<double,2> Point;
  typedef PiiCoordinateTrackerNode<double,2>* Trajectory;

  PiiExtendedCoordinateTracker<double,2> tracker;

  PiiMatrix<int> routes[] =
    {
      PiiMatrix<int>(20,2,
                     0,0,
                     0,0,
                     2,1,
                     3,1,
                     4,2,
                     5,3,
                     5,4,
                     6,5,
                     7,6,
                     8,7,
                     9,8,
                     9,9,
                     8,10,
                     7,11,
                     7,12,
                     7,13,
                     7,14,
                     7,15,
                     0,0,
                     0,0),
      PiiMatrix<int>(20,2,
                     0,0,
                     0,0,
                     1,9,
                     2,9,
                     3,9,
                     4,8,
                     5,7,
                     6,6,
                     7,5,
                     8,4,
                     8,3,
                     9,2,
                     10,1,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0),
      PiiMatrix<int>(20,2,
                     5,12,
                     6,12,
                     7,12,
                     8,12,
                     0,0,
                     0,0,
                     11,12,
                     12,12,
                     13,11,
                     13,10,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     13,4,
                     13,3,
                     13,2,
                     13,1),
      PiiMatrix<int>(20,2,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     11,15,
                     10,13,
                     0,0,
                     8,9,
                     6,7,
                     4,6,
                     5,2,
                     4,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0),
      PiiMatrix<int>(20,2,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     0,0,
                     15,12,
                     14,11,
                     13,10,
                     0,0,
                     0,0,
                     10,6,
                     10,5,
                     10,4,
                     9,3,
                     8,2,
                     7,0,
                     0,0)
    };

  tracker.setInitialThreshold(6);
  tracker.setPredictionThreshold(2);
  tracker.setMaximumStopTime(3);
  tracker.setMaximumPredictionLength(2);

  for (int t=0; t<20; ++t)
    {
      QList<Point> measurements;
      for (int r=0; r<2; r++)
        {
          int* row = routes[r].row(t);
          if (row[0] != 0 || row[1] != 0)
            measurements << Point(double(routes[r](t,0)), double(routes[r](t,1)));
        }

      tracker.addMeasurements(measurements, t);

      //qDebug("Routes after time step %i", t);
      for (int i=0; i<tracker.count(); ++i)
        {
          Trajectory tr = tracker[i];
          QString points;
          while (tr)
            {
              points += QString(" (%1,%2)").arg(tr->measurement()[0]).arg(tr->measurement()[1]);
              tr = tr->next();
            }
          //qDebug("  %i (%i points, score %lf): %s", i, tracker[i]->length(), tracker[i]->trajectoryFitness(), qPrintable(points));
        }
    }
}

QTEST_MAIN(TestPiiTracking)
