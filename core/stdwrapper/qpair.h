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

#ifndef _QPAIR_H
#define _QPAIR_H

#include <utility>
#define QPair std::pair

template <class A, class B>
inline QPair<A,B> qMakePair(const A& a, const B& b)
{
  return std::make_pair(a,b);
}

#endif //_QPAIR_H
