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

#include "PiiSerializationTraits.h"

#include <cstring> //for memcpy, strlen

namespace PiiSerializationTraits
{
  char* createTemplateName(const char* className, const char* typeName)
  {
    const int iClassLen = strlen(className), iTypeLen = strlen(typeName);
    int iLocation = 0;
    char* pResult = new char[iClassLen + iTypeLen + 3];
    memcpy(pResult, className, iClassLen);
    pResult[iClassLen] = '<';
    iLocation = iClassLen + 1;
    memcpy(pResult + iLocation, typeName, iTypeLen);
    iLocation += iTypeLen;
    pResult[iLocation] = '>';
    pResult[++iLocation] = 0;
    return pResult;
  }
}
