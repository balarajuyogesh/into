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

#ifndef _PIICIRCLE_H
#define _PIICIRCLE_H

/**
 * Stores the center and radius of a circle. See PiiPoint for usage
 * examples.
 *
 */
template <class T> struct PiiCircle
{
  PiiCircle(T xVal = 0, T yVal = 0, T r = 0) :
    x(xVal), y(yVal), radius(r)
  {}

  /**
   * The x coordinate of the center.
   */
  T x;
  /**
   * The y coordinate of the center.
   */
  T y;
  /**
   * The radius of the circle.
   */
  T radius;

  bool operator== (const PiiCircle& other) const { return x == other.x && y == other.y && radius == other.radius; }
  bool operator!= (const PiiCircle& other) const { return x != other.x || y != other.y || radius != other.radius; }
};


#endif //_PIICIRCLE_H
