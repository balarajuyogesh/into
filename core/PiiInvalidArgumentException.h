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

#ifndef _PIIINVALIDARGUMENTEXCEPTION_H
#define _PIIINVALIDARGUMENTEXCEPTION_H

/**
 * @decl class PiiInvalidArgumentException
 *
 * An exception for situations in which invalid arguments were used in
 * a function or constructor call.
 */
#include "PiiGlobal.h"

#define PII_EXCEPTION_CLASS PiiInvalidArgumentException
#define PII_EXCEPTION_BUILDING_LIBRARY PII_BUILDING_CORE
#include "PiiExceptionFactory.h"

#endif //_PIIINVALIDARGUMENTEXCEPTION_H
