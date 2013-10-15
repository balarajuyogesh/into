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

#ifndef _PIIDWTOPERATION_H
#define _PIIDWTOPERATION_H

#include <PiiDefaultOperation.h>
#include "PiiWavelet.h"

/**
 * Discrete wavelet decomposition in two dimensions.
 *
 * Inputs
 * ------
 *
 * @in input - input matrix. Any integer or floating-point matrix.
 * 
 * Outputs
 * -------
 *
 * @out approximation - approximation coefficients (the result of
 * low-pass filtering in both dimensions)
 *
 * @out horizontal - horizontal details (horizontal high-pass,
 * vertical low-pass)
 * 
 * @out vertical - vertical details (vertical high-pass, horizontal
 * low-pass)
 *
 * @out diagonal - diagonal details (high-pass in both dimensions)
 *
 */
class PiiDwtOperation : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * The family of wavelets used in the decomposition. The default is
   * `Haar`.
   */
  Q_PROPERTY(WaveletFamily waveletFamily READ waveletFamily WRITE setWaveletFamily);
  Q_ENUMS(WaveletFamily);
  
  /**
   * The index of the wavelet within the chosen family. This value is
   * ignored for the `Haar` "family". The operation supports members
   * 1-10 of the `Daubechies` family. Note that `Daubechies` 1 is
   * equal to `Haar`. The default value is 1.
   */
  Q_PROPERTY(int familyMember READ familyMember WRITE setFamilyMember);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Known wavelet families. Families currently known are:
   *
   * - Haar - the Haar wavelet. Not actually a family, but equal to
   * the first Daubechies wavelet.
   *
   * - Daubechies - Daubechies wavelet family
   */
  enum WaveletFamily { Haar, Daubechies };
  PiiDwtOperation();

  void setWaveletFamily(const WaveletFamily& waveletFamily);
  WaveletFamily waveletFamily() const;
  void setFamilyMember(int familyMember);
  int familyMember() const;

protected:
  void process();

private:
  template <class T> void floatTransform(const PiiVariant& obj);
  template <class T> void integerTransform(const PiiVariant& obj);
  template <class T> void transform(const PiiMatrix<T>& mat);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    
    WaveletFamily waveletFamily;
    int iFamilyMember;
  };
  PII_D_FUNC;
};


#endif //_PIIDWTOPERATION_H
