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

#ifndef _PIIHOUGHTRANSFORM_H
# error "Never use <PiiHoughTransform-templates.h> directly; include <PiiHoughTransform.h> instead."
#endif

#include <PiiMath.h>

template <class T, class Matrix, class UnaryOp>
PiiMatrix<T> PiiHoughTransform::transform(const Matrix& img, UnaryOp rule)
{
  const int iRows = img.rows();
  const int iCols = img.columns();
  setSize(iRows, iCols);

  int iStartDistance = startDistance(), iEndDistance = endDistance();
  int iStartAngle = startAngle(), iEndAngle = endAngle();
  const double dAngleResolution = angleResolution(), dDistanceResolution = distanceResolution();

  double centerX = double(iCols-1)/2.0; // the center point of X coordinates
  double centerY = double(iRows-1)/2.0; // the center point of Y coordinates

  // Maximum distance from origin.
  double dMaxDistance = ::sqrt(centerX*centerX + centerY*centerY) / dDistanceResolution;

  iStartDistance = Pii::round<int>(qBound(-dMaxDistance, double(iStartDistance)/dDistanceResolution, dMaxDistance));
  iEndDistance = Pii::round<int>(qBound(-dMaxDistance, double(iEndDistance)/dDistanceResolution, dMaxDistance));

  int iDistances = iEndDistance - iStartDistance + 1;
  int iAngles = Pii::round<int>((iEndAngle - iStartAngle) / dAngleResolution);

  //qDebug("distances: %d -> %d (%d)", iStartDistance, iEndDistance, iDistances);
  //qDebug("angles: %d -> %d (%d)", iStartAngle, iEndAngle, iAngles);
  //qDebug("center: (%lf, %lf)", centerY, centerX);

  PiiMatrix<T> result(iDistances, iAngles);

  initSinCosTables(iAngles);

  const double* pCosAngleValues = cosTable();
  const double* pSinAngleValues = sinTable();

  for (typename Matrix::const_iterator it = img.begin(); it != img.end(); ++it)
    // Is this pixel part of target?
    if (rule(*it))
      {
        //qDebug("(%d, %d) matches", r, c);
        double coordX = double(it.column()) - centerX;
        double coordY = double(it.row()) - centerY;
        for (int omega=0; omega<iAngles; ++omega)
          {
            // Calculate distance to origin
            int d = Pii::round<int>((coordX*pCosAngleValues[omega] +
                                     coordY*pSinAngleValues[omega])/dDistanceResolution);
            //qDebug("Accessing (%d, %d) of %d-by-%d", d-iStartDistance, omega, iDistances, iAngles);
            if (d >= iStartDistance && d <= iEndDistance)
              result(d - iStartDistance, omega) += T(*it);
          }
      }

  return result;
}
