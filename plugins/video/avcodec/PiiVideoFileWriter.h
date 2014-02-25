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

#ifndef _PIIVIDEOFILEWRITER_H
#define _PIIVIDEOFILEWRITER_H

#include <PiiDefaultOperation.h>
#include <PiiQImage.h>

#include "PiiVideoException.h"
#include "PiiVideoGlobal.h"
#include "PiiVideoWriter.h"

/**
 * An operation that writes images into video files in standard video
 * formats. If a video with the same name already exists,
 * it will be overwritten.
 *
 * Inputs
 * ------
 *
 * @in image - video frames, any gray-level or color image
 *
 */
class PII_VIDEO_EXPORT PiiVideoFileWriter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The directory the writer will store the video in. Relative and
   * absolute paths are OK. An example: "videos/".
   */
  Q_PROPERTY(QString outputDirectory READ outputDirectory WRITE setOutputDirectory);

  /**
   * The output file name.
   */
  Q_PROPERTY(QString fileName READ fileName WRITE setFileName);

  /**
   * Frame rate.
   */
  Q_PROPERTY(int frameRate READ frameRate WRITE setFrameRate);


  PII_OPERATION_SERIALIZATION_FUNCTION

public:
  PiiVideoFileWriter();
  virtual ~PiiVideoFileWriter();

  QString outputDirectory() const;
  void setOutputDirectory(const QString& dirName);

  QString fileName() const;
  void setFileName(const QString& fileName);

  int frameRate() const;
  void setFrameRate(int frameRate);

protected:
  void process();

private slots:
  void deletePiiVideoWriter(PiiOperation::State state);

private:
  template <class T> void initPiiVideoWriter(const PiiVariant& obj);
  template <class T> void grayImage(const PiiVariant& obj);
  template <class T> void floatImage(const PiiVariant& obj);
  template <class T> void colorImage(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    QString strOutputDirectory, strFileName;
    int iIndex, iWidth, iHeight, iFrameRate;

    PiiVideoWriter *pVideoWriter;
    PiiInputSocket* pImageInput;
  };
  PII_D_FUNC;
};


#endif //_PIIVIDEOFILEWRITER_H
