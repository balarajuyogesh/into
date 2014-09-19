/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#ifndef _PIITIMELIMITER_H
#define _PIITIMELIMITER_H

#include "PiiProgressController.h"

class PII_CORE_EXPORT PiiTimeLimiter : public PiiProgressController
{
public:
  PiiTimeLimiter(qint64 milliseconds);
  ~PiiTimeLimiter();

  qint64 millisecondsElapsed();

  bool canContinue(double progressPercentage) const;

private:
  class Data;
  Data* d;
  PII_DISABLE_COPY(PiiTimeLimiter);
};

#endif //_PIITIMELIMITER_H
