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

#ifndef _PIIVIDEOFILEREADER_H
#define _PIIVIDEOFILEREADER_H

#include "PiiVideoReader.h"

#include <PiiColor.h>
#include <PiiImageReaderOperation.h>

/**
 * An operation for reading videos from files.
 *
 * Inputs
 * ------
 *
 * @in filename - an optional input for reading in the video file
 * name. If this input is connected, the operation will send the
 * output frames on a flow level one higher than that of the input. If
 * both `trigger` and `filename` are connected, each frame within
 * the video file must be separately triggered.
 *
 */
class PII_VIDEO_EXPORT PiiVideoFileReader : public PiiImageReaderOperation
{
  Q_OBJECT

  /**
   * A video file name. An example: "videos/video1.avi". 
   */
  Q_PROPERTY(QString fileName READ fileName WRITE setFileName);

  /**
   * The number of times the source emits the video. This is useful
   * if you want to process the same video again and again. 1 means
   * once, < 1 means eternally. This property has effect only if the
   * trigger input is not connected.
   *
   */
  Q_PROPERTY(int repeatCount READ repeatCount WRITE setRepeatCount);

  /**
   * FrameStep description
   */
  Q_PROPERTY(int frameStep READ frameStep WRITE setFrameStep);
  
  
  PII_OPERATION_SERIALIZATION_FUNCTION

public:

  PiiVideoFileReader();
  ~PiiVideoFileReader();
  
  QString fileName() const;
  void setFileName(const QString& fileName);

  int repeatCount() const;
  void setRepeatCount(int cnt);

  void setFrameStep(int frameStep);
  int frameStep() const;

protected:

  void process();
  void check(bool reset);

private:
  template <class T> void emitFrames();
  template <class T> void emitFrame(int iFrameStep);
  template <class T> bool tryToEmitFrame(int iFrameStep);
  void initializeVideoReader(const QString& fileName);

  /// @internal
  class Data : public PiiImageReaderOperation::Data
  {
  public:
    Data();

    QString strFileName;
    int iRepeatCount;
    PiiVideoReader* pVideoReader;
    PiiInputSocket *pFileNameInput;
    int iFrameStep, iVideoIndex;
    bool bFileNameConnected, bTriggered;
  };
  PII_D_FUNC;
};

#endif //_PIIVIDEOFILEREADER_H
