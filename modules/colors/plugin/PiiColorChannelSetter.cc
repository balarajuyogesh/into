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

#include "PiiColorChannelSetter.h"

#include <PiiYdinTypes.h>

#include <PiiImage.h>

PII_TYPEMAP(ColorTypeMap)
{
  PII_MAP_PUT_SELF_DEFAULT;
  PII_MAP_PUT_TEMPLATE(PiiColor4, PiiColor4<unsigned char>);
  PII_MAP_PUT(PiiColor<int>, PiiColor<unsigned short>);
};

PiiColorChannelSetter::Data::Data() :
  defaultColor(NAN,NAN,NAN,NAN),
  iFirstConnectedInput(0)
{
}

PiiColorChannelSetter::PiiColorChannelSetter() : PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  addSocket(new PiiInputSocket("image"));
  addSocket(new PiiInputSocket("channel0"));
  addSocket(new PiiInputSocket("channel1"));
  addSocket(new PiiInputSocket("channel2"));
  addSocket(new PiiInputSocket("channel3"));

  for (int i=0; i<inputCount(); ++i)
    inputAt(i)->setOptional(true);

  addSocket(new PiiOutputSocket("image"));
}

void PiiColorChannelSetter::check(bool reset)
{
  PiiDefaultOperation::check(reset);

  PII_D;
  for (d->iFirstConnectedInput = 0; d->iFirstConnectedInput < inputCount(); ++d->iFirstConnectedInput)
    if (inputAt(d->iFirstConnectedInput)->isConnected())
      break;
  if (d->iFirstConnectedInput >= inputCount())
    PII_THROW(PiiExecutionException, tr("At least one input must be connected."));
}

void PiiColorChannelSetter::process()
{
  PII_D;

  // Image input connected -> copy image and modify it
  if (d->iFirstConnectedInput == 0)
    {
      PiiVariant varImg = readInput();
      switch (varImg.type())
        {
          PII_COLOR_IMAGE_CASES(setChannels, varImg);
        default:
          PII_THROW_UNKNOWN_TYPE(inputAt(0));
        }
    }
  else
    {
      // Create output image based on input type
      PiiVariant firstObject = inputAt(d->iFirstConnectedInput)->firstObject();
      switch (firstObject.type())
        {
          PII_GRAY_IMAGE_CASES(setChannels, );
        default:
          PII_THROW_UNKNOWN_TYPE(inputAt(d->iFirstConnectedInput));
        }
    }
}

template <class Clr> void PiiColorChannelSetter::setChannels(const PiiVariant& obj)
{
  PiiMatrix<Clr> matResult(obj.valueAs<PiiMatrix<Clr> >());
  setChannels(matResult);
  emitObject(matResult);
}

template <class T> void PiiColorChannelSetter::setChannels()
{
  PII_D;
  if (!Pii::isNan(d->defaultColor.c3) || inputAt(4)->isConnected())
    {
      PiiMatrix<typename PII_MAP_TYPE(ColorTypeMap, PiiColor4<T>) > matResult;
      setChannels(matResult);
      emitObject(matResult);
    }
  else
    {
      PiiMatrix<typename PII_MAP_TYPE(ColorTypeMap, PiiColor<T>) > matResult;
      setChannels(matResult);
      emitObject(matResult);
    }
}

template <class Clr> void PiiColorChannelSetter::setChannels(PiiMatrix<Clr>& img)
{
  PII_D;
  for (int c=0; c<Clr::ChannelCount; ++c)
    {
      if (inputAt(c+1)->isConnected())
        {
          PiiVariant varChannel = readInput(c+1);
          switch (varChannel.type())
            {
              PII_GRAY_IMAGE_CASES_M(setChannel, (img, c, varChannel));
            default:
              PII_THROW_UNKNOWN_TYPE(inputAt(c+1));
            }
        }
      else if (!Pii::isNan(d->defaultColor.channel(c)))
        PiiImage::setColorChannel(img, c, typename Clr::Type(d->defaultColor.channel(c)));
    }
}

template <class T, class Clr> void PiiColorChannelSetter::setChannel(PiiMatrix<Clr>& img, int index, const PiiVariant& channel)
{
  const PiiMatrix<T>& matChannel = channel.valueAs<PiiMatrix<T> >();
  if (img.isEmpty())
    img.resize(matChannel.rows(), matChannel.columns());
  else if (matChannel.rows() != img.rows() || matChannel.columns() != img.columns())
    PII_THROW_WRONG_SIZE(inputAt(index+1), matChannel, img.rows(), img.columns());

  PiiImage::setColorChannel(img, index, matChannel);
}


void PiiColorChannelSetter::setDefaultValue0(double defaultValue0) { _d()->defaultColor.c0 = float(defaultValue0); }
double PiiColorChannelSetter::defaultValue0() const { return _d()->defaultColor.c0; }
void PiiColorChannelSetter::setDefaultValue1(double defaultValue1) { _d()->defaultColor.c1 = float(defaultValue1); }
double PiiColorChannelSetter::defaultValue1() const { return _d()->defaultColor.c1; }
void PiiColorChannelSetter::setDefaultValue2(double defaultValue2) { _d()->defaultColor.c2 = float(defaultValue2); }
double PiiColorChannelSetter::defaultValue2() const { return _d()->defaultColor.c2; }
void PiiColorChannelSetter::setDefaultValue3(double defaultValue3) { _d()->defaultColor.c3 = float(defaultValue3); }
double PiiColorChannelSetter::defaultValue3() const { return _d()->defaultColor.c3; }
