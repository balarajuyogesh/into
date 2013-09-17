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

#ifndef _PIICOLORPERCENTILES_H
#define _PIICOLORPERCENTILES_H

#include <PiiDefaultOperation.h>
#include <QPair>
#include <PiiMatrix.h>
#include <PiiImage.h>

class QStringList;
template <class T> class QList;
class QString;

/**
 * An operation that calculates percentiles from color histograms.
 *
 * Inputs
 * ------
 *
 * @in image - a color image the percentiles are extracted from. 
 * Accepts all image types. For gray-scale images, the color channels
 * are ignored. For example, the 90% percentile for any color channel
 * (R:90, B:90, G:90) is the same.
 *
 * @in roi - region-of-interest. See [PiiRoi] for a description. 
 * Optional.
 *
 * Outputs
 * -------
 *
 * @out features - a feature vector that contains the extracted
 * percentiles in the defined order. (PiiMatrix<int>)
 *
 */
class PiiColorPercentiles : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The extracted percentiles as a list of strings. The format for
   * each percentile is C:P[-C:P], where C is the color
   * channel (R, G, or B, or more generally 0-2), and P is the
   * percentile value as an integer in the range 0-100. R:50 and 0:50
   * both denote the 50% percentile of the red color channel.
   *
   * Optionally, a difference of percentiles may be used. For example,
   * R:99-G:1 means the difference between the 99% percentile of red
   * and the 1% percentile of green.
   *
   * The feature vector emitted by the operation contains the given
   * percentiles in the defined order.
   */
  Q_PROPERTY(QStringList percentiles READ percentiles WRITE setPercentiles);

  /**
   * Set a type of the feature set.
   */
  Q_PROPERTY(FeatureSetType featureSetType READ featureSetType WRITE setFeatureSetType);
  Q_ENUMS(FeatureSetType);

  /**
   * Set an universal feature set.
   */
  Q_PROPERTY(UniversalFeatureSet universalFeatureSet READ universalFeatureSet WRITE setUniversalFeatureSet);
  Q_ENUMS(UniversalFeatureSet);

  /**
   * Set a prebuilt feature set.
   */
  Q_PROPERTY(PrebuiltFeatureSet prebuiltFeatureSet READ prebuiltFeatureSet WRITE setPrebuiltFeatureSet);
  Q_ENUMS(PrebuiltFeatureSet);
  
  /**
   * The number of quantization levels for the input images. If a
   * non-positive number is given, the maximum value of the
   * image/color channel is found and used (after adding one). For 8
   * bit gray-scale images, use 256. The default value is 256.
   */
  Q_PROPERTY(int levels READ levels WRITE setLevels);

  /**
   * The type or the `roi` input, if connected. The default value is
   * `AutoRoi`.
   */
  Q_PROPERTY(PiiImage::RoiType roiType READ roiType WRITE setRoiType);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:

  /**
   * Possible choices for a feature set.
   *
   * - `Universal` - uniformly sampled percentiles, either absolute
   * or differential.
   *
   * - `Prebuilt` - prebuilt application-specific sets.
   *
   * - `Custom` - user-defined custom percentiles (see [percentiles]).
   */
  enum FeatureSetType { Universal, Prebuilt, Custom };

  /**
   * An enumeration for prebuilt sets of RGB percentiles.
   *
   * - `WoodDefectDetection` - 13 percentiles for defect detection
   * in wood inspection.
   *
   * - `WoodDefectRecognition` - 15 percentiles for defect recognition
   * (classification) in wood inspection
   */
  enum PrebuiltFeatureSet { WoodDefectDetection, WoodDefectRecognition };

  
                    
  /**
   * Universal sets percentiles.
   *
   * - `Absolute10` - absolute differences from each color channels
   * in 10 percentage unit steps (10, 20, ..., 90). 27 percentiles in
   * total.
   *
   * - `Absolute20` - absolute differences from each color channels
   * in 20 percentage unit steps (20, 40, 60, 80). 12 percentiles in
   * total.
   *
   * - `Absolute30` - absolute differences from each color channels
   * in 30 percentage unit steps (30, 60, 90). 9 percentiles in total.
   *
   * - `Difference10` - intra-channel percentile differences for
   * each color channel in 10 percentage unit steps. (10-1, 20-1, ...,
   * 90-1). 27 percentiles in total.
   *
   * - `Difference20` - intra-channel percentile differences for
   * each color channel in 20 percentage unit steps. (20-10, 40-10,
   * 60-10, 80-10). 12 percentiles in total.
   *
   * - `Difference30` - intra-channel percentile differences for
   * each color channel in 30 percentage unit steps. (30-10, 60-10,
   * 90-10). 9 percentiles in total.
   *
   * - {Absolute,Difference}[123]0[RGB] (e.g. `Absolute10G` or 
   * `Difference30B`) - one-channel versions of the above. Percentiles
   * are calculated from the specified color channel only. The length
   * of the feature vector is one third of the corresponding
   * three-channel version. These features are useful for gray-scale
   * images and when one needs to discard two of the color channels.
   */
  enum UniversalFeatureSet { Absolute10, Absolute20, Absolute30,
                             Difference10, Difference20, Difference30,
                             Absolute10R, Absolute20R, Absolute30R,
                             Difference10R, Difference20R, Difference30R,
                             Absolute10G, Absolute20G, Absolute30G,
                             Difference10G, Difference20G, Difference30G,
                             Absolute10B, Absolute20B, Absolute30B,
                             Difference10B, Difference20B, Difference30B };


  PiiColorPercentiles();
  ~PiiColorPercentiles();

protected:
  void process();
  void check(bool reset);
  void aboutToChangeState(State state);

  QStringList percentiles() const;
  void setPercentiles(const QStringList& percentiles);

  void setFeatureSetType(FeatureSetType featureSetType);
  FeatureSetType featureSetType() const;

  void setUniversalFeatureSet(UniversalFeatureSet universalFeatureSet);
  UniversalFeatureSet universalFeatureSet() const;

  void setPrebuiltFeatureSet(PrebuiltFeatureSet prebuiltFeatureSet);
  PrebuiltFeatureSet prebuiltFeatureSet() const;

  void setLevels(int levels);
  int levels() const;

  void setRoiType(PiiImage::RoiType roiType);
  PiiImage::RoiType roiType() const;
  
private:
  template <class T> class ColorPercentiles;
  template <class T> class GrayPercentiles;
  template <class T> class Percentiles;
  template <class T> friend class ColorPercentiles;
  template <class T> friend class GrayPercentiles;
  template <class T> friend class Percentiles;
  
  void useUniversalFeatureSet(UniversalFeatureSet set);
  void usePrebuiltFeatureSet(PrebuiltFeatureSet set);
  void usePercentiles(const QStringList& percentiles);
  
  QPair<int,int> createDef(QString str);
  template <class T> void percentiles(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    FeatureSetType featureSetType;
    UniversalFeatureSet universalFeatureSet;
    PrebuiltFeatureSet prebuiltFeatureSet;
    
    QStringList lstPercentileNames;
    QList<QPair<int,int> > lstPercentiles;
    QList<QPair<int,int> > lstDiffs;
    int iLevels;

    PiiInputSocket* pRoiInput;

    PiiImage::RoiType roiType;
    PiiImage::HistogramHandler* pPercentiles;
    unsigned int uiPreviousType;
  };
  PII_D_FUNC;

};

#endif //_PIICOLORPERCENTILES_H
