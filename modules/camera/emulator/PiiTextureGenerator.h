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

#ifndef _PIITEXTUREGENERATOR_H
#define _PIITEXTUREGENERATOR_H

#include <QObject>
#include <PiiSerializationUtil.h>
#include <PiiMatrix.h>
#include <PiiCameraEmulatorGlobal.h>

/**
 * An interface for classes that produce background texture for
 * PiiLineScanEmulator. You may create your own generator by
 * subclassing this interface. Register the class to the resource
 * database (see PiiYdin::resourceDatabase()) and set the
 * [textureGenerator](PiiLineScanEmulator::textureGenerator) property
 * in PiiLineScanEmulator to the name of your class.
 *
 */
class PII_CAMERAEMULATOR_EXPORT PiiTextureGenerator : public QObject
{
  Q_OBJECT

  PII_PROPERTY_SERIALIZATION_FUNCTION(QObject)
public:
  ~PiiTextureGenerator();

  /**
   * Generate texture to the given frame buffer.
   *
   * @param buffer the image buffer
   *
   * @param row start generating at this row
   *
   * @param col start generating at this column
   *
   * @param rows the number of rows to generate
   *
   * @param columns the number of columns to generate
   *
   * @param first if `false`, the texture should form a contiuous
   * surface with previous data at `row`-1.
   *
   * ! Access the buffer in a circular manner. When the frame
   * buffer in PiiLinescanEmulator is full, it will be filled from the
   * beginning again. The emulator will not do the circular indexing
   * for you. Thus, it may happen that `row` points to the last row
   * in `buffer` while `rows` is larger than one.
   */
  virtual void generateTexture(PiiMatrix<unsigned char>& buffer,
                               int row, int column,
                               int rows, int columns,
                               bool first = false) = 0;
};

#define PII_SERIALIZABLE_QOBJECT PiiTextureGenerator
#define PII_SERIALIZABLE_IS_ABSTRACT
#define PII_BUILDING_LIBRARY PII_BUILDING_CAMERAEMULATOR

#include <PiiSerializableRegistration.h>

#endif //_PIITEXTUREGENERATOR_H
