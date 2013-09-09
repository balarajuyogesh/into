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

#ifndef _PIICLASSINFOREGISTERER_H
#define _PIICLASSINFOREGISTERER_H

#include "PiiYdin.h"

/// @internal
class PII_YDIN_EXPORT PiiClassInfoRegisterer
{
public:
  PiiClassInfoRegisterer(const char* parent,
                         const char* child,
                         const char* superClass = 0,
                         ptrdiff_t offset = 0);

  ~PiiClassInfoRegisterer();

private:
  class Data
  {
  public:
    QList<int> lstIds;
  } *d;
};

#endif //_PIICLASSINFOREGISTERER_H
