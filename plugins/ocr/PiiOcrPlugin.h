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

#ifndef _PIIOCRPLUGIN_H
#define _PIIOCRPLUGIN_H

#include <PiiGlobal.h>

#ifdef PII_BUILD_OCR
#  define PII_OCR_EXPORT PII_DECL_EXPORT
#  define PII_BUILDING_OCR 1
#else
#  define PII_OCR_EXPORT PII_DECL_IMPORT
#  define PII_BUILDING_OCR 0
#endif/**
 * @defgroup PiiOcrPlugin
 *
 * Description here.
 *
 * @dependencies PiiOcrPlugin
 *
 * @runtime PiiOtherPlugin Optional reason.
 * @compile PiiYetAnotherPlugin Optional reason
 */


#endif //_PIIOCRPLUGIN_H
