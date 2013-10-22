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

#include "PiiPlugin.h"

//Basic image handling
#include "PiiImageFileReader.h"
#include "PiiImageFileWriter.h"
#include "PiiImageSplitter.h"
#include "PiiImageCropper.h"
#include "PiiImagePieceJoiner.h"
#include "PiiEdgeDetector.h"
#include "PiiBackgroundExtractor.h"
#include "PiiImageAnnotator.h"
#include "PiiImageScaleOperation.h"
#include "PiiImageRotationOperation.h"
#include "PiiImageFilterOperation.h"
#include "PiiCornerDetector.h"
#include "PiiAdaptiveImageNormalizer.h"

//Histograms 
#include "PiiHistogramOperation.h"
#include "PiiQuantizerOperation.h"
#include "PiiHistogramBackProjector.h"
#include "PiiHistogramEqualizer.h"

//Binary
#include "PiiThresholdingOperation.h"
#include "PiiMorphologyOperation.h"
#include "PiiLabelingOperation.h"
#include "PiiObjectPropertyExtractor.h"
#include "PiiFeatureRangeLimiter.h"
#include "PiiMaskGenerator.h"
#include "PiiBoundaryFinderOperation.h"

// Other
#include "PiiImageUnwarpOperation.h"

PII_IMPLEMENT_PLUGIN(PiiImagePlugin);

//Basic image handling
PII_REGISTER_OPERATION(PiiImageFileReader);
PII_REGISTER_OPERATION(PiiImageFileWriter);
PII_REGISTER_OPERATION(PiiImageSplitter);
PII_REGISTER_OPERATION(PiiImageCropper);
PII_REGISTER_OPERATION(PiiImagePieceJoiner);
PII_REGISTER_OPERATION(PiiEdgeDetector);
PII_REGISTER_OPERATION(PiiBackgroundExtractor);
PII_REGISTER_OPERATION(PiiImageAnnotator);
PII_REGISTER_OPERATION(PiiImageScaleOperation);
PII_REGISTER_OPERATION(PiiImageRotationOperation);
PII_REGISTER_OPERATION(PiiImageFilterOperation);
PII_REGISTER_OPERATION(PiiCornerDetector);
PII_REGISTER_OPERATION(PiiAdaptiveImageNormalizer);

//Histograms
PII_REGISTER_OPERATION(PiiHistogramOperation);
PII_REGISTER_OPERATION(PiiQuantizerOperation);
PII_REGISTER_OPERATION(PiiHistogramBackProjector);
PII_REGISTER_OPERATION(PiiHistogramEqualizer);

//Binary
PII_REGISTER_OPERATION(PiiThresholdingOperation);
PII_REGISTER_OPERATION(PiiMorphologyOperation);
PII_REGISTER_OPERATION(PiiLabelingOperation);
PII_REGISTER_OPERATION(PiiObjectPropertyExtractor);
PII_REGISTER_OPERATION(PiiFeatureRangeLimiter);
PII_REGISTER_OPERATION(PiiMaskGenerator);
PII_REGISTER_OPERATION(PiiBoundaryFinderOperation);

//Other
PII_REGISTER_OPERATION(PiiImageUnwarpOperation);

#include <QtPlugin>
#if QT_VERSION < 0x050000
Q_IMPORT_PLUGIN(piiraw)
#else
Q_IMPORT_PLUGIN(PiiRawImagePlugin)
#endif
  
