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

#ifndef _PIIMATRIXVALUE_H
#define _PIIMATRIXVALUE_H

/**
 * A structure that stores a value and its location in a matrix.
 *
 */
template <class T> struct PiiMatrixValue
{
  PiiMatrixValue(T val=0, int r=0, int c=0) :
    value(val), row(r), column(c)
  {}

  bool operator<(const PiiMatrixValue& other) const { return value < other.value; }
  bool operator>(const PiiMatrixValue& other) const { return value > other.value; }
  bool operator<=(const PiiMatrixValue& other) const { return value <= other.value; }
  bool operator>=(const PiiMatrixValue& other) const { return value >= other.value; }
  bool operator==(const PiiMatrixValue& other) const { return value == other.value; }
  bool operator!=(const PiiMatrixValue& other) const { return value != other.value; }

  T value;
  int row;
  int column;
};

#endif //_PIIMATRIXVALUE_H
