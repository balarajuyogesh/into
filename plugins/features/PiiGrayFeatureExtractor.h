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

#ifndef _PIIGRAYFEATUREEXTRACTOR_H
#define _PIIGRAYFEATUREEXTRACTOR_H

// This file is intended to be included from
// PiiGrayFeatureOperation.h. The extractors were moved here to keep
// the operation's code clean.

// @internal
// Interface for optimized feature extractors
class PiiGrayFeatureOperation::FeatureExtractor
{
public:
  virtual ~FeatureExtractor() {}
  virtual void process() = 0;

protected:
  FeatureExtractor(PiiInputSocket* input, PiiOutputSocket* output) : _pInput(input), _pOutput(output) {}

  PiiInputSocket* _pInput;
  PiiOutputSocket* _pOutput;
};


/************************************ Optimized feature extractors *********************************/

class PiiGrayFeatureOperation::MinimumExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MinimumExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input,output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    _pOutput->emitObject(PiiMatrix<float>(1, 1, double(Pii::min(obj.valueAs<PiiMatrix<T> >()))));
  }
};

class PiiGrayFeatureOperation::MaximumExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MaximumExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    _pOutput->emitObject(PiiMatrix<float>(1, 1, double(Pii::max(obj.valueAs<PiiMatrix<T> >()))));
  }
};

class PiiGrayFeatureOperation::MinMaxExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MinMaxExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    T min, max;
    Pii::minMax(obj.valueAs<PiiMatrix<T> >(), &min, &max);
    _pOutput->emitObject(PiiMatrix<float>(1, 2, double(min), double(max)));
  }
};

class PiiGrayFeatureOperation::AverageExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  AverageExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    _pOutput->emitObject(PiiMatrix<float>(1, 1, Pii::mean<double>(obj.valueAs<PiiMatrix<T> >())));
  }
};

class PiiGrayFeatureOperation::MinAvgExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MinAvgExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
    float sum = 0.0;
    float minValue = std::numeric_limits<float>::max();
    for (int r=img.rows(); r--; )
      {
        const T* row = img[r];
        for (int c=img.columns(); c--; )
          {
            if (row[c] < minValue)
              minValue = row[c];
            sum += row[c];
          }
      }
    _pOutput->emitObject(PiiMatrix<float>(1, 2, double(minValue), double(sum/(img.rows()*img.columns()))));
  }
};

class PiiGrayFeatureOperation::MaxAvgExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MaxAvgExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
    float sum = 0.0;
    float maxValue = -std::numeric_limits<float>::max();
    for (int r=img.rows(); r--; )
      {
        const T* row = img[r];
        for (int c=img.columns(); c--; )
          {
            if (row[c] > maxValue)
              maxValue = row[c];
            sum += row[c];
          }
      }
    _pOutput->emitObject(PiiMatrix<float>(1, 2, double(maxValue), double(sum/(img.rows()*img.columns()))));
  }
};

class PiiGrayFeatureOperation::MinMaxAvgExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  MinMaxAvgExtractor(PiiInputSocket* input, PiiOutputSocket* output) : FeatureExtractor(input, output) {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj)
  {
    const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
    int imgSize = img.rows() * img.columns();
    if (imgSize == 0)
      _pOutput->emitObject(PiiMatrix<float>(1, 3));
    float sum = 0.0;
    float minValue = img(0,0), maxValue = minValue;
    for (int r=img.rows(); r--; )
      {
        const T* row = img[r];
        for (int c=img.columns(); c--; )
          {
            if (row[c] < minValue)
              minValue = row[c];
            if (row[c] > maxValue)
              maxValue = row[c];
            sum += row[c];
          }
      }
    _pOutput->emitObject(PiiMatrix<float>(1, 3, double(minValue), double(maxValue), double(sum/(img.rows()*img.columns()))));
  }
};


class PiiGrayFeatureOperation::DefaultExtractor : public PiiGrayFeatureOperation::FeatureExtractor
{
public:
  DefaultExtractor(PiiGrayFeatureOperation* parent) :
    FeatureExtractor(parent->inputAt(0), parent->outputAt(0)),
    _pParent(parent)
  {}
  
  void process()
  {
    PiiVariant obj = _pInput->firstObject();
    switch (obj.type())
      {
        PII_GRAY_IMAGE_CASES(extractFeatures,obj);
      default:
        PII_THROW_UNKNOWN_TYPE(_pInput);
      }
  }
  template <class T> void extractFeatures(const PiiVariant& obj);

private:
  PiiGrayFeatureOperation* _pParent;
};

template <class T> void PiiGrayFeatureOperation::DefaultExtractor::extractFeatures(const PiiVariant& obj)
{
  const PiiMatrix<T> img = obj.valueAs<PiiMatrix<T> >();
  int imgSize = img.rows() * img.columns();
  if (imgSize == 0)
    _pOutput->emitObject(PiiMatrix<float>(1, _pParent->enabledFeatures().size()));

  // Always calculate minimum, maximum, and average
  float features[PII_GRAY_FEATURE_OPERATION_FEATURECNT];
  features[0] = features[1] = img(0,0); //min, max
  features[2] = 0; // avg
  features[5] = -1; // std
  const int iRows = img.rows(), iCols = img.columns();
  for (int r=0; r<iRows; ++r)
    {
      const T* row = img.row(r);
      for (int c=0; c<iCols; ++c)
        {
          T val = row[c];
          if (val > features[1])
            features[1] = val;
          else if (val < features[0])
            features[0] = val;
          features[2] += val;
        }
    }
  features[2] /= imgSize; // average
  features[3] = features[1] - features[0]; // contrast
  features[4] = Pii::max(Pii::abs(_pParent->reference() - features[0]),
                             Pii::abs(_pParent->reference() - features[1])); // maximum difference to reference


  QList<int> lstEnabledFeatures = _pParent->enabledFeatures();
  PiiMatrix<float> matResult(1, lstEnabledFeatures.size());
  for (int f=0; f<lstEnabledFeatures.size(); f++)
    {
      int feature = lstEnabledFeatures[f];
      // complex features are calculated only on demand
      if (feature == 5 || feature == 6)
        {
          if (features[5] == -1)
            {
              features[5] = 0;
              for (int r=0; r<iRows; ++r)
                {
                  const T* row = img.row(r);
                  for (int c=0; c<iCols; ++c)
                    {
                      float diff = row[c] - features[2];
                      features[5] += diff * diff;
                    }
                }
              features[5] /= imgSize;
            }
          
          if (feature == 6)
            features[6] = std::sqrt(features[5]);
        }
      else if (feature == 7) // high average
        {
          features[7] = Pii::forEachIf(img.begin(),img.end(),
                                       std::bind2nd(std::greater<float>(), features[2]),
                                       Pii::Mean<float>()).mean();
          
        }
      else if (feature == 8) // low average
        {
          features[8] = Pii::forEachIf(img.begin(),img.end(),
                                       std::bind2nd(std::less<float>(), features[2]),
                                       Pii::Mean<float>()).mean();
        }
      
      matResult(0, f) = features[feature];
    }
  
  // Send calculated features
  _pOutput->emitObject(matResult);
}


#endif //_PIIGRAYFEATUREEXTRACTOR_H
