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

#ifndef _QVARLENGTHARRAY_H
#define _QVARLENGTHARRAY_H

template <class T, int prealloc> class QVarLengthArray
{
public:
  QVarLengthArray(int size = 0) :
    _iSize(size),
    _ptr(size <= prealloc ? _array : new T[size])
  {}

  ~QVarLengthArray()
  {
    if (_ptr != _array)
      delete _ptr;
  }

  T& operator[] (int index) { return _ptr[index]; }
  const T& operator[] (int index) const { return _ptr[index]; }
  const T& at(int index) const { return _ptr[index]; }

  int size() const { return _iSize; }

private:
  int _iSize;
  T _array[prealloc];
  T* _ptr;
};

#endif //_QVARLENGTHARRAY_H
