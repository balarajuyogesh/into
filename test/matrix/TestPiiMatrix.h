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

#ifndef _TESTPIIMATRIX_H
#define _TESTPIIMATRIX_H

#include <QObject>
#include <PiiMatrix.h>

class TestPiiMatrix : public QObject
{
  Q_OBJECT

private slots:
  void constructors();
  void iterators();
  void alignment();
  void assignment();
  void selfAssignment();
  void transpose();
  void logic();
  void resizing();
  void integerMath();
  void doubleMath();
  void rowHandling();
  void removeRows();
  void removeColumns();
  void multiply();
  void submatrix();
  void uninitialized();
  void reserve();

private:
  template <class Matrix> void setTo(Matrix& matrix, typename Matrix::value_type value);
  template <class Matrix> void setSubmatrix(Matrix& matrix, typename Matrix::value_type value);
};

#endif //_TESTPIIMATRIX_H
