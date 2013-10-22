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

#include "PiiSomOperation.h"
#include <PiiSerializationUtil.h>

template class PiiSomOperation::Template<PiiMatrix<float> >;
template class PiiSomOperation::Template<PiiMatrix<double> >;

PiiSomOperation::Data::Data() :
  PiiVectorQuantizerOperation::Data(PiiClassification::NonSupervisedLearner |
                                    PiiClassification::OnlineLearner),
  size(10, 10)
{
}

PiiSomOperation::PiiSomOperation(Data* dat) :
  PiiVectorQuantizerOperation(dat)
{
  PII_D;
  addSocket(d->pXOutput = new PiiOutputSocket("x"));
  //d->pXOutput->setDisplayName(tr("X coordinate of closest cell"));
  addSocket(d->pYOutput = new PiiOutputSocket("y"));
  //d->pYOutput->setDisplayName(tr("Y coordinate of closest cell"));
  protectProps();
}

void PiiSomOperation::protectProps()
{
  static const char* protectedProps[] =
    {
      "size",
      "width",
      "height",
      "topology",
      "rateFunction",
      "iterationNumber",
      "learningLength",
      "initialRadius",
      "initialLearningRate",
      "initMode",
      "learningAlgorithm"
    };
  for (unsigned i=0; i<sizeof(protectedProps)/sizeof(protectedProps[0]); ++i)
    setProtectionLevel(protectedProps[i], WriteWhenStoppedOrPaused);
}

/* TODO
QVariant PiiSomOperation::socketProperty(PiiAbstractSocket* socket, const char* name) const
{
  const PII_D;

  if (!PiiYdin::isNameProperty(name))
    {
      QString strPropertyName(name);
      if (socket == d->pXOutput || socket == d->pYOutput)
        {
          if (strPropertyName == "min")
            return 0;
          else if (strPropertyName == "resolution")
            return 1;
          else if (strPropertyName == "max")
            return socket == d->pXOutput ? d->size.width() : d->size.height();
          else if (strPropertyName == "displayName")
            return socket == d->pXOutput ? tr("x") : tr("y");
        }
    }

  return PiiVectorQuantizerOperation::socketProperty(socket,name);
}
*/

void PiiSomOperation::setSize(QSize size) { _d()->size = size; }
QSize PiiSomOperation::size() const { return _d()->size; }
int PiiSomOperation::width() const { return _d()->size.width(); }
void PiiSomOperation::setWidth(int width) { _d()->size.setWidth(width); }
int PiiSomOperation::height() const { return _d()->size.height(); }
void PiiSomOperation::setHeight(int height) { _d()->size.setHeight(height); }
