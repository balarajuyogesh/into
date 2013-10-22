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

#ifndef _PIITILEDIMAGEGENERATOR_H
#define _PIITILEDIMAGEGENERATOR_H

#include "PiiTextureGenerator.h"
#include <PiiMatrix.h>
#include <PiiImageFileReader.h>

/**
 * A texture generator that produces tiled image background
 * texture.
 *
 */
class PII_CAMERAEMULATOR_EXPORT PiiTiledImageGenerator : public PiiTextureGenerator
{
  Q_OBJECT

  /**
   * tileFileName description
   */
  Q_PROPERTY(QString tileFileName READ getTileFileName WRITE setTileFileName);
  
  
public:
  PiiTiledImageGenerator();
  ~PiiTiledImageGenerator();
  
  void generateTexture(PiiMatrix<unsigned char>& buffer,
                       int row, int column,
                       int rows, int columns,
                       bool first = false);

  void setTileFileName(const QString& tileFileName);
  QString getTileFileName() const { return _strTileFileName; }


private:
  void initImage();
  QImage convertColorToGray(QImage img);
  
  int _iRowCounter, _iDirection;
  PiiMatrix<unsigned char> *_pImage;
  QString _strTileFileName;
};


#endif //_PIITILEDIMAGEGENERATOR_H
