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

#ifndef _PIIDELAY_H
#define _PIIDELAY_H

#include <QThread>
#include "PiiGlobal.h"

/**
 * A utility class that reveals the protected msleep() member of
 * QThread so that program flow can be delayed anywhere.
 *
 * ~~~
 * PiiDelay::msleep(1000); // stops for one second
 * ~~~
 *
 */
class PII_CORE_EXPORT PiiDelay : public QThread
{
public:
  /**
   * Suspend the current thread for *msec* milliseconds.
   */
  static void msleep(int msec);
  /**
   * Suspend the current thread for *usec* microseconds.
   */
  static void usleep(int usec);
};

#endif //_PIIDELAY_H
