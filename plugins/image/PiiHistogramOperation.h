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

#ifndef _PIIHISTOGRAMOPERATION_H
#define _PIIHISTOGRAMOPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include "PiiImageGlobal.h"
#include "PiiRoi.h"

/**
 * An operation that calculates the histogram of a gray-scale or a
 * color image.
 *
 * Inputs
 * ------
 *
 * @in image - the input image, any gray-scale or color image.
 *
 * @in roi - region-of-interest. See [PiiImagePlugin] for details. 
 * Optional.
 *
 * Outputs
 * -------
 *
 * @out red - the red channel histogram
 *
 * @out green - the green channel histogram
 *
 * @out blue - the blue channel histogram
 *
 * If a gray-scale image is read, the histogram will be sent to all
 * three outputs.
 *
 */
class PiiHistogramOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of distinct levels in the output histogram. If a
   * non-positive number is given, the maximum value of the
   * image/color channel is found and used (after adding one). For 8
   * bit gray-scale images, use 256. The default value is 256.
   */
  Q_PROPERTY(int levels READ levels WRITE setLevels);

  /**
   * Output normalization. If set to `false` (the default), the
   * output will be a PiiMatrix<int> in which each column represents
   * the number of times the specific color was encountered. If 
   * `true`, output histograms will be normalized so that they always
   * sum up to unity and represented by PiiMatrix<float>.
   */
  Q_PROPERTY(bool normalized READ normalized WRITE setNormalized);

  /**
   * The type or the `roi` input, if connected. The default value is
   * `AutoRoi`.
   */
  Q_PROPERTY(PiiImage::RoiType roiType READ roiType WRITE setRoiType);

  PII_OPERATION_SERIALIZATION_FUNCTION;
public:
  PiiHistogramOperation();

protected:
  void process();
  
  void aboutToChangeState(State state);

  void setNormalized(bool normalize);
  bool normalized() const;

  void setRoiType(PiiImage::RoiType roiType);
  PiiImage::RoiType roiType() const;

  int levels() const;
  void setLevels(int levels);

private:
  template <class T> struct GrayHistogram;
  template <class T> struct ColorHistogram;
  template <class T> struct Histogram;

  template <class T> void histogram(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ~Data();
    
    int iLevels;
    PiiInputSocket* pImageInput, *pRoiInput;
    
    bool bNormalized;
    PiiImage::RoiType roiType;
    PiiImage::HistogramHandler* pHistogram;
    unsigned int uiPreviousType;
  };
  PII_D_FUNC;
};

#endif //_PIIHISTOGRAMOPERATION_H
