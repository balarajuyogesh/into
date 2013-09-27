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

#ifndef _PIIDEMUXOPERATION_H
#define _PIIDEMUXOPERATION_H

#include <PiiDefaultOperation.h>

/**
 * A demultiplexer. A demux sends objects coming from one channel to
 * multiple channels. The active output channel is selected by a
 * control input.
 *
 * Demultiplexer is useful in many different situations. For example,
 * it can be used to perform different processing for different
 * objects. In such a scenario the operation works as an analogue to
 * the `switch` clause in programming languages. The picture below
 * shows a scenario in which PiiDemuxOperation is used as a load
 * balancer. Let us assume thet both of the `HeavyProcessing`
 * operations are run in separate threads and that `Control` emits an
 * alternating sequence of ones and zeros. If the machine had two
 * processors, the `HeavyProcessing` operations would be executed
 * simultaneously.
 *
 * ![Demultiplexer](demuxoperation.png)
 *
 * Since the outputs of a demultiplexer are active in a
 * non-deterministic manner, there is no way to synchronize their
 * emissions. This causes problems if you connect two or more of the
 * outputs to a single operation either directly or indirectly. The
 * most likely cause is that the configuration hangs. Therefore, it is
 * advisable to always couple PiiDemuxOperation with a PiiMuxOperation
 * that is controlled by the same control signal.
 *
 * Inputs
 * ------
 *
 * @in control - selects the output channel. Accepts any numeric type
 * in the range [0, `outputCount` - 1]. All numeric values are
 * automatically converted to integers.
 *
 * @in input - any object. Passed to the output selected by the 
 * `control` input.
 * 
 * Outputs
 * -------
 *
 * @out outputX - the output channels. X ranges from 0 to 
 * `dynamicOutputCount` - 1.
 *
 */
class PiiDemuxOperation : public PiiDefaultOperation
{
  Q_OBJECT
  
  /**
   * The number of output channels. The value equals to the number of
   * output sockets. The default value is two.
   */
  Q_PROPERTY(int dynamicOutputCount READ dynamicOutputCount WRITE setDynamicOutputCount);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiDemuxOperation();

  void setDynamicOutputCount(int count);
  int dynamicOutputCount() const;

protected:
  void process();
};


#endif //_PIIDEMUXOPERATION_H
