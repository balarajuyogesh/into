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

#ifndef _PIIGAUSSIANKERNEL_H
#define _PIIGAUSSIANKERNEL_H

/**
 * Gaussian kernel function. The Gaussian kernel is defined as
 * \(k(x,y) = e^-\fraq{||x-y||^2}{2\sigma^2}\), where *x* and *y*
 * are vectors of any dimensionality. It is also known as the radial
 * basis function (RBF) kernel.
 *
 */
template <class FeatureIterator> class PiiGaussianKernel
{
public:
  /**
   * Constructs a new Gaussian kernel function.
   */
  PiiGaussianKernel() : _dSigma(1), _dNormalizer(0.5) {}

  /**
   * Sets the \(\sigma\) of the Gaussian function to *sigma*. This
   * value controls the "width" of the Gaussian bell curve. A large
   * *sigma* means generates a wide, flat curve. As *sigma* approaches
   * zero, the curve will approach the delta function. The default
   * value is 1.
   */
  void setSigma(double sigma)
  {
    _dSigma = sigma;
    _dNormalizer = 1.0/(2*sigma*sigma);
  }

  /**
   * Returns the value of \(\sigma\).
   */
  double sigma() const { return _dSigma; }

  inline double operator() (FeatureIterator sample, FeatureIterator model, int length) const throw()
  {
    return Pii::exp(-Pii::squaredDistanceN(sample, length, model, 0.0) * _dNormalizer);
  }
private:
  double _dSigma, _dNormalizer;
};

#endif //_PIIGAUSSIANKERNEL_H
