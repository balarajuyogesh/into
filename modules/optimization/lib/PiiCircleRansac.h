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

#ifndef _PIICIRCLERANSAC_H
#define _PIICIRCLERANSAC_H

#include "PiiRansac.h"
#include "PiiOptimization.h"

#include <PiiVector.h>
#include <PiiPseudoInverse.h>

/**
 * A RANSAC estimator that finds the parameters of a circle that best
 * matches a set of points. The circle is parametrized by its center
 * (x, y) and squared radius (r²). As the goodness of fit, squared
 * geometric distance from the circle is used.
 */
template <class T> class PiiCircleRansac :
  public PiiRansac
{
public:
  /**
   * Constructs a new RANSAC estimator with no points to match. You
   * need to set the points to match with [setPoints()] before calling
   * [findBestModel()](PiiRansac::findBestModel()) or use the
   * [findBestModel(const PiiMatrix<T>&, const PiiMatrix<T>&)]
   * function.
   */
  PiiCircleRansac();

  /**
   * Constructs a new RANSAC estimator that matches *points* to a
   * circle equation.
   */
  PiiCircleRansac(const PiiMatrix<T>& points);

  /**
   * Sets the points to which the circle is to be matched.
   */
  void setPoints(const PiiMatrix<T>& points);

  /// @internal
  inline bool findBestModel() { return PiiRansac::findBestModel(); }

  /**
   * Runs the RANSAC algorithm to find a transformation that maps
   * a circle equation to *points*.
   *
   * @see PiiRansac::findBestModel()
   */
  bool findBestModel(const PiiMatrix<T>& points);

  /**
   * Returns either the model estimated by RANSAC or a geometrically
   * refined, presumably better estimate, depending on the value of
   * the [autoRefine()] flag.
   */
  PiiMatrix<double> bestModel() const;

  /**
   * Refines the rough model estimate given by the standard RANSAC
   * algorithm. This function uses a iterative non-linear optimization
   * technique (Levenberg-Marquardt) to find the model that best
   * matches the found inlying points in geometrical sense. The
   * function returns refined model parameters as a 1-by-4 matrix, or
   * an empty matrix if findBestModel() has not found a suitable
   * model.
   */
  PiiMatrix<double> refineModel() const;

  /**
   * Enables or disables automatic geometric refinement of the best
   * model. If automatic refinement is enabled, the [bestModel()]
   * function returns [refineModel()] instead of the probably slightly
   * more inaccurate default estimate. By default, automatic
   * refinement is disabled.
   */
  void setAutoRefine(bool autoRefine);
  /**
   * Returns `true` if automatic geometric refinement of the best
   * model is enabled, `false` otherwise.
   */
  bool autoRefine() const;
  /**
   * Sets the minimum accepted radius. The default value is 0.
   */
  void setMinRadius(double minRadius);
  double minRadius() const;
  /**
   * Sets the maximum accepted scaling factor. Works analogously to
   * [setMinRadius()]. The default value is 2.
   */
  void setMaxRadius(double maxRadius);
  /**
   * Returns the maximum accepted radius factor.
   */
  double maxRadius() const;

  /**
   * Makes the fitting threshold relative to circle radius. By
   * default, PiiRansac uses a fixed fitting threshold. If this value
   * is set to a value other than NAN, the fitting threshold will
   * change depending on the radius of the estimated circle. If the
   * radius of the circle is r, a point is considered an inlier if its
   * distance to the circle's circumfence is at most *factor* * r. The
   * default value is NAN.
   */
  void setRelativeFittingThreshold(double factor);
  double relativeFittingThreshold() const;

  /**
   * Makes the number of required inliers relative to the circumfence
   * of the circle being estimated. Setting this value to NAN will
   * cause the fixed default value set by PiiRansac::setMinInliers()
   * to be used. Otherwise, there needs to be at least 2 * pi * r *
   * *factor* inlying points before a circle is accepted. The default
   * value is NAN.
   */
  void setRelativeMinInliers(double factor);
  double relativeMinInliers() const;

protected:
  /**
   * Returns the number of rows in the given point set.
   */
  int totalSampleCount() const;

  /**
   * Returns three. Solving circle parameters requires three
   * non-colinear points.
   */
  int minSamples() const;

  /**
   * Always returns a 1-by-3 matrix representing the parameters of a
   * single matching model. If the three points are colinear, an empty
   * matrix will be returned.
   */
  PiiMatrix<double> findPossibleModels(const int* dataIndices);

  /**
   * Returns the squared geometric distance between the point at
   * *dataIndices* and the circle represented by the given *model*.
   */
  double fitToModel(int dataIndex, const double* model);

  int minInliers(const double* model) const;
  double fittingThreshold(const double* model) const;

private:
  class Data : public PiiRansac::Data
  {
  public:
    Data() :
      bAutoRefine(false),
      dMinRadius(0),
      dMaxRadius(INFINITY),
      dRelativeFittingThreshold(NAN),
      dRelativeMinInliers(NAN)
    {}

    Data(const PiiMatrix<T>& points) :
      matPoints(points),
      bAutoRefine(false),
      dMinRadius(0),
      dMaxRadius(INFINITY),
      dRelativeFittingThreshold(NAN),
      dRelativeMinInliers(NAN)
    {}

    PiiMatrix<T> matPoints;
    bool bAutoRefine;
    double dMinRadius;
    double dMaxRadius;
    double dRelativeFittingThreshold;
    double dRelativeMinInliers;
  };
  PII_D_FUNC;
};

