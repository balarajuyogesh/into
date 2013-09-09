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

#ifndef _PIIFLOWLEVELCHANGER_H
#define _PIIFLOWLEVELCHANGER_H

#include <PiiDefaultOperation.h>

/**
 * An operation that changes the flow level of data passed through it. 
 * It is useful in situations where one needs to adjust different flow
 * levels before connecting them to further processing. A typical use
 * is to fake image pieces to be "original" images as shown in the
 * image below. The images emitted through @p output will be at the
 * flow level of PiiImageFileReader's @p image.
 *
 * @image html flowlevelchanger.png
 *
 * @inputs
 *
 * @in sync - Synchronization input. The output will always maintain
 * the flow level of this input.
 *
 * @in input - The objects whose flow level must match @p sync.
 * 
 * @outputs
 *
 * @out output - Objects read from @p input at the flow level of @p
 * sync.
 *
 * @ingroup PiiFlowControlPlugin
 */
class PiiFlowLevelChanger : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiFlowLevelChanger();

protected:
  void process();
};


#endif //_PIIFLOWLEVELCHANGER_H
