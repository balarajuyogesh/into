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

#include "PiiPlugin.h"

#include "PiiNetworkCameraOperation.h"
#include "PiiCameraOperation.h"
#include "protocols/PiiAviivaM2CLSerialProtocol.h"
#include "protocols/PiiAviivaSC2CLSerialProtocol.h"

PII_IMPLEMENT_PLUGIN(PiiCameraPlugin);

PII_REGISTER_OPERATION(PiiNetworkCameraOperation);
PII_REGISTER_OPERATION(PiiCameraOperation);
PII_REGISTER_CLASS(PiiAviivaM2CLSerialProtocol, PiiCameraConfigurationProtocol);
PII_REGISTER_CLASS(PiiAviivaSC2CLSerialProtocol, PiiCameraConfigurationProtocol);


#ifndef PII_NO_FIREWIRE
#include "firewire/PiiFireWireOperation.h"
PII_REGISTER_OPERATION(PiiFireWireOperation);
#endif



