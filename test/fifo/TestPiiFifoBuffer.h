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

#ifndef _TESTPIIFIFOBUFFER_H
#define _TESTPIIFIFOBUFFER_H

#include <QThread>
#include <QObject>
#include <QIODevice>
#include <cstdlib>

#define BUFFERSIZE 1024

class RWBase : public QThread
{
public:
  RWBase(QIODevice* device) :  _pDevice(device), _iIndex(0), _iBlockSize(1), _iBufferSize(BUFFERSIZE)
  {
    reset();
  }

  void setBlockSize(int size) { _iBlockSize = size; }
  virtual void reset() { _iIndex = 0; }
  unsigned char* getArray() { return _array; }
  void setBufferSize(int size) { _iBufferSize = size; }

protected:
  QIODevice* _pDevice;
  int _iIndex, _iBlockSize, _iBufferSize;
  unsigned char _array[BUFFERSIZE];
};

class Writer : public RWBase
{
public:
  Writer(QIODevice* device) : RWBase(device) {}

  void reset(bool random = true)
  {
    RWBase::reset();
    if (random)
      for (int i=0; i<BUFFERSIZE; i++)
        _array[i] = (unsigned char)std::rand();
    else
      for (int i=0; i<BUFFERSIZE; i++)
        _array[i] = (unsigned char)i;
  }

protected:
  void run()
  {
    while (_iIndex < _iBufferSize)
      {
        int remaining = _iBufferSize - _iIndex;
        int toBeWritten = _iBlockSize < remaining ? _iBlockSize : remaining;
        int wrote = _pDevice->write((char*)_array + _iIndex, toBeWritten);
        if (wrote != toBeWritten)
          qCritical("Could not write everything! %d != %d", toBeWritten, wrote);
        _iIndex += toBeWritten;
      }
    //qDebug("Everything written out!");
  }
};

class Reader : public RWBase
{
public:
  Reader(QIODevice* device) : RWBase(device) {}

protected:
  void run()
  {
    while (_iIndex < _iBufferSize)
      {
        int remaining = _iBufferSize - _iIndex;
        int toBeRead = _iBlockSize < remaining ? _iBlockSize : remaining;
        int read = _pDevice->read((char*)_array + _iIndex, toBeRead);
        if (read != toBeRead)
          qCritical("Could not read everything! %d != %d", toBeRead, read);
        _iIndex += toBeRead;
      }
    //qDebug("Everything read!");
  }
};


class TestPiiFifoBuffer : public QObject
{
  Q_OBJECT

private slots:
  void oneThread();
  void oneThread_data();
  void twoThreads();
  void twoThreads_data();
};

#endif //_TESTPIIFIFOBUFFER_H
