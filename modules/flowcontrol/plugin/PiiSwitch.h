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

#ifndef _PIISWITCH_H
#define _PIISWITCH_H

#include <PiiDefaultOperation.h>

/**
 * An operation that passes objects conditionally depending on a
 * trigger input.
 *
 * Inputs
 * ------
 *
 * @in trigger - trigger input. Type is ignored in
 * `AsynchronousMode`, and the input can even be left unconnected. In
 * `SynchronousMode`, the trigger must be a boolean value or a number.
 *
 * @in inputX - reads in objects of any type. The object read from
 * `inputX` will be emitted to `outputX` when triggered. X ranges from
 * 0 to [inputCount](PiiVariableInputOperation::inputCount) - 1.
 * `input0` can also be accessed as `input`.
 *
 * Outputs
 * -------
 *
 * @out outputX - emits the last input object whenever a trigger is
 * received. `output0` can also be accessed as `output`.
 *
 * The default number of input-output pairs is 1.
 *
 */
class PiiSwitch : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of dynamic inputs (in addition to `trigger`). The
   * default is one.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);

  /**
   * Operation mode. The default mode is `SynchronousMode`.
   */
  Q_PROPERTY(OperationMode operationMode READ operationMode WRITE setOperationMode);
  Q_ENUMS(OperationMode);

  /**
   * Turns the trigger input to "always on". This property only
   * affects AsynchronousMode. Setting its value to `true` makes the
   * operation pass all incoming objects directly and ignore the
   * `trigger` input. If the `trigger` input is not connected (in
   * `AsynchronousMode`), this property controls can be used to block
   * and release the object flow through this operation. The default
   * value is `false`.
   */
  Q_PROPERTY(bool passThrough READ passThrough WRITE setPassThrough);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Operation modes.
   *
   * - `AsynchronousMode` - trigger and the other inputs are not
   * synchronized to each other. The trigger can come from any
   * unrelated source. If [passThrough] is `false`, exactly one set of
   * input objects will be passed for each object received from the
   * trigger input. If there are no incoming objects when the trigger
   * arrives, the next incoming objects will be emitted. If
   * [passThrough] is `true`, the trigger input will have no effect
   * and all input objects will be passed. Note that the same objects
   * may be sent many times if the trigger input has a higher data
   * rate than the other inputs.
   *
   * - `SynchronousMode` - the trigger signal is used as a "filter".
   * All inputs work in sync: there must be a trigger signal for each
   * set of input objects. If a non-zero number or `true` is received,
   * the input objects will be passed. Otherwise, the input objects
   * will be ignored.
   */
  enum OperationMode { SynchronousMode, AsynchronousMode };

  PiiSwitch();

  /**
   * Aliases `input` to `input0`.
   */
  PiiInputSocket* input(const QString &name) const;
  /**
   * Aliases `output` to `output0`.
   */
  PiiOutputSocket* output(const QString &name) const;

  void check(bool reset);

protected:
  PiiInputSocket* createInput(int index);

  void process();
  void aboutToChangeState(State state);

  void setOperationMode(OperationMode operationMode);
  OperationMode operationMode() const;
  void setDynamicInputCount(int inputCount);
  int dynamicInputCount() const;
  void setPassThrough(bool passThrough);
  bool passThrough() const;

private:
  void emitInputObjects();
  void emitObjectList();
  void storeInputObjects();

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    OperationMode operationMode;
    int iTriggerCount;
    QList<PiiVariant> lstObjects;
    int iStaticInputCount;
    bool bPassThrough;
  };
  PII_D_FUNC;
};

#endif //_PIISWITCH_H
