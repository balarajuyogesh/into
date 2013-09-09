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

#ifndef _PIIGRADIENTFEATUREOPERATION_H
#define _PIIGRADIENTFEATUREOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * An operation that calculates the maximum vertical and horizontal
 * gradient in a gray-level image.
 *
 * @inputs
 *
 * @in image - input image. Any gray-level image.
 * 
 * @outputs
 *
 * @out features - a 1-by-2 PiiMatrix<float> that contains the maximum
 * absolute value of vertical and horizontal gradient, in this order.
 *
 * @ingroup PiiFeaturesPlugin
 */
class PiiGradientFeatureOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Feature names (read only).
   */
  Q_PROPERTY(QStringList features READ features);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiGradientFeatureOperation();

  QStringList features() const;

protected:
  void process();

private:
  template <class T> void calculateGradient(const PiiVariant& obj);
};


#endif //_PIIGRADIENTFEATUREOPERATION_H
