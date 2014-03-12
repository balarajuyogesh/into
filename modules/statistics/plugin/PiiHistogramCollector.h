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

#ifndef _PIIHISTOGRAMCOLLECTOR_H
#define _PIIHISTOGRAMCOLLECTOR_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * An operation that collects a histogram of any numeric data.
 *
 * Inputs
 * ------
 *
 * @in sync - an optional sync input. If this input is connected,
 * PiiHistogramCollector collects data until this and the `data` input
 * are in sync. The histogram will be emitted once for each object
 * read from this input. If this input is not connected, the histogram
 * will be emitted for each object read from `data`.
 *
 * @in data - the data. Any number or a numeric matrix. If a matrix is
 * received, each element in the matrix will be added to the
 * histogram. If a scalar is received, it will be added to the
 * histogram. The data will be converted to integers before adding to
 * the histogram.
 *
 * Outputs
 * -------
 *
 * @out sync - the object received in the `sync` input. If the
 * `sync` input is connected, the operation stores the object received
 * until the inputs are synchronized. It will then send the stored
 * object to this output before it sends the histogram.
 *
 * @out y - in `FixedLengthOutput` mode: the histogram as a
 * 1-by-[binCount] PiiMatrix<int>. Zero bins are present in the
 * histogram. Any value outside of [0, [binCount]-1] will be ignored.
 * In the output, the element at (0,0) is the number of zeros
 * collected, the element at (0,1) the number of ones etc. In
 * `VariableLengthOutput` mode: bins with zero entries will not be
 * present in the output. The output value is a 1-by-N PiiMatrix<int>,
 * where N is the number of non-zero histogram bins. In this mode, the
 * operation places no restrictions on the range of input values.
 *
 * @out x - the x coordinates of the histogram bins. A 1-by-N
 * PiiMatrix<int>. In `FixedLengthOutput` mode this will always be
 * the same: (0, 1, 2, ..., [binCount]-1). In `VariableLengthOutput`
 * mode the size of the matrix will be equal to that of `y`. The
 * coordinates will always be in ascending order.
 *
 */
class PiiHistogramCollector : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of histogram bins. This property must be set to a
   * non-zero value in `FixedLengthOutput` mode. In
   * `VariableLengthOutput` mode, this value works as an initial guess
   * to the number of distinct histogram bins. A good guess
   * (over-estimate) increases performance. The default value is 256.
   */
  Q_PROPERTY(int binCount READ binCount WRITE setBinCount);

  /**
   * The output mode. Default is `FixedLengthOutput`.
   */
  Q_PROPERTY(OutputMode outputMode READ outputMode WRITE setOutputMode);
  Q_ENUMS(OutputMode);

  /**
   * Output normalization. If set to `false` (the default), the
   * output will be a PiiMatrix<int> in which each column represents
   * the number of times the index appeared in input. If `true`,
   * output histogram will be a PiiMatrix<float> that is normalized so
   * that it sums up to unity .
   */
  Q_PROPERTY(bool normalized READ normalized WRITE setNormalized);

  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  /**
   * Output modes.
   *
   * - `FixedLengthOutput` - the size of the output histogram is
   * determined by [binCount]. `x` output will always be the same.
   *
   * - `VariableLengthOutput` - only non-zero bins will be output.
   * The size of the output varies, and `x` records the indices of
   * non-zero bins.
   */
  enum OutputMode { FixedLengthOutput, VariableLengthOutput };

  PiiHistogramCollector();


  void check(bool reset);

protected:
  void process();
  void syncEvent(SyncEvent* event);

  void setBinCount(int binCount);
  int binCount() const;
  void setOutputMode(const OutputMode& outputMode);
  OutputMode outputMode() const;
  void setNormalized(bool normalized);
  bool normalized() const;

private:
  void addToHistogram(int element);
  template <class T> void addPrimitive(const PiiVariant& obj);
  template <class T> void addMatrix(const PiiVariant& obj);
  void emitHistogram();

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    int iBinCount;
    bool bSyncConnected;
    PiiMatrix<int> matHistogram;
    PiiMatrix<int> matX;
    OutputMode outputMode;
    PiiVariant pSyncObject;
    bool bNormalized;
  };
  PII_D_FUNC;
};


#endif //_PIIHISTOGRAMCOLLECTOR_H
