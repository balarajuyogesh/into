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

#ifndef _PIIBITS_H
#define _PIIBITS_H

#include <climits>
#include "PiiGlobal.h"

#ifndef INTBITS
#  define INTBITS (sizeof(int)*8)
#endif

namespace Pii
{
  /**
   * @group bits Bit Fiddling
   *
   * Functions for tweaking single bits. Some functions in this
   * group come with inline assembly versions for maximum
   * performance.
   */

  /**
   * Rotate a binary code right by `n` positions.
   *
   * @param c the number to be rotated
   * @param n the number of positions to rotate
   * @param bits the number of bits to consider
   */
  inline unsigned int ror(unsigned int c, const unsigned char n, const unsigned char bits)
  {
    return ((c >> n) | (c << (bits - n))) & ((1 << bits) - 1);
  }

  template <unsigned char bits> inline unsigned int ror(unsigned int c, const unsigned char n)
  {
    return ((c >> n) | (c << (bits - n))) & ((1 << bits) - 1);
  }

  template <> inline unsigned int ror<INTBITS>(unsigned int c, const unsigned char n)
  {
    return ((c >> n) | (c << (INTBITS - n)));
  }

  /**
   * Rotate a binary code left by `n` positions.
   *
   * @param c the number to be rotated
   * @param n the number of positions to rotate
   * @param bits the number of bits to consider
   */
  inline unsigned int rol(unsigned int c, const unsigned char n, const unsigned char bits)
  {
    return ((c << n) | (c >> (bits-n))) & ((1 << bits) - 1);
  }
  template <unsigned char bits> inline unsigned int rol(unsigned int c, const unsigned char n)
  {
    return ((c << n) | (c >> (bits-n))) & ((1 << bits) - 1);
  }
  template <> inline unsigned int rol<INTBITS>(unsigned int c, const unsigned char n)
  {
    return ((c << n) | (c >> (INTBITS-n)));
  }

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
  inline unsigned int rol(unsigned int c, const unsigned char n)
  {
    asm ("roll %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }

  inline unsigned int ror(unsigned int c, const unsigned char n)
  {
    asm ("rorl %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }

  inline unsigned short rol(unsigned short c, const unsigned char n)
  {
    asm ("rolw %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }

  inline unsigned short ror(unsigned short c, const unsigned char n)
  {
    asm ("rorw %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }

  inline unsigned char rol(unsigned char c, const unsigned char n)
  {
    asm ("rolb %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }

  inline unsigned char ror(unsigned char c, const unsigned char n)
  {
    asm ("rorb %2, %0"
         : "=r" (c) // c is output value, in any register
         : "0" (c), "c" (n) // c is also input value, in the same register. n must be in cl.
         );
    return c;
  }
#else
  inline unsigned int rol(unsigned int c, const unsigned char n) { return rol<32>(c, n); }
  inline unsigned int ror(unsigned int c, const unsigned char n) { return ror<32>(c, n); }
  inline unsigned short rol(unsigned short c, const unsigned char n) { return (unsigned short)rol<16>(c, n); }
  inline unsigned short ror(unsigned short c, const unsigned char n) { return (unsigned short)ror<16>(c, n); }
  inline unsigned char rol(unsigned char c, const unsigned char n) { return (unsigned char)rol<16>(c, n); }
  inline unsigned char ror(unsigned char c, const unsigned char n) { return (unsigned char)ror<16>(c, n); }
#endif

  /**
   * Get the number of ones in a binary number.
   *
   * @param c the number
   * @param bits the number of bits to consider
   */
  PII_CORE_EXPORT int countOnes(unsigned int c, const unsigned char bits = INTBITS);

  template <unsigned char bits> int countOnes(unsigned int c)
  {
    int count = 0;
    for (int i=0;i<bits;i++)
      {
        count += c & 1; //add the lsb
        c >>= 1; //take next bit to lsb
      }
    return count;

  }

  /**
   * Get the number of 0-to-1 or 1-to-0 transitions in a binary
   * number. The number of transitions is calculated circularly. For
   * example, the number of transitions in the 8-bit number
   * "00011001" is four.
   *
   * @param c the number
   * @param bits the number of bits to consider
   */
  PII_CORE_EXPORT int countTransitions(unsigned int c, const unsigned char bits = INTBITS);

  template <unsigned char bits> int countTransitions(unsigned int c)
  {
    return countOnes<bits>(c ^ ror<bits>(c, 1));
  }


  /**
   * Rotate a binary number to its minimum value.
   *
   * @param n the number
   * @param bits the number of bits to consider
   */
  PII_CORE_EXPORT unsigned int rotateToMinimum(unsigned int n, const unsigned char bits = INTBITS);

  template <unsigned char bits> unsigned int rotateToMinimum(unsigned int n)
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


  /**
   * Calculate the Hamming distance between two binary numbers.
   *
   * @param a a number
   * @param b another number
   * @param bits the number of bits to consider
   */
  PII_CORE_EXPORT int hammingDistance(unsigned int a, unsigned int b, const unsigned char bits = INTBITS);

  template <unsigned char bits> int hammingDistance(unsigned int a, unsigned int b)
  {
    return countOnes<bits>(a^b); //find differing bits
  }

  /**
   * Get a binary mask for the sign bit of any integer type. To get
   * the sign bit, do the following:
   *
   * ~~~(c++)
   * int i = -1;
   * int sign = i & signMask<int>();
   * ~~~
   */
  template <class T> inline T signMask();

  template <> inline short signMask() { return SHRT_MIN; }
  template <> inline int signMask() { return INT_MIN; }
  template <> inline long signMask() { return LONG_MIN; }
  template <> inline ushort signMask() { return SHRT_MIN; }
  template <> inline uint signMask() { return INT_MIN; }
  template <> inline ulong signMask() { return LONG_MIN; }

  /**
   * Find the index of the first bit set to one. The function returns
   * 0 if a one is found on the first bit position and so on. If the
   * number is zero, -1 is returned.
   */
  template <class T> int firstOneBit(T c)
  {
    if (c == 0)
      return -1;

    for (int i=0; i<int(sizeof(T)*8); i++)
      {
        if (c & 1)
          return i;
        c >>= 1;
      }
    return 0;
  }

  /**
   * Find the index of the last bit set to one. The function returns 0
   * if a one is found on the first bit position and so on. If the
   * number is zero, -1 is returned.
   */
  template <class T> int lastOneBit(T c)
  {
    if (c == 0)
      return -1;

    for (int i=sizeof(T)*8; i--;)
      {
        if (c & signMask<T>())
          return i;
        c <<= 1;
      }
    return -1;
  }

  /**
   * Align a memory address. Memory alignment is useful when you want
   * to speed up access to a certain memory location.
   *
   * @param address the memory address to be aligned
   *
   * @param bitMask create a new memory address in which all bits set
   * in this mask are zero. To align an address to a 16-byte boundary,
   * use 0xf as the bit mask. 0xff aligns to a 256-byte boundary etc.
   * Different values can be used to align memory to word and/or cache
   * boundaries. The actual values of these depend on the underlying
   * processor architecture.
   *
   * @return a new memory address rounded up to the next address
   * divisible by `bitMask` + 1. If `address` is already aligned,
   * it'll be returned unmodified.
   */
  template <class T> inline T alignAddress(T address, std::size_t bitMask)
  {
    return T((std::size_t(address) + bitMask) & ~bitMask);
  }

  /* NOTE
   * The signBit functions assume that
   * a) sizeof(float) == sizeof(int)
   * b) sizeof(double) == 2*sizeof(int)
   */

#if __FLOAT_WORD_ORDER != __BYTE_ORDER
#  error "Your floating point byte order differs from integer byte order."
#endif

  /**
   * Returns the sign bit of *value1* - *value2*.
   */
  template <class T> inline unsigned int signBit(T value1, T value2)
  {
    //Calculate difference and take the sign bit.
    return static_cast<unsigned int>(value1 - value2) & INT_MIN;
  }

  /**
   * Returns the sign bit of *value1* - *value2*.
   */
  inline unsigned int signBit(float value1, float value2)
  {
    //Same as above, but here we trick the float to be an integer
    union { float _f; unsigned int _ui; } value = { value1 - value2 };
    return (value._ui & INT_MIN);
  }

  /**
   * Returns the sign bit of *value1* - *value2*.
   */
  inline unsigned int signBit(double value1, double value2)
  {
    //Same as above, but now the trick is to take only the 32 MSBs.
    //Unfortunately, this also means that byte order needs to be taken
    //into account.
    union { double _d; unsigned int _ui[2]; } value = { value1 - value2 };
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return (value._ui[1] & INT_MIN);
#elif __BYTE_ORDER == BIG_ENDIAN
    return (value._ui[0] & INT_MIN);
#else
    return value._d < 0 ? (unsigned int)INT_MIN ? 0;
#endif
  }

  /**
   * Returns the sign bit of *value1* - *value2*. This function
   * always uses the floating point version of signBit().
   */
  template <class T> inline unsigned int floatSignBit(T value1, float value2)
  {
    return signBit((float)value1, value2);
  }

  /**
   * Returns the sign bit of *value1* - *value2*. This function
   * always uses the floating point version of signBit().
   */
  template <> inline unsigned int floatSignBit(double value1, float value2)
  {
    return signBit(value1, (double)value2);
  }

  /// @endgroup
}

#endif //_PIIBITS_H
