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

#ifndef _PIIPERCEPTRONOPERATION_H
#define _PIIPERCEPTRONOPERATION_H

#include "PiiPerceptron.h"
#include <PiiClassifierOperation.h>

/**
 * An Ydin-compatible Perceptron classifier operation. Perceptron
 * is a supervised learning machine. Thus,
 * it does have a class label input. See the
 * description of input and output sockets in PiiClassifierOperation.
 * 
 * To instantiate the operation from an object registry, one must
 * specify the data type, e.g "PiiPerceptronOperation<double>". 
 *
 * @see PiiClassifierOperation
 *
 * @ingroup PiiClassificationPlugin
 */
class PiiPerceptronOperation : public PiiClassifierOperation
{
  Q_OBJECT

public:
  template <class SampleSet> class Template;

  PiiPerceptronOperation();
};

#include "PiiPerceptronOperation-templates.h"

#endif // _PIIPERCEPTRONOPERATION_H 
