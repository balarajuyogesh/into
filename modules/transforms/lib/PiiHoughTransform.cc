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

#include "PiiHoughTransform.h"
#include <PiiMath.h>

PiiHoughTransform::Data::Data() :
  dAngleResolution(1),
  dDistanceResolution(1),
  iStartAngle(0),
  iEndAngle(180),
  iStartDistance(Pii::Numeric<int>::minValue()),
  iEndDistance(Pii::Numeric<int>::maxValue()),
  iRows(0),
  iColumns(0),
  dMaxDistance(0),
  pSinTable(0),
  pCosTable(0),
  iPreviousAngles(-1),
  iPreviousStartAngle(0),
  dPreviousAngleResolution(0)
{}

PiiHoughTransform::Data::Data(double angleResolution,
                              double distanceResolution,
                              int startAngle,
                              int endAngle,
                              int startDistance,
                              int endDistance) :
  dAngleResolution(angleResolution),
  dDistanceResolution(distanceResolution),
  iStartAngle(startAngle),
  iEndAngle(endAngle),
  iStartDistance(startDistance),
  iEndDistance(endDistance),
  iRows(0),
  iColumns(0),
  dMaxDistance(0),
  pSinTable(0),
  pCosTable(0),
  iPreviousAngles(-1),
  iPreviousStartAngle(0),
  dPreviousAngleResolution(0)
{}

PiiHoughTransform::Data::Data(const Data& other) :
  dAngleResolution(other.dAngleResolution),
  dDistanceResolution(other.dDistanceResolution),
  iStartAngle(other.iStartAngle),
  iEndAngle(other.iEndAngle),
  iStartDistance(other.iStartDistance),
  iEndDistance(other.iEndDistance),
  iRows(other.iRows),
  iColumns(other.iColumns),
  dMaxDistance(other.dMaxDistance),
  pSinTable(0),
  pCosTable(0),
  iPreviousAngles(-1),
  iPreviousStartAngle(0),
  dPreviousAngleResolution(0)
{}

PiiHoughTransform::Data::~Data()
{
  delete[] pSinTable;
  delete[] pCosTable;
}

PiiHoughTransform::PiiHoughTransform() :
  d(new Data)
{
}

PiiHoughTransform::PiiHoughTransform(double angleResolution,
                                     double distanceResolution,
                                     int startAngle,
                                     int endAngle,
                                     int startDistance,
                                     int endDistance) :
  d(new Data(angleResolution,
             distanceResolution,
             startAngle,
             endAngle,
             startDistance,
             endDistance))
{}

PiiHoughTransform::PiiHoughTransform(const PiiHoughTransform& other) :
  d(other.d->reserved())
{}

PiiHoughTransform::~PiiHoughTransform()
{
  d->release();
}

PiiHoughTransform& PiiHoughTransform::operator= (const PiiHoughTransform& other)
{
  other.d->assignTo(d);
  return *this;
}

void PiiHoughTransform::setStartAngle(int startAngle) { _d()->iStartAngle = startAngle; }
int PiiHoughTransform::startAngle() const { return _d()->iStartAngle; }
void PiiHoughTransform::setEndAngle(int endAngle) { _d()->iEndAngle = endAngle; }
int PiiHoughTransform::endAngle() const { return _d()->iEndAngle; }
void PiiHoughTransform::setAngleResolution(double angleResolution) { _d()->dAngleResolution = angleResolution; }
double PiiHoughTransform::angleResolution() const { return _d()->dAngleResolution; }
void PiiHoughTransform::setStartDistance(int startDistance) { _d()->iStartDistance = startDistance; }
int PiiHoughTransform::startDistance() const { return _d()->iStartDistance; }
void PiiHoughTransform::setEndDistance(int endDistance) { _d()->iEndDistance = endDistance; }
int PiiHoughTransform::endDistance() const { return _d()->iEndDistance; }
void PiiHoughTransform::setDistanceResolution(double distanceResolution) { _d()->dDistanceResolution = distanceResolution; }
double PiiHoughTransform::distanceResolution() const { return _d()->dDistanceResolution; }

void PiiHoughTransform::setSize(int rows, int columns)
{
  PII_D;
  d->iRows = rows;
  d->iColumns = columns;
  d->dMaxDistance = Pii::hypotenuse(rows/2, columns/2);
}