template <class T>
PiiCircleRansac<T>::PiiCircleRansac() :
  PiiRansac(new Data)
{}

template <class T>
PiiCircleRansac<T>::PiiCircleRansac(const PiiMatrix<T>& points) :
  PiiRansac(new Data(points))
{}

template <class T>
void PiiCircleRansac<T>::setPoints(const PiiMatrix<T>& points)
{
  _d()->matPoints = points;
}

template <class T>
bool PiiCircleRansac<T>::findBestModel(const PiiMatrix<T>& points)
{
  setPoints(points);
  return PiiRansac::findBestModel();
}

template <class T>
PiiMatrix<double> PiiCircleRansac<T>::bestModel() const
{
  return _d()->bAutoRefine ? refineModel() : PiiRansac::bestModel();
}

template <class T>
PiiMatrix<double> PiiCircleRansac<T>::refineModel() const
{
  const PII_D;
  PiiMatrix<double> matBestModel(PiiRansac::bestModel());
  if (matBestModel.isEmpty())
    return PiiMatrix<double>();

  /* Fit the circle to the found inliers using a least squares
   * estimation technique.
   *
   * The non-linear function we are trying to optimize is
   * f(cx, cy, r) = (cx - x)² + (cy - y)² - r²
   * = cx² - 2x cx + x² + cy² - 2y cy + y² - r² = 0
   *
   * The best model given by RANSAC is pretty close already, so we
   * should be fine with 1st order Taylor expansion, which in a
   * multi-variate case is f(X) = f(Xi) + Df(Xi)(X - Xi), where Df is
   * a derivative (Jacobian) matrix.
   *
   * Partial derivatives:
   * ðf/ðcx = 2cx - 2x
   * ðf/ðcy = 2cy - 2y
   * ðf/ðr² = -2r
   *
   * Putting this together and marking the known initial values for
   * cx, cy and r with "i" for "initial" we have:
   *
   * f(X) = f(Xi) + [ 2cxi - 2x, 2cyi - 2y, -2ri ](X - Xi)
   *      = f(Xi) + 2 (cxi - x) (cx - cxi) + 2 (cyi - y) (cy - cyi) - 2 ri (r - ri)
   *      = f(Xi) + 2     dx    (cx - cxi) + 2    dy     (cy - cyi) - 2 ri r + 2 ri²
   *      = f(Xi) + 2 dx cx - 2 dx cxi + 2 dy cy - 2 dy cyi - 2 ri r + 2 ri²
   *      = 2 dx cx + 2 dy cy - 2 ri r + f(Xi) - 2 dx cxi - 2 dy cyi + 2 ri² = 0
   *     => [2 dx, 2 dy, -2 ri] [cx, cy, r]' = 2 dx cxi + 2 dy cyi - 2 ri² - f(Xi)
   *                                         = 2 dx cxi + 2 dy cyi - 2 ri² - (cxi - x)² - (cyi - y)² + ri²
   *                                         = 2 dx cxi + 2 dy cyi - ri² - dx² - dy²
   *
   * This is a linear estimation problem of the form AX = Y, for which
   * an optimal (in LS sense) solution is given by the Moore-Penrose
   * pseudoinverse: X = A⁺Y. (Transposed: X' = Y'A⁺')
   */

  const int iCnt = d->matPoints.template rows();
  PiiMatrix<double> matA(0, 3), matY(1, iCnt);
  matA.reserve(iCnt);
  double cxi = matBestModel(0, 0), cyi = matBestModel(0, 1), ri = matBestModel(0, 2);
  for (int i = 0; i < iCnt; ++i)
    {
      double x = d->matPoints(i, 0), y = d->matPoints(i, 1);
      double dx = cxi - x, dy = cyi - y;
      matA.appendRow(2 * dx, 2 * dy, -2 * ri);
      matY(0, i) = 2 * dx * cxi + 2 * dy * cyi - ri * ri - dx * dx - dy * dy;
    }
  return matY * Pii::transpose(Pii::pseudoInverse(matA));
}

