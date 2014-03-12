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

#ifndef _PIIFREQUENCYCOUNTER_H
#define _PIIFREQUENCYCOUNTER_H

#include <PiiDefaultOperation.h>
#include <QThread>
#include <QTime>

class PiiOutputSocket;
class PiiFreqOutputEmitter;

/**
 * Limits object rate based on frequency. This operation can be used
 * to limit the rate of objects to a certain maximum frequency. It
 * works by just passing objects until the maximum rate is exceeded.
 * No objects will be passed until the object rate comes back to an
 * allowable level. The operation can handle any number of
 * synchronized object streams in parallel.
 *
 * Inputs
 * ------
 *
 * @in inputX - any number of input sockets.
 *
 * Outputs
 * -------
 *
 * @out frequency - the current input frequency in Hz (double)
 *
 * @out outputX - numbered outputs corresponding to the inputs.
 *
 */
class PiiFrequencyCounter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Number of inputs. When the property is set, new input are
   * created, or extra inputs are removed so that the amount of input
   * sockets matches with the property value. For each input a
   * corresponding output is created, or removed, depending the amount
   * of inputs and outputs before setting the property. It should be
   * taken into account, that there is always `frequency` output,
   * independent the amount of inputs and other outputs. The default
   * value is one.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);

  /**
   * Defines the maximum output frequency of the output sockets, which
   * are connected to the corresponding intputs. With the zero value
   * no filtering is done, but all incoming objects are let gone through.
   */
  Q_PROPERTY(double maxFrequency READ maxFrequency WRITE setMaxFrequency);

  /**
   * Defines the frequency, how often the frequency is measured and
   * sent through the frequency ouput.
   */
  Q_PROPERTY(double measurementFrequency READ measurementFrequency WRITE setMeasurementFrequency);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiFrequencyCounter();
  ~PiiFrequencyCounter();

  void check(bool reset);
  void start();

  void setDynamicInputCount(int dynamicInputCount);
  int dynamicInputCount() const;

  void setMaxFrequency(double frequency);
  double maxFrequency() const;

  void setMeasurementFrequency(double frequency);
  double measurementFrequency() const;

protected:
  void process();

private slots:
  void stateChangeOccured(PiiOperation::State state);

private:
  friend class PiiFreqOutputEmitter;

  class Data;
  PII_UNSAFE_D_FUNC;
};

/// @internal
class PiiFreqOutputEmitter : public QThread
{
public:
  PiiFreqOutputEmitter(PiiFrequencyCounter* parent);

  /* Stops the thread.  */
  void stop() { _bRunning = false; }
  void startThread() { _bRunning = true; start(); }

protected:
  void run();

private:
  // Sleeping time in milliseconds
  unsigned int _iSleepDuration;
  PiiFrequencyCounter* _pParent;
  PiiFrequencyCounter::Data* _pParentData;
  volatile bool _bRunning;

  /* Emits the frequency output. */
  void emitFrequency(int elapsed);
};

class PiiFrequencyCounter::Data : public PiiDefaultOperation::Data
{
public:
  Data(PiiFrequencyCounter* parent);
  // The interval (in milliseconds), how often the "frequency" output
  // is emitted. Calculated from dMeasurementFrequency
  unsigned int iMeasurementInterval;

  PiiFreqOutputEmitter *pEmitter;
  double dMaxFrequency;
  // The interval (in mill seconds), how often the outputs are emitted.
  // The value is calculated from dMaxFrequency.
  int iFilterInterval;
  // The frequency of the "frequency" output.
  double dMeasurementFrequency;
  // Weighted mean of the interval of the received objects.
  double dMeanInterval;
  // Maximum value of dMeanInterval
  double dMaxMeanInterval;
  PiiOutputSocket *pFreqOutput;
  QTime filterTime;
  QTime measurementTime;
  // Counter for the frequency output. Must be volatile type, because
  // it is referenced from two different threads.
  volatile int iFrequencyCounter;

  bool bFrequencyOutputConnected;
  int iStaticOutputCount;
};

#endif //_PIIFREQUENCYCOUNTER_H
