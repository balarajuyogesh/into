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

#include "PiiBits.h"

using namespace std;

namespace Pii
{
  int countOnes(unsigned int c, const unsigned char bits)
  {
    int count = 0;
    for (int i=0;i<bits;i++)
      {
        count += c & 1; //add the lsb
        c >>= 1; //take next bit to lsb
      }
    return count;
  }

  int countTransitions(unsigned int c, const unsigned char bits)
  {
    return countOnes(c ^ ror(c, 1, bits));
  }

  unsigned int rotateToMinimum(unsigned int n, const unsigned char bits)
  {
    unsigned int tmp = n << sizeof(int)*4;
    unsigned int lowmask = INT_MAX >> (sizeof(int)*4-1);
    unsigned int min = tmp;
    int minIndex=0;
    for (int i=1;i<bits;i++)
      {
        tmp >>= 1;
        tmp |= (tmp & lowmask) << bits;
        tmp &= ~lowmask;
        if (tmp<min)
          {
            min = tmp;
            minIndex = i;
          }
      }
    tmp = n << (sizeof(int)*4 - minIndex);
    tmp |= (tmp & lowmask) << bits;
    tmp &= ~lowmask;
    return tmp >> sizeof(int)*4;
  }

  int hammingDistance(unsigned int a, unsigned int b, const unsigned char bits)
  {
    return countOnes(a^b, bits); //find differing bits
  }
}

