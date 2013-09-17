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

#ifndef _PIIKERNELFUNCTION_H
#define _PIIKERNELFUNCTION_H

#include "PiiDistanceMeasure.h"

/**
 * @file
 */

#ifdef PII_CXX0X
template <class SampleSet> using PiiKernelFunction = PiiDistanceMeasure<SampleSet>
#else
#  define PiiKernelFunction PiiDistanceMeasure
#endif

/**
 * Type definition for a polymorphic implementation of the function
 * object *KERNEL*.
 *
 * ~~~
 * PiiKernelFunction<ConstFeatureIterator>* pKernel = new PII_POLYMORPHIC_KERNEL(PiiGaussianKernel);
 * ~~~
 */
#define PII_POLYMORPHIC_KERNEL(KERNEL) typename PiiKernelFunction<ConstFeatureIterator>::template Impl<KERNEL<ConstFeatureIterator> >

#endif //_PIIKERNELFUNCTION_H
