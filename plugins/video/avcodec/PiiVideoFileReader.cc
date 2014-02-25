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

#include "PiiVideoFileReader.h"
#include <PiiYdinTypes.h>

PiiVideoFileReader::Data::Data() :
  strFileName(""),
  iRepeatCount(1),
  pVideoReader(0),
  pFileNameInput(0),
  iFrameStep(1),
  iVideoIndex(0),
  bFileNameConnected(false),
  bTriggered(false)
{}

PiiVideoFileReader::PiiVideoFileReader() :
  PiiImageReaderOperation(new Data)
{
  PII_D;
  d->pVideoReader = new PiiVideoReader;

  addSocket(d->pFileNameInput = new PiiInputSocket("filename"));
  d->pFileNameInput->setOptional(true);
}

PiiVideoFileReader::~PiiVideoFileReader()
{
  PII_D;
  delete d->pVideoReader;
}

void PiiVideoFileReader::check(bool reset)
{
  PII_D;
  PiiImageReaderOperation::check(reset);

  d->iVideoIndex = 0;

  d->bFileNameConnected = d->pFileNameInput->isConnected();
  d->bTriggered = d->pTriggerInput->isConnected();

  if (d->strFileName.isEmpty() && !d->bFileNameConnected)
    PII_THROW(PiiExecutionException, tr("Video source cannot start because filename is empty."));

  if (!d->bFileNameConnected && (d->pVideoReader->fileName() != d->strFileName || reset))
    initializeVideoReader(d->strFileName);

  if (d->bTriggered && d->bFileNameConnected)
    PII_THROW(PiiExecutionException, tr("Both trigger and filename cannot be connected."));
}

void PiiVideoFileReader::initializeVideoReader(const QString& fileName)
{
  PII_D;
  d->pVideoReader->setFileName(fileName);
  try
    {
      d->pVideoReader->initialize();
    }
  catch (PiiVideoException& ex)
    {
      PII_THROW(PiiExecutionException, tr("Failed to initialize video reader. %1").arg(ex.message()));
    }
}

void PiiVideoFileReader::process()
{
  PII_D;
  // If file name is connected, loop the whole video
  if (d->bFileNameConnected)
    {
      d->pImageOutput->startMany();
      initializeVideoReader(PiiYdin::convertToQString(d->pFileNameInput));
      if (d->imageType == GrayScale)
        emitFrames<unsigned char>();
      else
        emitFrames<PiiColor4<> >();
      d->pImageOutput->endMany();
      return;
    }

  // maxImages is in effect only if trigger isn't connected
  if (!d->bTriggered && // trigger suspends auto-finish
      d->iMaxImages >= 0  && // images are limited
      d->iCurrentIndex >= d->iMaxImages) // we have got enough
    {
      operationStopped(); //throws
    }

  int iFrameStep = d->iFrameStep;

  if (d->bTriggered)
    {
      PiiVariant obj = d->pTriggerInput->firstObject();
      if (obj.type() == PiiVariant::IntType)
        iFrameStep *= obj.valueAs<int>();
    }

  if (d->imageType == GrayScale)
    emitFrame<unsigned char>(iFrameStep);
  else
    emitFrame<PiiColor4<> >(iFrameStep);
}

template <class T> void PiiVideoFileReader::emitFrames()
{
  PII_D;
  while (true)
    {
      PiiMatrix<T> res(d->pVideoReader->getFrame<T>());
      if (!res.isEmpty())
        d->pImageOutput->emitObject(res);
      else
        return;
    }
}

template <class T> bool PiiVideoFileReader::tryToEmitFrame(int iFrameStep)
{
  PII_D;
  PiiMatrix<T> res(d->pVideoReader->getFrame<T>(iFrameStep));
  if (!res.isEmpty())
    {
      d->pImageOutput->emitObject(res);
      ++d->iCurrentIndex;
      return true;
    }

  return false;
}

template <class T> void PiiVideoFileReader::emitFrame(int iFrameStep)
{
  PII_D;
  // Try to emit the next frame depends on frameStep.
  if (!tryToEmitFrame<T>(iFrameStep))
    {
      ++d->iVideoIndex;

      if (d->bTriggered || (d->iRepeatCount < 1 || d->iVideoIndex < d->iRepeatCount))
        {
          // The video stream must be seek to the start or end.
          if (iFrameStep > 0)
            d->pVideoReader->seekToBegin();
          else if (iFrameStep < 0)
            d->pVideoReader->seekToEnd();

          // Try to emit frame
          if (!tryToEmitFrame<T>(iFrameStep))
            operationStopped(); //stop here
        }
      else
        operationStopped(); //stop here
    }
}

QString PiiVideoFileReader::fileName() const { return _d()->strFileName; }
void PiiVideoFileReader::setFileName(const QString& fileName) { _d()->strFileName = fileName; }
int PiiVideoFileReader::repeatCount() const { return _d()->iRepeatCount; }
void PiiVideoFileReader::setRepeatCount(int cnt) { _d()->iRepeatCount = cnt; }
void PiiVideoFileReader::setFrameStep(int frameStep) { _d()->iFrameStep = frameStep; }
int PiiVideoFileReader::frameStep() const { return _d()->iFrameStep; }
