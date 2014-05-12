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

#include "PiiConstCharWrapper.h"

uint qHash(const PiiConstCharWrapper& key)
{
  uint h = 0;
  uint g;

  const char* p = key.ptr;
  int n = std::strlen(p);
  while (n--)
    {
      h = (h << 4) + (*p++);
      if ((g = (h & 0xf0000000)) != 0)
        h ^= g >> 23;
      h &= ~g;
    }
  return h;
}
