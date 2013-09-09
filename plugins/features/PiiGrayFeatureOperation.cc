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

#include "PiiGrayFeatureOperation.h"

#include <PiiYdinTypes.h>
#include <PiiMath.h>
#include "PiiGrayFeatureExtractor.h"

const char* PiiGrayFeatureOperation::_pFeatureNames[PII_GRAY_FEATURE_OPERATION_FEATURECNT] = {
  "Minimum",
  "Maximum",
  "Average",
  "Contrast",
  "MaxDiff",
  "Variance",
  "Deviation",
  "HighAverage",
  "LowAverage"
};


PiiGrayFeatureOperation::Data::Data() :
  dReference(127),
  iFeatureMask(1+4),
  pExtractor(0)

{
  // Enable min and avg
  lstEnabledFeatures << 0 << 2;
}

PiiGrayFeatureOperation::PiiGrayFeatureOperation() :
  PiiDefaultOperation(new Data)
{
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiOutputSocket("features"));
}

PiiGrayFeatureOperation::~PiiGrayFeatureOperation()
{
  PII_D;
  delete d->pExtractor;
}

void PiiGrayFeatureOperation::setFeatures(const QStringList& features)
{
  PII_D;
  d->lstEnabledFeatures.clear();
  // Add features always in the same order they appear in _pFeatureNames
  for (int n=0; n<PII_GRAY_FEATURE_OPERATION_FEATURECNT; ++n)
    {
      // If this feature is listed in features, and it is not already
      // enabled, enable it
      if (features.contains(_pFeatureNames[n]) && !d->lstEnabledFeatures.contains(n))
        d->lstEnabledFeatures << n;
    }
  // Enable minimum gray level if nothing else is selected
  if (d->lstEnabledFeatures.size() == 0)
    d->lstEnabledFeatures << 0;

  // Build a bit mask of enabled features
  d->iFeatureMask = 0;
  for (int i = 0; i<d->lstEnabledFeatures.size(); ++i)
    d->iFeatureMask |= 1 << d->lstEnabledFeatures[i];
}

QStringList PiiGrayFeatureOperation::features() const
{
  const PII_D;
  QStringList result;
  for (int f=0; f<d->lstEnabledFeatures.size(); f++)
    result << _pFeatureNames[d->lstEnabledFeatures[f]];
  return result;
}

void PiiGrayFeatureOperation::check(bool reset)
{
  PII_D;
  PiiDefaultOperation::check(reset);

  // Optimized feature extractors for some feature combinations
  switch (d->iFeatureMask)
    {
    case 1:
      setExtractor(new MinimumExtractor(inputAt(0), outputAt(0)));
      break;
    case 2:
      setExtractor(new MaximumExtractor(inputAt(0), outputAt(0)));
      break;
    case 3:
      setExtractor(new MinMaxExtractor(inputAt(0), outputAt(0)));
      break;
    case 4:
      setExtractor(new AverageExtractor(inputAt(0), outputAt(0)));
      break;
    case 5:
      setExtractor(new MinAvgExtractor(inputAt(0), outputAt(0)));
      break;
    case 6:
      setExtractor(new MaxAvgExtractor(inputAt(0), outputAt(0)));
      break;
    case 7:
      setExtractor(new MinMaxAvgExtractor(inputAt(0), outputAt(0)));
      break;
    default:
      // The default extractor handles all cases
      setExtractor(new DefaultExtractor(this));
      break;
    }
}

void PiiGrayFeatureOperation::process()
{
  _d()->pExtractor->process();
}

void PiiGrayFeatureOperation::setExtractor(PiiGrayFeatureOperation::FeatureExtractor* extractor)
{
  PII_D;
  delete d->pExtractor;
  d->pExtractor = extractor;
}

void PiiGrayFeatureOperation::setReference(double reference) { _d()->dReference = reference; }
double PiiGrayFeatureOperation::reference() const { return _d()->dReference; }
QList<int> PiiGrayFeatureOperation::enabledFeatures() const { return _d()->lstEnabledFeatures; }
