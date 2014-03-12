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

#ifndef _PIILABELINGOPERATION_H
#define _PIILABELINGOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include "PiiImageGlobal.h"

/**
 * Basic labeling operations.
 *
 * Inputs
 * ------
 *
 * @in image - the input image. Binary image. If the image is not
 * binary, it will be automatically thresholded. (Any gray-level image
 * type.)
 *
 * Outputs
 * -------
 *
 * @out image - the labeled image. A PiiMatrix<int> type in which
 * objects are marked with increasing ordinal numbers 1,2,...,N. The
 * background will be zero.
 *
 * @out labels - the number of distinct objects in the input image.
 * (int)
 *
 */
class PiiLabelingOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Connectivity type for labeling. The default is `Connect4`.
   */
  Q_PROPERTY(PiiImage::Connectivity connectivity READ connectivity WRITE setConnectivity);

  /**
   * A static threshold value for automatic thresholding. Every pixel
   * whose gray level is higher than this value will be considered an
   * object. The default value is zero.
   */
  Q_PROPERTY(double threshold READ threshold WRITE setThreshold);

  /**
   * A hysteresis for the thresholding. If this value is non-zero,
   * `threshold` will decide the minimum value for a gray level that can
   * be a "seed" for a connected object. A all pixels surrounding the
   * seed that are brighter than `threshold` - `hysteresis` are
   * joined to the connected component.
   */
  Q_PROPERTY(double hysteresis READ hysteresis WRITE setHysteresis);

  /**
   * A flag that can be used to invert the automatic thresholding. If
   * this value is set to `true`, dark objects will be considered
   * targets. The default value is `false`.
   */
  Q_PROPERTY(bool inverse READ inverse WRITE setInverse);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiLabelingOperation();

  void setConnectivity(PiiImage::Connectivity connectivity);
  PiiImage::Connectivity connectivity() const;
  void setThreshold(double threshold);
  double threshold() const;
  void setHysteresis(double hysteresis);
  double hysteresis() const;
  void setInverse(bool inverse);
  bool inverse() const;

protected:
  void process();

private:
  template <class T> void operate(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiImage::Connectivity connectivity;

    PiiInputSocket* pBinaryImageInput;
    PiiOutputSocket* pLabeledImageOutput;
    PiiOutputSocket* pLabelsOutput;
    double dThreshold;
    double dHysteresis;
    bool bInverse;
  };
  PII_D_FUNC;
};


#endif //_PIILABELINGOPERATION_H
