/* This file is part of Into.
 * Copyright (C) Intopii
 * All rights reserved.
 */

#ifndef _PIIPYLONDRIVERGLOBAL_H
#define _PIIPYLONDRIVERGLOBAL_H

#include <PiiGlobal.h>

#ifdef PII_BUILD_PYLONDRIVER
#  define PII_PYLONDRIVER_EXPORT PII_DECL_EXPORT
#else
#  define PII_PYLONDRIVER_EXPORT PII_DECL_IMPORT
#endif

#endif //_PIIPYLONDRIVERGLOBAL_H
