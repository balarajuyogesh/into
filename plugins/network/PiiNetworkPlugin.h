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

/**
 * @defgroup PiiNetworkPlugin
 *
 * PiiNetworkPlugin contains operations for sending and receiving
 * objects through the network.
 */

#ifndef _PIINETWORKPLUGIN_H
#define _PIINETWORKPLUGIN_H

#include <PiiNetworkGlobal.h>

#ifdef PII_BUILD_NETWORKPLUGIN
#  define PII_NETWORKPLUGIN_EXPORT PII_DECL_EXPORT
#else
#  define PII_NETWORKPLUGIN_EXPORT PII_DECL_IMPORT
#endif

#endif //_PIINETWORKPLUGIN_H
