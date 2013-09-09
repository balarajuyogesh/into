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

#include "PiiUndoCommandProducer.h"
#include <QUndoCommand>

PiiUndoCommandProducer::PiiUndoCommandProducer() :
  _pUndoCommandListener(0)
{
}

PiiUndoCommandProducer::~PiiUndoCommandProducer()
{
}

void PiiUndoCommandProducer::setUndoCommandListener(PiiUndoCommandProducer::Listener* undoCommandListener)
{
  _pUndoCommandListener = undoCommandListener;
}

PiiUndoCommandProducer::Listener* PiiUndoCommandProducer::undoCommandListener() const
{
  return _pUndoCommandListener;
}

void PiiUndoCommandProducer::undoCommandPerformed(QUndoCommand* command)
{
  if (_pUndoCommandListener != 0)
    _pUndoCommandListener->undoCommandPerformed(this, command);
  else
    delete command;
}

int PiiUndoCommandProducer::generateId()
{
  static int lastId = -1;
  return ++lastId;
}
