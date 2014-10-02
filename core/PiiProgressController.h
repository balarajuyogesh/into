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

#ifndef _PIIPROGRESSCONTROLLER_H
#define _PIIPROGRESSCONTROLLER_H

#include "PiiGlobal.h"
#include "PiiMathDefs.h"

/**
 * An interface for objects that control the functioning of operations
 * that take a long time.
 *
 */
class PII_CORE_EXPORT PiiProgressController
{
public:
  virtual ~PiiProgressController();

  /**
   * Check if the slow operation is still allowed to continue. The
   * implementation should return `true` if the operation is still
   * allowed to run and `false` otherwise. The default implementation
   * returns true.
   *
   * @param progressPercentage an estimate of the current state of
   * progress as a percentage (0.0 - 1.0). If the value is `NaN`, the
   * current state of progress is not known to the caller.
   */
  virtual bool canContinue(double progressPercentage = NAN) const;
};


#endif //_PIIPROGRESSCONTROLLER_H