template <class T> bool PiiCircleRansac<T>::autoRefine() const { return _d()->bAutoRefine; }
template <class T> void PiiCircleRansac<T>::setAutoRefine(bool autoRefine) { _d()->bAutoRefine = autoRefine; }
template <class T> int PiiCircleRansac<T>::totalSampleCount() const { return _d()->matPoints.rows(); }
template <class T> int PiiCircleRansac<T>::minSamples() const { return 3; }

template <class T>
PiiMatrix<double> PiiCircleRansac<T>::findPossibleModels(const int* dataIndices)
{
  PII_D;

  // Use the first point as the origin and translate the other two
  // accordingly.
  PiiVector<T, 2>
    a = d->matPoints.template rowAs<PiiVector<T, 2> >(dataIndices[0]),
    b = d->matPoints.template rowAs<PiiVector<T, 2> >(dataIndices[1]) - a,
    c = d->matPoints.template rowAs<PiiVector<T, 2> >(dataIndices[2]) - a;

  // Calculate the circumcenter origin of the triangle.
  double dd = 2 * (b[0] * c[1] - b[1] * c[0]);
  // Degenerate case: three colinear points
  if (Pii::almostEqualRel(dd, 0.0))
    return PiiMatrix<double>();

  double b2 = b[0] * b[0] + b[1] * b[1], c2 = c[0] * c[0] + c[1] * c[1];
  // Circle parameters
  double dCx = (c[1] * b2 - b[1] * c2) / dd;
  double dCy = (b[0] * c2 - c[0] * b2) / dd;
  double dRadius = Pii::fastHypotenuse(b[0] - dCx, b[1] - dCy);
  if (dRadius < d->dMinRadius || dRadius > d->dMaxRadius)
    return PiiMatrix<double>();

  return PiiMatrix<double>(1, 3,
                           dCx + a[0],
                           dCy + a[1],
                           dRadius);
}

template <class T>
double PiiCircleRansac<T>::fitToModel(int dataIndex, const double* model)
{
  const T* pData = _d()->matPoints[dataIndex];
  return Pii::abs(Pii::fastHypotenuse(pData[0] - model[0],
                                      pData[1] - model[1]) -
                  model[2]);
}

template <class T>
int PiiCircleRansac<T>::minInliers(const double* model) const
{
  const PII_D;
  if (Pii::isNan(d->dRelativeMinInliers))
    return d->iMinInliers;
  return int(2 * M_PI * model[2] * d->dRelativeMinInliers + 0.5);
}

template <class T>
double PiiCircleRansac<T>::fittingThreshold(const double* model) const
{
  const PII_D;
  if (Pii::isNan(d->dRelativeFittingThreshold))
    return d->dFittingThreshold;
  return model[2] * d->dRelativeFittingThreshold;
}

template <class T> void PiiCircleRansac<T>::setMinRadius(double minRadius) { _d()->dMinRadius = minRadius; }
template <class T> double PiiCircleRansac<T>::minRadius() const { return _d()->dMinRadius; }
template <class T> void PiiCircleRansac<T>::setMaxRadius(double maxRadius) { _d()->dMaxRadius = maxRadius; }
template <class T> double PiiCircleRansac<T>::maxRadius() const { return _d()->dMaxRadius; }
template <class T> void PiiCircleRansac<T>::setRelativeFittingThreshold(double factor) { _d()->dRelativeFittingThreshold = factor; }
template <class T> double PiiCircleRansac<T>::relativeFittingThreshold() const { return _d()->dRelativeFittingThreshold; }
template <class T> void PiiCircleRansac<T>::setRelativeMinInliers(double factor) { _d()->dRelativeMinInliers = factor; }
template <class T> double PiiCircleRansac<T>::relativeMinInliers() const { return _d()->dRelativeMinInliers; }

#endif //_PIICIRCLERANSAC_H
