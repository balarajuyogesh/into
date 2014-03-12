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
#include "PiiLbpOperation.h"
#include "PiiContrastOperation.h"
#include "PiiWaveletTextureOperation.h"
#include "PiiOrientationEstimator.h"
#include "PiiSpectralPeakDetector.h"

PII_IMPLEMENT_PLUGIN(PiiTexturePlugin);

PII_REGISTER_OPERATION(PiiLbpOperation);
PII_REGISTER_OPERATION(PiiContrastOperation);
PII_REGISTER_OPERATION(PiiWaveletTextureOperation);
PII_REGISTER_OPERATION(PiiOrientationEstimator);
PII_REGISTER_OPERATION(PiiSpectralPeakDetector);
