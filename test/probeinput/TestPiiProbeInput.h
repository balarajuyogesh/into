/* This file is part of Into.
 * Copyright (C) Intopii 2014.
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

#ifndef _TESTPIIPROBEINPUT_H
#define _TESTPIIPROBEINPUT_H

#include <QObject>
#include <PiiProbeInput.h>

class TestPiiProbeInput : public QObject
{
  Q_OBJECT
public:
  TestPiiProbeInput();

protected slots:
  void count(const PiiVariant& obj, PiiProbeInput* sender);

private slots:
  void tryToReceive();
  void savedObject();
  void discardControlObjects();
  void signalInterval();

private:
  void sendNumbers();
  void send(const PiiVariant& obj);

  PiiProbeInput* _pProbe;
  int _iCount;
  PiiVariant _varSaved;
};


#endif //_TESTPIIPROBEINPUT_H
