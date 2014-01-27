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

#ifndef _PIICORNERDETECTOR_H
#define _PIICORNERDETECTOR_H

#include <PiiDefaultOperation.h>

/**
 * Detects corners in gray-level images using the FAST corner
 * detector.
 *
 * Inputs
 * ------
 *
 * @in image - a gray-level input image
 *
 * Outputs
 * -------
 *
 * @out corners - corner coordinates, a N-by-2 PiiMatrix<int> in which
 * each row stores the (x,y) coordinates of a detected corner.
 *
 */
class PiiCornerDetector : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Corner detection threshold. See
   * [PiiImage::detectFastCorners()].
   */
  Q_PROPERTY(double threshold READ threshold WRITE setThreshold);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiCornerDetector();

  void setThreshold(double threshold);
  double threshold() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dThreshold;
  };
  PII_D_FUNC;

  template <class T> void detectCorners(const PiiVariant& obj);
};

#endif //_PIICORNERDETECTOR_H
