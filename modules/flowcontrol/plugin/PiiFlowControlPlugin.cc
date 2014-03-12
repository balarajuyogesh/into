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

#include "PiiDemuxOperation.h"
#include "PiiPisoOperation.h"
#include "PiiFlowLevelChanger.h"
#include "PiiSwitch.h"
#include "PiiFrequencyCounter.h"
#include "PiiObjectRateChanger.h"
#include "PiiObjectReplicator.h"
#include "PiiCacheOperation.h"

PII_IMPLEMENT_PLUGIN(PiiFlowControlPlugin);

PII_REGISTER_OPERATION(PiiDemuxOperation);
PII_REGISTER_OPERATION(PiiPisoOperation);
PII_REGISTER_OPERATION(PiiFlowLevelChanger);
PII_REGISTER_OPERATION(PiiSwitch);
PII_REGISTER_OPERATION(PiiFrequencyCounter);
PII_REGISTER_OPERATION(PiiObjectRateChanger);
PII_REGISTER_OPERATION(PiiObjectReplicator);
PII_REGISTER_OPERATION(PiiCacheOperation);
