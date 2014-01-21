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

#ifndef _PII_H
#define _PII_H

#ifndef PII_NO_QT
#  include <QObject>
#endif

#include "PiiGlobal.h"

/**
 * This namespace contains general-purpose functions, type
 * definitions, and data structures that are independent of other
 * modules and plug-ins.
 *
 */
#ifdef Q_MOC_RUN
class Pii
#else
namespace Pii
#endif
{
#ifdef Q_MOC_RUN
  Q_GADGET

  Q_ENUMS(Interpolation);
public:
#endif
#ifndef PII_NO_QT
  extern PII_CORE_EXPORT const QMetaObject staticMetaObject;
#endif

  /**
   * Pointer ownership modes. If the ownership of a pointer is
   * released to the receiver in a function or constructor call,
   * `ReleaseOwnership` is used. If the ownership is retained by the
   * caller, this is indicated by `RetainOwnership`.
   */
  enum PtrOwnership { ReleaseOwnership, RetainOwnership };

  /**
   * Interpolation is needed in many occasions where discrete signals
   * are sampled at arbitrary positions. Many operations work with
   * multiple interpolation modes.
   *
   * - `NearestNeighborInterpolation` means that the interpolated
   * value is that of the nearest discretized neighbor.
   *
   * - `LinearInterpolation` means that the interpolated value is
   * built by a weighted sum of the nearest neighbors. With
   * two-dimensional signals, linear interpolation is in fact
   * bi-linear.
   */
  enum Interpolation { NearestNeighborInterpolation, LinearInterpolation };

  /**
   * An enumeration that specifies the direction of operation for
   * certain matrix operations.
   */
  enum MatrixDirection { Horizontally = 1, Vertically = 2 };
  Q_DECLARE_FLAGS(MatrixDirections, MatrixDirection);
  Q_FLAGS(MatrixDirection MatrixDirections);

}; // namespace Pii

/// @hide
Q_DECLARE_OPERATORS_FOR_FLAGS(Pii::MatrixDirections);

#endif //_PII_H
