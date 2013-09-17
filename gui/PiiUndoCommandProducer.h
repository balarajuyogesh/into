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

#ifndef _PIIUNDOCOMMANDPRODUCER_H
#define _PIIUNDOCOMMANDPRODUCER_H

#include "PiiGui.h"
#include <QObject>

class QUndoCommand;

/**
 * An interface for UI components that support undo/redo functionality.
 * Using this interface makes it possible to combine undo/redo
 * functionality provided by diverse software components into a global
 * undo/redo function.
 *
 */
class PII_GUI_EXPORT PiiUndoCommandProducer
{
public:
  /**
   * A listener interface for applied undoable commands. This
   * interface is implemented by the application that controls a set
   * of UI components performing undoable (a.k.a impossible) actions.
   */
  class Listener
  {
  public:
    /**
     * Informs the listener that an undoable action has been
     * performed.
     *
     * @param sender the object that performed the action
     *
     * @param command the action that was performed. The receiver @b
     * **must** take the ownership of this pointer. Typically, it will be
     * placed on a QUndoStack.
     */
    virtual void undoCommandPerformed(PiiUndoCommandProducer* sender, QUndoCommand* command) = 0;
  };

  /**
   * Default constructor.
   */
  PiiUndoCommandProducer();
  
  /**
   * Virtual destructor. Does nothing.
   */
  virtual ~PiiUndoCommandProducer();

  /**
   * Set the listener.
   */
  void setUndoCommandListener(Listener* undoCommandListener);
  /**
   * Get the listener.
   */
  Listener* undoCommandListener() const;

  /**
   * Generate a unique id number for a class of undoable commands. 
   * Unique id numbers can be utilized by Qt's undo system in merging
   * successive commands. Using the technique described in the example
   * below ensures application-wide unique command identifiers and
   * ensures that no overlapping ids will be used by independent
   * components.
   *
   * ~~~
   * // MyCommand.h
   * class MyCommand : public QUndoCommand
   * {
   * public:
   *   //...
   *   int id() { return iCommandId; }
   * private:
   *   static int iCommandId;
   * };
   *
   * // MyCommand.cc
   * int MyCommand::iCommandId = PiiUndoCommandProducer::generateId();
   * ~~~
   */
  static int generateId();

protected:
  /**
   * Called by subclasses to indicate that an undoable command has
   * been performed. The caller must release the ownership of the
   * pointer. If there is a listener, the command will be passed to
   * its `undoCommandPerformed`() function. If there is no listener,
   * the command will be deleted.
   */
  void undoCommandPerformed(QUndoCommand* command);
  
private:
  Listener* _pUndoCommandListener;
};

Q_DECLARE_INTERFACE(PiiUndoCommandProducer, "com.intopii.into.piiundocommandproducer/1.0");

#endif //_PIIUNDOCOMMANDPRODUCER_H
