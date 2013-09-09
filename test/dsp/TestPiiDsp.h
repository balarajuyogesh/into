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

#ifndef _TESTPIIDSP_H
#define _TESTPIIDSP_H

#include <QObject>

class TestPiiDsp : public QObject
{
  Q_OBJECT

private slots:
  void fftShift();
  void fft();
  void correlation();
  void normalizedCorrelation();
  void convolution();
  void fastCorrelation();
  void findPeaks();
};


#endif //_TESTPIIDSP_H
