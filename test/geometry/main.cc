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

#include "TestPiiGeometry.h"

#include <PiiGeometry.h>
#include <QtTest>
#include <PiiMatrixUtil.h>
#include <QDebug>
#include <PiiMath.h>


void TestPiiGeometry::simplifyVertices()
{
  {
    PiiMatrix<double> points(0,2);
    for (double alpha=0; alpha < M_PI*2; alpha += M_PI/16)
      points.insertRow(-1, 5*sin(alpha), 5*cos(alpha));
    points.insertRow(-1, points[0]); // duplicate first point

    PiiMatrix<double> pruned = PiiGeometry::reduceVertices(points, 1.0);

    int index = 0;
    for (double alpha=0; alpha < M_PI*2; alpha += M_PI/4, ++index)
      {
        QVERIFY( abs(pruned(index, 0) - 5*sin(alpha)) < 1e-10 );
        QVERIFY( abs(pruned(index, 1) - 5*cos(alpha)) < 1e-10 );
        //QCOMPARE(pruned(index,0), 5*sin(alpha));
        //QCOMPARE(pruned(index,1), 5*cos(alpha));
      }
    QCOMPARE(pruned(0,0), pruned(pruned.rows()-1,0));
    QCOMPARE(pruned(0,1), pruned(pruned.rows()-1,1));
  }
  {
    PiiMatrix<int> points(1,2,
                          1,2);
    PiiMatrix<int> pruned = PiiGeometry::reduceVertices(points,1.0);
    QVERIFY(Pii::equals(pruned, points));
  }
}

void TestPiiGeometry::lineToLineDistance()
{
  {
    // Lines are equal
    PiiMatrix<double> first(2,3,
                            1.0, 0.0, 0.0,
                            0.0, 0.0, 0.0);
    
    PiiMatrix<double> second(2,3,
                             1.0, 0.0, 0.0,
                             0.0, 0.0, 0.0);

    double distance = PiiGeometry::lineToLineDistance(first, second);
    QCOMPARE(distance, 0.0);
  }

  {
    // Lines are parallel
    PiiMatrix<double> first(2,3,
                            1.0, 0.0, 0.0,
                            0.0, 0.0, 0.0);
    
    PiiMatrix<double> second(2,3,
                             1.0, 0.0, 0.0,
                             0.0, 0.0, 1.0);

    double distance = PiiGeometry::lineToLineDistance(first, second);
    QCOMPARE(distance, 1.0);
  }
  
  {
    // Lines are on the same plane, and not parallel
    PiiMatrix<double> first(2,3,
                            1.0, 0.0, 0.0,
                            0.0, 0.0, 0.0);
    
    PiiMatrix<double> second(2,3,
                             -1.0, 1.0, 0.0,
                             1.0, 5.0, 0.0);

    double distance = PiiGeometry::lineToLineDistance(first, second);
    QCOMPARE(distance, 0.0);
  }

  {
    // Lines are on different planes
    PiiMatrix<double> first(2,3,
                            1.0, 0.0, 0.0,
                            0.0, 0.0, 0.0);
    
    PiiMatrix<double> second(2,3,
                             -1.0, 1.0, 0.0,
                             0.0, 0.0, 3.0);
    double distance = PiiGeometry::lineToLineDistance(first, second);
    QCOMPARE(distance, 3.0);
  }
  
}

void TestPiiGeometry::pointToLineSegmentDistance()
{
  {
    PiiMatrix<double> dMatLine(2,2, 2.0, 0.0,
                               2.0, 3.0);
    PiiMatrix<double> dMatPoint(1,2, 1.0,1.0);

    QCOMPARE(PiiGeometry::pointToLineSegmentDistance(dMatLine, dMatPoint),1.0);
      
  }

  {
    PiiMatrix<double> dMatLine(2,3, 1.0, 1.0, 1.0,
                               4.0, 4.0, 4.0);
    PiiMatrix<double> dMatPoint(1,3, 1.0, 1.0, 1.0);

    QCOMPARE(PiiGeometry::pointToLineSegmentDistance(dMatLine,dMatPoint),0.0);

    dMatLine(0,0) = 3.0;
    dMatLine(0,1) = 0.0;
    dMatLine(0,2) = 3.0;
    dMatLine(1,0) = 6.0;
    dMatLine(1,1) = 0.0;
    dMatLine(1,2) = 3.0;

    dMatPoint(0,0) = 3.0;
    dMatPoint(0,1) = 0.0;
    dMatPoint(0,2) = 0.0;
    
    QCOMPARE(PiiGeometry::pointToLineSegmentDistance(dMatLine, dMatPoint),3.0);


  }
  
  

}


int main(int argc, char *argv[]) 
{
  //Contains all geometry-namespace functions.
  
  TestPiiGeometry geometry;
  QTest::qExec(&geometry,argc, argv);
  
  return 0;
}
