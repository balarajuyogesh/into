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

#include "PiiBoundaryFinder.h"

PiiBoundaryFinder::Data::Data(int rows, int columns,
                              PiiMatrix<unsigned char>* boundaryMask) :
  pmatBoundaryMask(boundaryMask),
  iRow(rows-1),
  iColumn(columns-1),
  iRightEdge(iColumn)
{
  if (boundaryMask != 0)
    pmatBoundaryMask->resize(rows, columns);
  else
    {
      matBoundaryMask.resize(rows, columns);
      pmatBoundaryMask = &matBoundaryMask;
    }
}

PiiBoundaryFinder::PiiBoundaryFinder(int rows, int columns,
                                     PiiMatrix<unsigned char>* boundaryMask) :
  d(new Data(rows, columns, boundaryMask))
{
}

PiiBoundaryFinder::~PiiBoundaryFinder()
{
  delete d;
}

PiiMatrix<unsigned char> PiiBoundaryFinder::boundaryMask() const { return d->matBoundaryMask; }
