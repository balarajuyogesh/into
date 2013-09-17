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

#ifndef _PIIIMAGEFILTEROPERATION_H
#define _PIIIMAGEFILTEROPERATION_H

#include <PiiDefaultOperation.h>
#include <PiiMatrixUtil.h>

/**
 * An operation that implements basic image filtering operations. It
 * supports the most commonly used edge detection and noise reduction
 * filters and also allows one to specify a custom filter.
 *
 * Inputs
 * ------
 *
 * @in image - the image to be filtered. Any image type. For color
 * images, the filter will be applied channel-wise.
 * 
 * Outputs
 * -------
 *
 * @out image - the filtered image. The type of the output image
 * equals that of the input.
 *
 */
class PiiImageFilterOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The name of the image filter. This is an easy way to set the
   * filter. For valid values see PiiImage::PrebuiltFilterType. Use
   * "sobelx" for `SobelXFilter`, "gaussian" for `GaussianFilter`
   * etc. There are two special values not supported by makeFilter():
   *
   * - `median` - a median filter. Median filter is non-linear and
   * cannot be implemented with ordinary correlation masks.
   *
   * - `custom` - [filter] will be used as the filter mask.
   *
   * The default value is "uniform".
   */
  Q_PROPERTY(QString filterName READ filterName WRITE setFilterName);

  /**
   * The size of the filter mask. This property has no effect if
   * [filterName] is set to `custom`. It is also ignored by some
   * filters as described in PiiImage::makeFilter(). Automatically
   * created filters are always square. The default value is 3.
   */
  Q_PROPERTY(int filterSize READ filterSize WRITE setFilterSize);
    
  /**
   * The filter as a matrix. This value is used only if [filterName] is
   * set to "custom".
   *
   * ~~~
   * PiiOperation* op = engine.createOperation("PiiImageFilter");
   * op->setProperty("filter",
   *                 PiiVariant(new PiiMatrix<int>(3, 3,
   *                                                       1, 2, 1,
   *                                                       2, 4, 2,
   *                                                       1, 2, 1));
   * ~~~
   */
  Q_PROPERTY(PiiVariant filter READ filter WRITE setFilter);

  /**
   * The method of handling image borders. The default value is 
   * `ExtendZeros`.
   */
  Q_PROPERTY(ExtendMode borderHandling READ borderHandling WRITE setBorderHandling);
  Q_ENUMS(ExtendMode);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  /**
   * Different ways of extending images in filtering. Copied from
   * PiiMatrixUtil to avoid extensive `moc` hacking.
   *
   * - `ExtendZeros` - pad with zeros
   * - `ExtendReplicate` - replicate the value on the border
   * - `ExtendSymmetric` - symmetrically mirror boundary values
   * - `ExtendPeriodic` - take values from the opposite border
   * - `ExtendNot` - do not extend
   */
  enum ExtendMode { ExtendZeros, ExtendReplicate, ExtendSymmetric, ExtendPeriodic, ExtendNot };

  PiiImageFilterOperation();

  void setFilterName(const QString& filterName);
  QString filterName() const;
  void setFilter(const PiiVariant& filter);
  PiiVariant filter() const;
  void setFilterSize(int filterSize);
  int filterSize() const;
  void setBorderHandling(ExtendMode borderHandling);
  ExtendMode borderHandling() const;

  void check(bool reset);

protected:
  void process();

private:
  enum FilterType { Prebuilt, Median, Custom };

  template <class T> void intGrayFilter(const PiiVariant& obj);
  template <class T> void floatGrayFilter(const PiiVariant& obj);
  template <class T> void intColorFilter(const PiiVariant& obj);
  template <class T> void floatColorFilter(const PiiVariant& obj);
  template <class T> void setCustomFilter(const PiiVariant& obj);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    FilterType filterType;
    QString strFilterName;
    int iFilterSize;
    Pii::ExtendMode borderHandling;
    
    PiiVariant pCustomFilter;
    PiiMatrix<double> matPrebuiltFilter;
    PiiMatrix<double> matCustomFilter;
    // Active filter and its decomposition (is available)
    bool bSeparableFilter;
    PiiMatrix<double> matActiveFilter, matHorzFilter, matVertFilter;
  };
  PII_D_FUNC;

};


#endif //_PIIIMAGEFILTEROPERATION_H
