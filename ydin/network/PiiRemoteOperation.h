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

#ifndef _PIIREMOTEOPERATION_H
#define _PIIREMOTEOPERATION_H

#include <PiiRemoteQObject.h>

#include "PiiOperation.h"

/**
 * An operation that is executed on another computer. This operation
 * works just like an ordinary operation but transparently passes data
 * to and from a remote object server. The inputs and outputs of the
 * operation are defined by the server object.
 *
 * @ingroup Ydin
 */
class PII_YDIN_EXPORT PiiRemoteOperation :
  public PiiRemoteQObject<PiiOperation>
{
public:
  PiiRemoteOperation();
  PiiRemoteOperation(const QString& serverUri);
  ~PiiRemoteOperation();

  void check(bool reset);
  void start();
  void pause();
  void stop();
  void interrupt();
  bool wait(unsigned long time=ULONG_MAX);

  State state() const;
  PiiAbstractInputSocket* input(const QString& name) const;
  PiiAbstractOutputSocket* output(const QString& name) const;
  QList<PiiAbstractInputSocket*> inputs() const;
  QList<PiiAbstractOutputSocket*> outputs() const;
  void startPropertySet(const QString& name = QString());
  void endPropertySet();
  void removePropertySet(const QString& name = QString());
  void reconfigure(const QString& propertySetName = QString());
  int inputCount() const;
  int outputCount() const;
  PiiRemoteOperation* clone() const;

private:
  class Data : public PiiRemoteMetaObject::Data
  {
  public:
    bool bConfigured;
    QList<PiiRemoteInputSocket*> lstInputs;
    QList<PiiRemoteOutputSocket*> lstOutputs;
  };
  PII_D_FUNC;
};

#endif //_PIIREMOTEOPERATION_H
