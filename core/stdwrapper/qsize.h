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

#ifndef _QSIZE_H
#define _QSIZE_H

class PII_CORE_EXPORT QSize
{
public:
  QSize() :
    _iWidth(-1), _iHeight(-1)
    {}
  QSize(int width, int height) :
    _iWidth(width), _iHeight(height)
  {}
  QSize(const QSize& other) :
    _iWidth(other._iWidth),
    _iHeight(other._iHeight)
  {}
  QSize& operator= (const QSize& other)
  {
    _iWidth = other._iWidth;
    _iHeight = other._iHeight;
  }

  int width() const { return _iWidth; }
  int height() const { return _iHeight; }

private:
  int _iWidth, _iHeight;
};

#endif //_QSIZE_H
