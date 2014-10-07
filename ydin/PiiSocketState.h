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

#ifndef _PIISOCKETSTATE_H
#define _PIISOCKETSTATE_H

#include "PiiYdin.h"
#include "PiiVariant.h"
#include <PiiAtomicInt.h>

/**
 * A structure that stores information about a socket's state at run
 * time.
 *
 * @internal
 */
struct PII_YDIN_EXPORT PiiSocketState
{
  /**
   * Initialize a state with flow level `l` and delay `d`.
   */
  PiiSocketState(int l = 0, int d = 0) :
    flowLevel(l), delay(d)
  {}

  /// The current flow level.
  PiiAtomicInt flowLevel;
  /// The number of delayed objects.
  PiiAtomicInt delay;

  bool operator== (const PiiSocketState& other) const
  {
    return flowLevel == other.flowLevel && delay == other.delay;
  }

private:
  friend struct PiiSerialization::Accessor;
  PII_SEPARATE_SAVE_LOAD_MEMBERS
  template <class Archive> void save(Archive& archive, const unsigned int)
  {
    archive << PII_NVP("flowLevel", flowLevel.load());
    archive << PII_NVP("delay", delay.load());
  }

  template <class Archive> void load(Archive& archive, const unsigned int)
  {
    int iTmp;
    archive >> PII_NVP("flowLevel", iTmp);
    flowLevel = iTmp;
    archive >> PII_NVP("delay", iTmp);
    delay = iTmp;
  }
};

#endif //_PIISOCKETSTATE_H
