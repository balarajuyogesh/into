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

#ifndef _PIIPOINT_H
#define _PIIPOINT_H

/**
 * A two-dimensional point. In many occasions, matrices are used in
 * storing geometric objects like points, circles (PiiCircle), sizes
 * (PiiSize), rectangles (PiiRectangle) etc. The parameters of these
 * objects are represented as row vectors. The geometric object
 * structures are provided to conveniently handle such matrices.
 *
 * ~~~(c++)
 * // A matrix that stores two points
 * PiiMatrix<int> mat(2,  2,
 *                    1,  1,
 *                   -1, -1);
 * PiiPoint<int> *pt = mat.rowAs<PiiPoint<int> >(0);
 * pt->x = 2; // modifies mat(0,0)
 * pt->y = 0; // modifies mat(0,1)
 * pt = (PiiPoint<int>*)mat.row(1);
 * pt->x = 0; // modifies mat(1,0)
 *
 * // A circle
 * PiiMatrix<float> mat2(1, 3,
 *                       0.0, 0.0, 5.0);
 * PiiCircle<float> *circle = mat2.rowAs<PiiCircle<float> >(0);
 * circle->radius += 5; // radius is now 10
 * PiiPoint<float> *pt2 = mat2.rowAs<PiiPoint<float> >(0);
 * pt2->y = 2; // modifies mat2(0,1)
 *
 * // Store the matrix row into a circle structure
 * PiiCircle<float> circle2 = *circle;
 * ~~~
 *
 */
template <class T> struct PiiPoint
{
  PiiPoint(T xVal = 0, T yVal = 0) :
    x(xVal), y(yVal)
  {}
               
  /**
   * The x coordinate.
   */
  T x;
  /**
   * The y coordinate.
   */
  T y;

  bool operator== (const PiiPoint& other) const { return x == other.x && y == other.y; }
  bool operator!= (const PiiPoint& other) const { return x != other.x || y != other.y; }
};


#endif //_PIIPOINT_H
