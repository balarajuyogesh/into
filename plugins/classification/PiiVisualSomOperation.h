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

#ifndef _PIIVISUALSOMOPERATION_H
#define _PIIVISUALSOMOPERATION_H

#include <PiiSomOperation.h>
#include <PiiFrequencyLimiter.h>
#include <PiiQImage.h>

/**
 * PiiVisualSomOperation is a special som classifier. Here is also
 * an image input. So user can control training of the classifier. In
 * the first situation this classifier collects features to the
 * internal buffer. User can train this classifier when there is
 * enough images on the map.
 *
 *
 * Inputs
 * ------
 *
 * @in image - an image (any image type)
 *
 */
class PII_CLASSIFICATION_EXPORT PiiVisualSomOperation : public PiiSomOperation::Template<PiiMatrix<double> >
{
  Q_OBJECT

  /**
   * How many features per second will be emitted to the ui-component.
   * Still all features will be classified and emit forward.
   */
  Q_PROPERTY(double maxFrequency READ maxFrequency WRITE setMaxFrequency);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiVisualSomOperation();
  ~PiiVisualSomOperation();

  void setMaxFrequency(double maxFrequency);
  double maxFrequency();

public slots:
  void removeImage(QImagePtr ptr);
  
signals:
  void removeSubImage(QImagePtr ptr);  
  void removeSubImages(int start);
  void addSubImage(QImagePtr ptr, int classIndex);
  void allSubImagesAdded();

protected:
  void collectSample(double label, double weight);
  double classify();
  bool learnBatch();
  void replaceClassifier();

private:
  typedef PiiSomOperation::Template<PiiMatrix<double> > SuperType;
  void storeImage(const PiiVariant& image, int classification);
  
  /// @internal
  class Data : public PiiSomOperation::Template<PiiMatrix<double> >::Data
  {
  public:
    Data();
    
    PiiInputSocket *pImageInput;

    QList<QImagePtr> lstImages, lstNewImages;
    PiiFrequencyLimiter limiter;
    QMutex storeMutex;
    double dClassification;
    int iFakedClassification;
  };
  PII_D_FUNC;
};


#endif //_PIIVISUALSOMOPERATION_H
