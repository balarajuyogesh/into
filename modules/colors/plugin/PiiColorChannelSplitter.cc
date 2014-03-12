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

#include "PiiColorChannelSplitter.h"
#include <PiiYdinTypes.h>
#include <PiiColor.h>

using namespace Pii;
using namespace PiiYdin;

PiiColorChannelSplitter::Data::Data() :
  pImageInput(0)
{
}

PiiColorChannelSplitter::PiiColorChannelSplitter() :
  PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;

  d->pImageInput = new PiiInputSocket("image");
  addSocket(d->pImageInput);

  setNumberedOutputs(4,0,"channel");
}

PiiColorChannelSplitter::~PiiColorChannelSplitter()
{
}

void PiiColorChannelSplitter::process()
{
  PII_D;

  PiiVariant obj = d->pImageInput->firstObject();

  switch (obj.type())
    {
    case UnsignedCharColorMatrixType:
      splitChannels<PiiColor<unsigned char>, 3>(obj);
      break;
    case UnsignedShortColorMatrixType:
      splitChannels<PiiColor<unsigned short>, 3>(obj);
      break;
    case FloatColorMatrixType:
      splitChannels<PiiColor<float>, 3>(obj);
      break;
    case UnsignedCharColor4MatrixType:
      splitChannels<PiiColor4<unsigned char>, 4>(obj);
      break;
    default:
      PII_THROW_UNKNOWN_TYPE(d->pImageInput);
    }
}

template <class Color, int channels> void PiiColorChannelSplitter::splitChannels(const PiiVariant& obj)
{
  typedef typename Color::Type T;
  const PiiMatrix<Color> image = obj.valueAs<PiiMatrix<Color> >();
  PiiMatrix<T> channelImages[channels];

  for (int i=0; i<channels; ++i)
    channelImages[i] = PiiMatrix<T>(PiiMatrix<T>::uninitialized(image.rows(), image.columns()));

  const int iRows = image.rows(), iCols = image.columns();
  for (int r=0; r<iRows; ++r)
    {
      const Color* row = image.row(r);
      T *row0 = channelImages[0][r],
        *row1 = channelImages[1][r],
        *row2 = channelImages[2][r],
        *row3;

      if (channels == 4)
        row3 = channelImages[3][r];

      for (int c=0; c<iCols; ++c)
        {
          row0[c] = row[c].c0;
          row1[c] = row[c].c1;
          row2[c] = row[c].c2;
          if (channels == 4)
            row3[c] = row[c].channels[3];
        }
    }

  for (int i=0; i<channels; ++i)
    outputAt(i)->emitObject(channelImages[i]);
}
