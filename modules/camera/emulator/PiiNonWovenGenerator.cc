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

#include "PiiNonWovenGenerator.h"

#include <PiiMatrix.h>

PiiNonWovenGenerator::PiiNonWovenGenerator() :
  _dRoughness(0),
  _iMeanGrayLevel(127),
  _iBondingPatternSize(0),
  _iBondingPatternStartRow(0),
  _iBondingPatternStartCol(0),
  _productQuality(MediumQuality)
{
  _fractalGenerator.setTargetMean(_iMeanGrayLevel);
  setProductQuality(MediumQuality);
}

void PiiNonWovenGenerator::setMeanGrayLevel(int meanGrayLevel)
{
  _iMeanGrayLevel = meanGrayLevel;
  _fractalGenerator.setTargetMean(_iMeanGrayLevel);
}

void PiiNonWovenGenerator::setProductQuality(ProductQuality productQuality)
{
  _productQuality = productQuality;

  switch(_productQuality)
    {
    case HighQuality:
      _dRoughness = 0.2;
      _fractalGenerator.setRoughnessScale(0.5);
      setMeanGrayLevel(150);
      break;
    case MediumQuality:
      _dRoughness = 0.4;
      _fractalGenerator.setRoughnessScale(0.7);
      setMeanGrayLevel(130);
      break;
    case LowQuality:
      _dRoughness = 0.6;
      _fractalGenerator.setRoughnessScale(0.8);
      setMeanGrayLevel(120);
      break;
    }
}

void PiiNonWovenGenerator::generateTexture(PiiMatrix<unsigned char>& buffer,
                                           int row, int column,
                                           int rows, int columns,
                                           bool first)
{
  int iFirstRow = 0;

  if (!first)
    {
      std::memcpy(_matBuffer[0], _matBuffer[_matBuffer.rows()-1], columns);
      iFirstRow = 1;
    }
  else
    _matBuffer.resize(rows+1, columns);

  int roughnessX = int(_fractalGenerator.scaleRoughness(columns,100) * _dRoughness * 100 + 0.5);

  int c1, c2;
  // If we can seed the fractal by previous data, we'll do it.
  if (iFirstRow == 1)
    {
      c1 = _matBuffer(0, column);
      c2 = _matBuffer(0, column + columns-1);
    }
  else
    {
      c1 = _iMeanGrayLevel;
      c2 = _iMeanGrayLevel;
    }

  _fractalGenerator.generateFractal(_matBuffer,
                                    iFirstRow, column, rows, columns,
                                    c1, c2,
                                    (c1 + _iMeanGrayLevel) / 2,
                                    (c2 + _iMeanGrayLevel) / 2,
                                    roughnessX);

  // Generate bonding pattern if requested
  if (_iBondingPatternSize > 0)
    // Leave the last row out so that it doesn't affect the generation
    // of the next fractal.
    generateBondingPattern(_matBuffer,
                           iFirstRow, column,
                           rows-1, columns);

  buffer(row, column, rows, columns) << _matBuffer(iFirstRow, 0, rows, columns);
}

void PiiNonWovenGenerator::generateBondingPattern(PiiMatrix<unsigned char>& buffer,
                                                  int row, int column,
                                                  int rows, int columns)
{
  int halfSize = _iBondingPatternSize >> 1;
  int meanLevel3 = _iMeanGrayLevel * 3;
  for (int r = _iBondingPatternStartRow; r < rows; r += _iBondingPatternSize)
    {
      unsigned char* pRow = buffer.row(row+r);
      for (int c = _iBondingPatternStartCol*halfSize; c < columns; c += _iBondingPatternSize)
        {
          pRow[column+c] = (pRow[column+c] + meanLevel3) >> 2;
        // Alternate starting position
        _iBondingPatternStartCol ^= 1;
        }
    }
  _iBondingPatternStartRow = _iBondingPatternSize - (rows - _iBondingPatternStartRow) % _iBondingPatternSize;
}
