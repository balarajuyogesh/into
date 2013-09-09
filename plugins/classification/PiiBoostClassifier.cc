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

#include "PiiBoostClassifier.h"

const char* pBoostFactoryNotSetError = QT_TRANSLATE_NOOP("PiiBoostClassifier", "Factory object is not set.");
const char* pBoostTooFewClassesError =
  QT_TRANSLATE_NOOP("PiiBoostClassifier",
                    "Training set must containt at least two classes. It only has %1.");
const char* pBoostTooManyClassesError =
  QT_TRANSLATE_NOOP("PiiBoostClassifier",
                    "Only SammeBoost accepts more than two classes. The training set contains %1.");
const char* pBoostTooWeakClassifierError =
  QT_TRANSLATE_NOOP("PiiBoostClassifier",
                    "Boosting was interrupted due to a too weak classifier. Error = %1, must be less than %2.");
