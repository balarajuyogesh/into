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

#ifndef _PIIARCHIVEMACROS_H
#define _PIIARCHIVEMACROS_H

/**
 * @file
 *
 * Contains preprocessor macros for conveniently creating primitive
 * input/output operators for archive classes. Mostly for internal
 * use.
 *
 */

#define PII_PRIMITIVE_OPERATOR(Archive, T, Stream, dir, ref) Archive& operator dir (T ref value) { startDelim(); Stream::operator dir (value); endDelim(); return *this; }

#define PII_INTEGER_OPERATORS(Archive, Stream, dir, ref)   \
  PII_PRIMITIVE_OPERATOR(Archive, short, Stream, dir, ref) \
  PII_PRIMITIVE_OPERATOR(Archive, int, Stream, dir, ref)   \
  PII_PRIMITIVE_OPERATOR(Archive, long long, Stream, dir, ref) \
  PII_PRIMITIVE_OPERATOR(Archive, unsigned short, Stream, dir, ref)  \
  PII_PRIMITIVE_OPERATOR(Archive, unsigned int, Stream, dir, ref)    \
  PII_PRIMITIVE_OPERATOR(Archive, unsigned long long, Stream, dir, ref) \
  Archive& operator dir (bool ref value) { return operator dir ((unsigned char ref) value); } \
  Archive& operator dir (long ref value) { return operator dir ((int ref) value); } \
  Archive& operator dir (unsigned long ref value) { return operator dir ((unsigned int ref) value); }
  
#define PII_PRIMITIVE_OPERATORS(Archive, Stream, dir, ref) \
  PII_INTEGER_OPERATORS(Archive, Stream, dir, ref) \
  PII_PRIMITIVE_OPERATOR(Archive, float, Stream, dir, ref) \
  PII_PRIMITIVE_OPERATOR(Archive, double, Stream, dir, ref)
//PII_PRIMITIVE_OPERATOR(Archive, long double, Stream, dir, ref)

//These are ambiguous
//PII_PRIMITIVE_OPERATOR(Archive, unsigned char, Stream, dir, ref)
//PII_PRIMITIVE_OPERATOR(Archive, char, Stream, dir, ref)
//PII_PRIMITIVE_OPERATOR(Archive, long, Stream, dir, ref)
//PII_PRIMITIVE_OPERATOR(Archive, unsigned long, Stream, dir, ref)

#define PII_PRIMITIVE_OUTPUT_OPERATORS(Archive, Stream) \
  PII_PRIMITIVE_OPERATORS(Archive, Stream, <<, )

#define PII_PRIMITIVE_INPUT_OPERATORS(Archive, Stream) \
  PII_PRIMITIVE_OPERATORS(Archive, Stream, >>, &)
#define PII_INTEGER_INPUT_OPERATORS(Archive, Stream) \
  PII_INTEGER_OPERATORS(Archive, Stream, >>, &)


#define PII_DEFAULT_OUTPUT_OPERATORS(Archive) \
  template <class T> Archive& operator<< (T& value) { return PiiOutputArchive<Archive>::operator<<(value); } \
  template <class T> Archive& operator<< (const T& value) { return PiiOutputArchive<Archive>::operator<<(value); }

#define PII_DEFAULT_INPUT_OPERATORS(Archive) \
  template <class T> Archive& operator>> (T& value) { return PiiInputArchive<Archive>::operator>>(value); } \
  template <class T> Archive& operator>> (const T& value) { return PiiInputArchive<Archive>::operator>>(value); }

#endif //_PIIARCHIVEMACROS_H
