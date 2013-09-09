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

#ifndef _TESTPIISERIALIZATION_H
#define _TESTPIISERIALIZATION_H

#include <QObject>
#include <PiiMatrix.h>

class TestPiiSerialization : public QObject
{
  Q_OBJECT

public:
  TestPiiSerialization();

private slots:
  void textArchive();
  void binaryArchive();
  void derivedTypes();

private:
  PiiMatrix<double> _dMat;
  PiiMatrix<int>  _iMat;
  PiiMatrix<char> _cMat;
  PiiMatrix<double> *_pDMat;

  template <class InputArchive, class OutputArchive> void anyArchive();
};


#endif //_TESTPIISERIALIZATION_H
