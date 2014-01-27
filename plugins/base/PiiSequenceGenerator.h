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

#ifndef _PIISEQUENCEGENERATOR_H
#define _PIISEQUENCEGENERATOR_H

#include <PiiDefaultOperation.h>

/**
 * An operation that generates sequences of numbers. The sequence goes
 * from [sequenceStart] to [sequenceEnd], stepping [step] units each time.
 *
 * Inputs
 * ------
 *
 * @in trigger - an optional input that triggers the emission.
 *
 * Outputs
 * -------
 *
 * @out output - a number in the sequence.
 *
 */
class PiiSequenceGenerator : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The first value in the sequence. The default value is 0. This
   * value must be less than or equal to [sequenceEnd].
   */
  Q_PROPERTY(double sequenceStart READ sequenceStart WRITE setSequenceStart);
  /**
   * The last value in the sequence. The default value is 1. This
   * value must be larger than or equal to [sequenceStart].
   */
  Q_PROPERTY(double sequenceEnd READ sequenceEnd WRITE setSequenceEnd);
  /**
   * The increment on each iteration. Use a negative value to go
   * backwards. The default value is 1.
   */
  Q_PROPERTY(double step READ step WRITE setStep);
  /**
   * The sequence mode. The default mode is `Repeat`.
   */
  Q_PROPERTY(SequenceMode sequenceMode READ sequenceMode WRITE setSequenceMode);
  Q_ENUMS(SequenceMode);

  /**
   * The number of times the sequence will be repeated. Once this
   * count is reached, the operation will send a stop signal which
   * will stop the whole execution engine if properly connected. The
   * default value is zero, which means "eternally". This value has no
   * effect if `trigger` is connected.
   */
  Q_PROPERTY(int repeatCount READ repeatCount WRITE setRepeatCount);
  /**
   * The data type of the output. By default, this value is `false`
   * and `doubles` will be emitted. Setting this value to `true`
   * causes converts the output to `ints`.
   */
  Q_PROPERTY(bool integerOutput READ integerOutput WRITE setIntegerOutput);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Sequence modes.
   *
   * - `Repeat` - the sequence will start at the beginning once
   * [sequenceEnd] is reached.
   *
   * - `BackAndForth` - the sequence goes back and forth between
   * [sequenceEnd] and [sequenceStart].
   */
  enum SequenceMode { Repeat, BackAndForth };

  PiiSequenceGenerator();

  void setSequenceStart(double sequenceStart);
  double sequenceStart() const;
  void setSequenceEnd(double sequenceEnd);
  double sequenceEnd() const;
  void setStep(double step);
  double step() const;
  void setSequenceMode(const SequenceMode& sequenceMode);
  SequenceMode sequenceMode() const;
  void setRepeatCount(int repeatCount);
  int repeatCount() const;
  void setIntegerOutput(bool integerOutput);
  bool integerOutput() const;

  void check(bool reset);

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    double dSequenceStart;
    double dSequenceEnd;
    double dStep;
    SequenceMode sequenceMode;
    int iRepeatCount;
    bool bIntegerOutput;
    double dCurrentValue, dCurrentStep;
    int iCurrentRepeatCount;
    bool bAutoExit;
  };
  PII_D_FUNC;
};


#endif //_PIISEQUENCEGENERATOR_H
