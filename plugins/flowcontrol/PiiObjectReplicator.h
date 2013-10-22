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

#ifndef _PIIOBJECTREPLICATOR_H
#define _PIIOBJECTREPLICATOR_H

#include <PiiDefaultOperation.h>

/**
 * An operation that replicates an object received in its `data`
 * input. The operation useful when an object needs to be repeated
 * many times. A typical situation arises when an image is split into
 * smaller pieces, each of which is processed separately. Later, the
 * pieces need to be connected to the name of the original image file,
 * which is sent by PiiImageFileReader only once per each image. The
 * solution is to connect the file name to the `data` input of a
 * PiiObjectReplicator and trigger its output with the smaller image
 * pieces.
 *
 * ! This operation can lead to lock-ups if not used carefully. If
 * two branches of a processing pipeline are connected to the `data`
 * and `trigger` inputs, make sure that `data` is always received
 * first. Otherwise, if there are no threaded operations in the
 * pipelines, and if the input queue in `trigger` becomes full before
 * `data` is received, the whole configuration may hang. The output
 * that sends data to the two branches should be first connected to 
 * `data` and then to the other branch.
 *
 * Inputs
 * ------
 *
 * @in data - the data that needs to be copied. Any type.
 *
 * @in trigger - emit the last object in `data` each time an object
 * is received in this input. Any type.
 *
 * Outputs
 * -------
 *
 * @out output - the object last received in `data`. This output will
 * emit the object whenever an object is received in `trigger`.
 *
 */
class PiiObjectReplicator : public PiiDefaultOperation
{
  Q_OBJECT

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiObjectReplicator();

protected:
  void process();
  PiiFlowController* createFlowController();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    PiiVariant pData;
  };
  PII_D_FUNC;
};


#endif //_PIIOBJECTREPLICATOR_H
