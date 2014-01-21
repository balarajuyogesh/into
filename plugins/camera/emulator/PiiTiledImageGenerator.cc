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


#include "PiiTiledImageGenerator.h"

#include <QFile>

PiiTiledImageGenerator::PiiTiledImageGenerator() : _iRowCounter(0),
                                                   _iDirection(1),
                                                   _pImage(0),
                                                   _strTileFileName("")

{
}

PiiTiledImageGenerator::~PiiTiledImageGenerator()
{
  delete _pImage;
}

void PiiTiledImageGenerator::generateTexture(PiiMatrix<unsigned char>& buffer,
                                             int row, int column,
                                             int rows, int columns,
                                             bool /*first*/)
{
  if (_pImage != 0)
    {
      int iColumns = qMin(_pImage->columns(), buffer.columns());

      for (int r=0; r<rows; r++ )
        {
          for (int c=0; c<iColumns; c++)
            buffer(r+row,c+column) = (*_pImage)(_iRowCounter,c);

          _iRowCounter += _iDirection;
          if (_iRowCounter <= 0 || _iRowCounter >= (_pImage->rows()-1))
            _iDirection = -_iDirection;
        }
    }
  else
    {
      buffer(row,column,rows,columns) = 128;
    }
}

void PiiTiledImageGenerator::setTileFileName(const QString& tileFileName)
{
  _strTileFileName = tileFileName;
  initImage();
}

void PiiTiledImageGenerator::initImage()
{
  if (QFile::exists(_strTileFileName))
    {
      delete _pImage;
      PiiGrayQImage *pImg = PiiImageFileReader::readGrayImage(_strTileFileName);
      if (pImg)
        _pImage = pImg->toMatrixPointer();
      else
        _pImage = 0;
    }
  else
    piiWarning(tr("Image file '%1' doesn't exists.").arg(_strTileFileName));
}
