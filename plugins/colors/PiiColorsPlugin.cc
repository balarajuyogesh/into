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

#include "PiiRgbNormalizer.h"
#include "PiiColorConverter.h"
#include "PiiColorModelMatcher.h"
#include "PiiImageConversionSwitch.h"
#include "PiiColorChannelSplitter.h"
#include "PiiColorChannelSetter.h"
#include "PiiColorPercentiles.h"
#include "PiiColorCorrelogramOperation.h"
#include <PiiYdinResources.h>

PII_IMPLEMENT_PLUGIN(PiiColorsPlugin);


PII_REGISTER_OPERATION(PiiRgbNormalizer);
PII_REGISTER_OPERATION(PiiColorConverter);
PII_REGISTER_OPERATION(PiiColorModelMatcher);
PII_REGISTER_OPERATION(PiiImageConversionSwitch);
PII_REGISTER_OPERATION(PiiColorChannelSplitter);
PII_REGISTER_OPERATION(PiiColorChannelSetter);
PII_REGISTER_OPERATION(PiiColorPercentiles);
PII_REGISTER_OPERATION(PiiColorCorrelogramOperation);
