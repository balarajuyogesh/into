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

#ifndef _PIIRESOURCECONNECTOR_H
#define _PIIRESOURCECONNECTOR_H

#include "PiiYdin.h"
#include <PiiSerializationTraits.h>

/**
 * An interface for classes that connect resource instances together.
 * A "resource" can be an operation, a widget, or a distance measure,
 * to name a few examples. To establish a connection between two
 * resources one needs to first instantiate them and the connector.
 * Then, pointers to the two resources are passed to the connector to
 * bind the instances together.
 *
 */
class PII_YDIN_EXPORT PiiResourceConnector
{
public:
  virtual ~PiiResourceConnector();

  /**
   * Connect two resources. There are different flavors of
   * "connection" including, for example, Qt's signal-slot connections
   * and socket connections between PiiOperations. Each connector is
   * specific to a pair of resources. Thus, it knows how to cast the
   * provided `void` pointers to the correct type.
   *
   * @return `true` if the resources were successfully connected,
   * `false` otherwise.
   */
  virtual bool connectResources(void* resource1, void* resource2, const QString& role) const = 0;
  /**
   * Disconnect resources. If the resources are not connected, this
   * function does nothing.
   *
   * @return `true` if the resources were successfully disconnected,
   * `false` otherwise.
   */
  virtual bool disconnectResources(void* resource1, void* resource2, const QString& role) const = 0;
};
PII_SERIALIZATION_NAME(PiiResourceConnector);
PII_SERIALIZATION_ABSTRACT(PiiResourceConnector);

#endif //_PIIRESOURCECONNECTOR_H
