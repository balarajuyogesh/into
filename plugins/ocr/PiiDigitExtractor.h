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

#ifndef _PIIDIGITEXTRACTOR_H
#define _PIIDIGITEXTRACTOR_H

#include <PiiDefaultOperation.h>
#include <PiiKnnClassifier.h>

/**
 * Recognizes hand-written digits in gray-level images.
 *
 * The operation first segments out individual digits and scales them
 * to a fixed size (20-by-20). Then, PCA is used to reduce the
 * original dimensionality of the image (400) to 50. The PCA
 * coefficients are used as the feature vector, which is classified
 * using a k-NN classifier.
 *
 * Inputs
 * ------
 *
 * @in image - a gray-level input image. There can be multiple digits
 * in one image, but they are assumed to be on the same row. All
 * digits should be written in dark (i.e. black) pen on lighter
 * background (i.e. white paper).
 *
 * Outputs
 * -------
 *
 * @out number - the recognized number as an integer
 *
 * @out digitstring - a concatenated string of found digits
 *
 * @out digits - a 1xN row matrix storing each found digit
 *
 * @out boundingboxes - an Nx4 PiiMatrix<int> storing locations of
 * each found digit in bounding box format (x, y, width, height)
 *
 */
class PiiDigitExtractor: public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * inverse description
   */
  Q_PROPERTY(bool inverse READ inverse WRITE setInverse);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiDigitExtractor();

  void check(bool reset);

protected:
  void process();

  void setInverse(bool inverse);
  bool inverse() const;

private:
  template <class T> void extractIntDigits(const PiiVariant& obj);
  template <class T> void extractDigits(const PiiMatrix<T>& image);
  void createBlob(PiiMatrix<int> &boundingBoxes);
  bool isIncorrectBlob(const PiiMatrix<int> &boundingBoxes, int index,int imageHeight);
  void initializeKnnClassifier();
  int classify(const PiiMatrix<int> &scaledImage);

  /// @internal
  class Data : public PiiDefaultOperation::Data
    {
    public:
      Data();
      PiiInputSocket *pImageInput;
      PiiOutputSocket *pNumberOutput, *pDigitsInStringOutput, *pDigitsOutput, *pBoundingBoxesOutput;
      int iCombineYThreshold;
      int iCombineXThreshold;
      double dSizeThreshold;
      double dLocationThreshold;
      PiiKnnClassifier<PiiMatrix<float> > digitClassifier;
      PiiMatrix<float> matMeanDigitVector; //(400,1);
      PiiMatrix<float> matBaseDigitVectors; //(50,400);
      bool bInverse;
    };
  PII_D_FUNC;
};


#endif // _PIIDIGITEXTRACTOR_H
