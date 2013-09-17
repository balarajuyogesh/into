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

#ifndef _PIISPECTRALPEAKDETECTOR_H
#define _PIISPECTRALPEAKDETECTOR_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <PiiFft.h>

/**
 * Find repeating components in textures. This operation uses the FFT
 * to find repeating components in a surface texture. It computes the
 * locations of the peaks and the wavelengths and orientations of the
 * corresponding waves in the image.
 *
 * Inputs
 * ------
 *
 * @in image - Any gray-level image.
 * 
 * Outputs
 * -------
 *
 * @out peaks - detected peaks. A N-by-5 (possibly empty) matrix in
 * which each row represents a detected peak. The stored values are
 * (in this order): horizontal location of the peak in the spectrum in
 * pixels, vertical location, relative strength of the peak, wave
 * length in pixels, and angle with respect to the y axis in radians. 
 * The angle is measured clockwise. (PiiMatrix<double>)
 *
 * @out composition - an image composed of the detected peaks. This
 * image illustrates the main frequency components present in the
 * image. (PiiMatrix<float>)
 *
 */
class PiiSpectralPeakDetector : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Detection threshold. Assume \(P(\omega)\) is the power spectrum
   * of the input image whose size is \(w \times h\). The threshold
   * \(t\) is compared to the normalized square root of the power
   * spectrum, and a peaks must match the following rule:
   *
   * \[
   * \frac{\sqrt{P(\omega)}}{w h} \ge t
   * \]
   *
   * The default value is 0.4.
   */
  Q_PROPERTY(double peakThreshold READ peakThreshold WRITE setPeakThreshold);
  /**
   * The minimum wavelength accepted (in pixels). Use this and the
   * [maxWaveLength] property to control the range of accepted
   * wavelengths. Frequency components outside of the accepted range
   * will be ignored. The default value is 0.
   */
  Q_PROPERTY(double minWaveLength READ minWaveLength WRITE setMinWaveLength);
  /**
   * The maximum wavelength accepted (in pixels). Use this and the
   * [minWaveLength] property to control the range of accepted
   * wavelengths. Frequency components outside of the accepted range
   * will be ignored. The default value is infinity.
   */
  Q_PROPERTY(double maxWaveLength READ maxWaveLength WRITE setMaxWaveLength);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiSpectralPeakDetector();

  void check(bool reset);

  void setPeakThreshold(double peakThreshold);
  double peakThreshold() const;
  void setMinWaveLength(double minWaveLength);
  double minWaveLength() const;
  void setMaxWaveLength(double maxWaveLength);
  double maxWaveLength() const;

protected:
  void process();

private:
  template <class T> void findPeaks(const PiiVariant& obj);
  void markPeak(PiiMatrix<float>& powerSpectrum,
                int row, int column, double aspectRatio,
                PiiMatrix<double>& peaks);
  
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiFft<float> fft;
    double dPeakThreshold;
    bool bCompositionConnected;
    double dMinWaveLength;
    double dMaxWaveLength;
  };
  PII_D_FUNC;
};


#endif //_PIISPECTRALPEAKDETECTOR_H
