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

#ifndef _PIITEMPLATEMATCHER_H
#define _PIITEMPLATEMATCHER_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <cv.h>

/**
 * Correlates a template against an image. This operation uses
 * cvMatchTemplate() for matching. It is mostly an illustration on how
 * to integrate OpenCV to Into and not intended for production-grade
 * applications.
 *
 * Inputs
 * ------
 *
 * @in image - the input image. Any gray-scale image.
 *
 * Outputs
 * -------
 *
 * @out correlation - a matrix in which each element denotes the
 * correlation (or difference) between the input image and the
 * template. If `SquaredDiff` or `NormedSquaredDiff` method is used,
 * smaller values mean better matches. If any other method is used,
 * larger values mean better matches. If the size of the input image
 * is R x C, and the size of the template is r x c, the size of the
 * correlation matrix will be R-r+1 x C-c+1. If the input image is
 * smaller than the template, an empty matrix will be emitted.
 * (PiiMatrix<float>)
 *
 * @out mask - a binary matrix in which a mask ([maskFile]) is placed
 * on each correlation peak. If mask has not been set, a matrix with
 * correlation peaks will be emitted. The size of the mask is equal to
 * the size of the input image. (PiiMatrix<bool>)
 *
 */
class PiiTemplateMatcher : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The name of the template file. Set this property to load the
   * image template from a file.
   */
  Q_PROPERTY(QString templateFile READ templateFile WRITE setTemplateFile);
  /**
   * The name of the mask file. Mask is a binary image that describes
   * the interesting parts of template (the ROI). Once the template
   * matching has succeeded, the mask is placed on each peak, and the
   * result is emitted to the `mask` output. Non-zero values in the
   * mask are regarded as `true`. The size of the mask must be equal
   * to the size of the template.
   */
  Q_PROPERTY(QString maskFile READ maskFile WRITE setMaskFile);
  /**
   * Detection threshold. All correlation values above (for
   * correlation-based measurements) or below (for squared diff) this
   * threshold are considered matches. The default is 1.0.
   */
  Q_PROPERTY(double threshold READ threshold WRITE setThreshold);

  /**
   * Matching method. The default is `SquaredDiff`.
   */
  Q_PROPERTY(MatchingMethod matchingMethod READ matchingMethod WRITE setMatchingMethod);
  Q_ENUMS(MatchingMethod);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Different matching methods. Names should stand for themselves.
   * *Diff methods produce small values for good matches. Others
   * produce large values for good matches.
   */
  enum MatchingMethod
  {
    SquaredDiff,
    NormalizedSquaredDiff,
    CrossCorrelation,
    NormalizedCrossCorrelation,
    CorrelationCoeff,
    NormalizedCorrelationCoeff
  };

  PiiTemplateMatcher();
  ~PiiTemplateMatcher();

  void setMatchingMethod(MatchingMethod matchingMethod);
  MatchingMethod matchingMethod() const;
  void setTemplateFile(const QString& templateFile);
  QString templateFile() const;
  void setMaskFile(const QString& maskFile);
  QString maskFile() const;
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

    int iMatchingMethod;
    PiiMatrix<unsigned char> *pTemplate, *pMask;
    double dThreshold;
    QString strTemplateFile;
    QString strMaskFile;
  };
  PII_D_FUNC;

  template <class T> void match(const PiiVariant& obj);
  template <class T> void match(const PiiMatrix<T>& image);
};


#endif //_PIITEMPLATEMATCHER_H
