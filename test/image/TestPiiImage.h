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

#ifndef _TESTPIIIMAGE_H
#define _TESTPIIIMAGE_H

#include <QObject>
#include <PiiMatrix.h>

class TestPiiImage : public QObject
{
  Q_OBJECT

public:
  TestPiiImage();

private slots:

  // Basic image handling
  void scaleNearestNeighborInterpolation();
  void scaleLinearInterpolation();
  void scaleColor();
  void rotate();
  void colorChannel();
  void setColorChannel();
  void detectEdges();
  void suppressNonMaxima();
  void medianFilter();
  void separateFilter();
  void filter();
  void intFilter();
  void maxFilter();
  void minFilter();

  // Thresholding
  void threshold();
  void inverseThreshold();
  void cut();
  void inverseCut();
  void zeroAbove();
  void zeroBelow();
  void twoLevelThreshold();
  void inverseTwoLevelThreshold();
  void hysteresisThreshold();
  void adaptiveThreshold();

  // Morphology
  void createMask();

  void erode();
  void dilate();
  void open();
  void close();
  void hitAndMiss();
  void border();
  void thin();
  void bottomHat();
  void labelImage();
  void labelLargerThan();

  // Histogram
  void equalize();
  void histogram();
  void cumulative();
  void normalize();
  void percentile();
  void backProject();

  // Boundaries
  void findBoundary();
  void findNextBoundary();
  void findBoundaries();

  // Distortions
  void unwarpCylinder();

  // Other
  void calculateDirection();
  void calculateProperties();
  void sweepLine();
  void crop();
  void xorMatch();

private:
  template <class TernaryFunction, class T>
  PiiMatrix<typename TernaryFunction::result_type> apply(const PiiMatrix<T>& mat,
                                                         TernaryFunction func,
                                                         typename TernaryFunction::second_argument_type p1,
                                                         typename TernaryFunction::third_argument_type p2);
  const PiiMatrix<int> _matThreshold;
};

#endif //_TESTPIIIMAGE_H
