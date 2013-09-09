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

#ifndef _PIICONTRASTOPERATION_H
#define _PIICONTRASTOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * An operation that calculates the contrast in a local neighborhood
 * in a gray-scale image.
 *
 * @inputs
 *
 * @in image - the input image, any numeric matrix
 *
 * @outputs
 *
 * @out image - the contrast image, data type depends on the @p type
 * property and on the input.
 *
 * @ingroup PiiTexturePlugin
 */
class PiiContrastOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Constrast measurement type.
   */
  Q_PROPERTY(ContrastType type READ type WRITE setType);
  Q_ENUMS(ContrastType);
  /**
   * Local neighborhood radius. The local contrast is calculated for
   * each pixel in a neighborhood that reaches @p radius pixels from
   * the center.
   */
  Q_PROPERTY(int radius READ radius WRITE setRadius);
  
  PII_OPERATION_SERIALIZATION_FUNCTION;
public:
  PiiContrastOperation();
  /**
   * Contrast calculation modes.
   *
   * @lip MaxDiff - maximum absolute difference between the center and
   * a neighbor. The output is of the same type as the input.
   *
   * @lip LocalVar - contrast calculated as the variance of gray
   * levels in a local neighborhood. The output is PiiMatrix<float>.
   */
  enum ContrastType { MaxDiff, LocalVar };

protected:
  void process();

  ContrastType type() const;
  void setType(ContrastType type);
  int radius() const;
  void setRadius(int radius);

private:
  template <class T> void contrast(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ContrastType type;
    int iRadius;
    PiiInputSocket* pImageInput;
    PiiOutputSocket* pImageOutput;
  };
  PII_D_FUNC;
};

#endif //_PIICONTRASTOPERATION_H