double PiiHoughTransform::distance(int row) const
{
  const PII_D;
  return d->iStartDistance < -d->dMaxDistance ?
    d->dDistanceResolution * row - d->dMaxDistance :
    d->dDistanceResolution * row + d->iStartDistance;
}

double PiiHoughTransform::angle(int column) const
{
  return d->dAngleResolution * column + d->iStartAngle;
}

PiiMatrix<int> PiiHoughTransform::lineEnds(int row, int column) const
{
  const PII_D;
  double dTheta = angle(column) * M_PI/180;
  double dDistance = distance(row);

  double dLeftY, dLeftX, dRightY, dRightX;
  // Vertical line
  if (Pii::almostEqualRel(dTheta, 0.0, 1e-4))
    {
      dLeftX = dRightX = d->iColumns/2.0 + dDistance;
      dLeftY = 0;
      dRightY = d->iRows-1;
    }
  // Horizontal line
  else if (Pii::almostEqualRel(dTheta, M_PI/2, 1e-4))
    {
      dLeftY = dRightY = d->iRows/2.0 + dDistance;
      dLeftX = 0;
      dRightX = d->iColumns-1;
    }
  // Any other line
  else
    {
      double dSinTheta = ::sin(dTheta), dCosTheta = ::cos(dTheta);
      double dHalfCols = d->iColumns/2.0, dHalfRows = d->iRows/2;
      dLeftX = -dHalfCols;
      dLeftY = (dDistance - dLeftX*dCosTheta)/dSinTheta;
      // Left edge intersection is above the image
      if (dLeftY < -dHalfRows)
        {
          dLeftY = -dHalfRows;
          dLeftX = (dDistance - dLeftY*dSinTheta)/dCosTheta;
        }
      // Left edge intersection is below the image
      else if (dLeftY > dHalfRows - 1)
        {
          dLeftY = dHalfRows - 1;
          dLeftX = (dDistance - dLeftY*dSinTheta)/dCosTheta;
        }

      dRightX = dHalfCols - 1;
      dRightY = (dDistance - dRightX*dCosTheta)/dSinTheta;
      // Right edge intersection is above the image
      if (dRightY < -dHalfRows)
        {
          dRightY = -dHalfRows;
          dRightX = (dDistance - dRightY*dSinTheta)/dCosTheta;
        }
      // Right edge intersection is below the image
      else if (dRightY > dHalfRows - 1)
        {
          dRightY = dHalfRows - 1;
          dRightX = (dDistance - dRightY*dSinTheta)/dCosTheta;
        }
      dLeftX += dHalfCols;
      dRightX += dHalfCols;
      dLeftY += dHalfRows;
      dRightY += dHalfRows;
    }
  //qDebug("(%lf %lf) -> (%lf %lf)", dLeftX, dLeftY, dRightX, dRightY);
  return PiiMatrix<int>(1,4,
                        Pii::round<int>(dLeftX), Pii::round<int>(dLeftY),
                        Pii::round<int>(dRightX), Pii::round<int>(dRightY));
}

void PiiHoughTransform::initSinCosTables(int angles)
{
  PII_D;
  if (angles == d->iPreviousAngles &&
      d->iStartAngle && d->iPreviousStartAngle &&
      d->dAngleResolution == d->dPreviousAngleResolution)
    return;

  d->iPreviousAngles = angles;
  d->iPreviousStartAngle = d->iStartAngle;
  d->dPreviousAngleResolution = d->dAngleResolution;

  delete[] d->pCosTable;
  delete[] d->pSinTable;

  d->pCosTable = new double[angles];
  d->pSinTable = new double[angles];

  double dAngleConversion = M_PI / 180 * d->dAngleResolution;
  double dStartRadians = double(d->iStartAngle) / 180.0 * M_PI;

  // Look-up tables for sin and cos
  for (int omega=0; omega<angles; ++omega)
    {
      d->pSinTable[omega] = sin(double(omega)*dAngleConversion + dStartRadians);
      d->pCosTable[omega] = cos(double(omega)*dAngleConversion + dStartRadians);
    }
}

const double* PiiHoughTransform::sinTable() const { return _d()->pSinTable; }
const double* PiiHoughTransform::cosTable() const { return _d()->pCosTable; }
