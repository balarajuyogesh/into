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

#include "PiiTriggerSource.h"
#include "PiiDebugOperation.h"
#include "PiiArithmeticOperation.h"
#include "PiiTypeCastingOperation.h"
#include "PiiAbsoluteOperation.h"
#include "PiiLogOperation.h"
#include "PiiMatrixNormalizer.h"
#include "PiiAverageOperation.h"
#include "PiiMovingAverageOperation.h"
#include "PiiDiffOperation.h"
#include "PiiClock.h"
#include "PiiComparisonOperation.h"
#include "PiiMatrixCombiner.h"
#include "PiiStringizer.h"
#include "PiiMatrixComposer.h"
#include "PiiMatrixDecomposer.h"
#include "PiiObjectCounter.h"
#include "PiiIdGenerator.h"
#include "PiiLookupTable.h"
#include "PiiSequenceGenerator.h"
#include "PiiObjectCapturer.h"
#include "PiiMinMaxOperation.h"
#include "PiiMatrixFlipper.h"
#include "PiiMathematicalFunction.h"
#include "PiiStringFormatter.h"
#include "PiiRegExpMatcher.h"
#include "PiiFileSystemWatcher.h"
#include "PiiStringConverter.h"
#include "PiiFileSystemScanner.h"
#include "PiiConstantOperation.h"

PII_IMPLEMENT_PLUGIN(PiiBasePlugin);

PII_REGISTER_OPERATION(PiiTriggerSource);
PII_REGISTER_OPERATION(PiiDebugOperation);
PII_REGISTER_OPERATION(PiiArithmeticOperation);
PII_REGISTER_OPERATION(PiiTypeCastingOperation);
PII_REGISTER_OPERATION(PiiAbsoluteOperation);
PII_REGISTER_OPERATION(PiiLogOperation);
PII_REGISTER_OPERATION(PiiMatrixNormalizer);
PII_REGISTER_OPERATION(PiiAverageOperation);
PII_REGISTER_OPERATION(PiiMovingAverageOperation);
PII_REGISTER_OPERATION(PiiDiffOperation);
PII_REGISTER_OPERATION(PiiClock);
PII_REGISTER_OPERATION(PiiComparisonOperation);
PII_REGISTER_OPERATION(PiiMatrixCombiner);
PII_REGISTER_OPERATION(PiiStringizer);
PII_REGISTER_OPERATION(PiiMatrixComposer);
PII_REGISTER_OPERATION(PiiMatrixDecomposer);
PII_REGISTER_OPERATION(PiiObjectCounter);
PII_REGISTER_OPERATION(PiiIdGenerator);
PII_REGISTER_OPERATION(PiiLookupTable);
PII_REGISTER_OPERATION(PiiSequenceGenerator);
PII_REGISTER_OPERATION(PiiObjectCapturer);
PII_REGISTER_OPERATION(PiiMinMaxOperation);
PII_REGISTER_OPERATION(PiiMatrixFlipper);
PII_REGISTER_OPERATION(PiiMathematicalFunction);
PII_REGISTER_OPERATION(PiiStringFormatter);
PII_REGISTER_OPERATION(PiiRegExpMatcher);
PII_REGISTER_OPERATION(PiiFileSystemWatcher);
PII_REGISTER_OPERATION(PiiStringConverter);
PII_REGISTER_OPERATION(PiiFileSystemScanner);
PII_REGISTER_OPERATION(PiiConstantOperation);
