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

#ifndef _PIIMOVINGAVERAGEOPERATION_H
#define _PIIMOVINGAVERAGEOPERATION_H

#include <PiiDefaultOperation.h>
#include <QLinkedList>

/**
 * Calculate the moving average over a window of a predefined size. 
 * The output will be the average over the last N input values, which
 * must be of the same type (and size, if matrices are averaged).
 *
 * Inputs
 * ------
 *
 * @in input - the input value. Any numeric or complex type or a
 * matrix containing such types.
 * 
 * Outputs
 * -------
 *
 * @out average - the average over the last N entries (see
 * [windowSize]). The output type is floating point numbers. Double and
 * long int in input result in double output, others result in float
 * output.
 *
 */
class PiiMovingAverageOperation : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The size of the averaging window. Note that the operation buffers
   * this many past values. If large matrices are averaged, a large
   * portion of memory may be reserved. The default value is two (2).
   */
  Q_PROPERTY(int windowSize READ windowSize WRITE setWindowSize);
  /**
   * The minimum acceptable value. If `rangeMin` and [rangeMax] are
   * not equal, they define the range of acceptable values, and the
   * values are handled circularly. This is especially useful for
   * angles, because the mean of 359 and 1 should be 0, not 180. The
   * default value is 0. The range applies only to scalars. Matrices
   * are not checked for range under/overflow.
   */
  Q_PROPERTY(double rangeMin READ rangeMin WRITE setRangeMin);  
  /**
   * The maximum acceptable value. See [rangeMin] for a detailed
   * description. The default value is 0. Use 360 or 2*M_PI for
   * angles.
   */
  Q_PROPERTY(double rangeMax READ rangeMax WRITE setRangeMax);

  /**
   * If this property is true, output type will be forced to input type.
   */
  Q_PROPERTY(bool forceInputType READ forceInputType WRITE setForceInputType);
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiMovingAverageOperation();

  void setWindowSize(int windowSize);
  int windowSize() const;
  void setRangeMin(double rangeMin);
  double rangeMin() const;
  void setRangeMax(double rangeMax);
  double rangeMax() const;
  void setForceInputType(bool forceInputType);
  bool forceInputType() const;

protected:
  void process();
  void check(bool reset);

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    int iWindowSize;
    double dRangeMin;
    double dRangeMax;
    double dRange;
    unsigned int uiType;
    QLinkedList<PiiVariant> lstBuffer;
    bool bForceInputType;
  };
  PII_D_FUNC;

  template <class T> void average(const PiiVariant& obj);
  template <class T> void matrixAverage(const PiiVariant& obj);
  template <class T, class ResultType> void averageTemplate(const PiiVariant& obj);
  template <class T> void addImpl(T& op1, T op2, int index);
  template <class T> void scaleImpl(T& result, int cnt);
  template <class T> void add(T& op1, const T& op2, int index);
  template <class T> void scale(T& result, int cnt);
};


#endif //_PIIMOVINGAVERAGEOPERATION_H
