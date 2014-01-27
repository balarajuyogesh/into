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

#include "PiiRawImageHandler.h"

#include <QFile>
#include <QString>
#include <QSize>
#include <QRect>
#include <QImage>

#include <PiiDelay.h>
#include <PiiColorTable.h>

PiiRawImageHandler::Data::Data() :
  bHeaderRead(false),
  bHeaderWritten(false)
{
}

PiiRawImageHandler::Data::~Data()
{
}

PiiRawImageHandler::PiiRawImageHandler() :
  d(new Data)
{
}

PiiRawImageHandler::~PiiRawImageHandler()
{
  delete d;
}

bool PiiRawImageHandler::canRead() const
{
  return canRead(device());
}

bool PiiRawImageHandler::canRead(QIODevice* device)
{
  if (!device)
    return false;
  char buffer[4];
  if (device->peek(buffer, 4) != 4)
    return false;
  return *reinterpret_cast<int*>(buffer) == Header::magicValue;
}

bool PiiRawImageHandler::supportsOption(ImageOption option) const
{
  return option == Size || option == ImageFormat;
}

QVariant PiiRawImageHandler::option(ImageOption option) const
{
  switch(option)
    {
    case Size:
      readHeader();
      return frameSize();
    case ImageFormat:
      return QImage::Format_Indexed8;
    default:
      return QVariant();
    }
}

int PiiRawImageHandler::currentImageNumber() const
{
  return d->iFrameIndex;
}

QRect PiiRawImageHandler::currentImageRect() const
{
  return QRect(QPoint(0, 0), frameSize());
}

int PiiRawImageHandler::imageCount() const
{
  readHeader();
  return d->header.frameCount;
}

bool PiiRawImageHandler::jumpToImage(int imageNumber)
{
  return seekFrame(imageNumber);
}

bool PiiRawImageHandler::jumpToNextImage()
{
  return seekFrame(d->iFrameIndex + 1);
}

bool PiiRawImageHandler::read(QImage* image)
{
  readHeader();
  if (image->size() != frameSize() || image->format() != QImage::Format_Indexed8)
    {
      *image = QImage(frameSize(), QImage::Format_Indexed8);
      image->setColorTable(Pii::grayColorTable());
    }
  return readFrame(image->bits());
}

bool PiiRawImageHandler::write(const QImage& image)
{
  setFrameCount(1);
  setFrameSize(image.size());
  return writeFrame(image.bits(), image.bytesPerLine());
}

void PiiRawImageHandler::setFrameSize(const QSize& frameSize)
{
  d->header.width = frameSize.width();
  d->header.height = frameSize.height();
}

QSize PiiRawImageHandler::frameSize() const
{
  return QSize(d->header.width, d->header.height);
}

void PiiRawImageHandler::setBitsPerPixel(unsigned int bitsPerPixel)
{
  d->header.bitsPerPixel = bitsPerPixel;
}

bool PiiRawImageHandler::write(QIODevice* device, const char* data, int amount)
{
  while (amount > 0)
    {
      int iWritten = device->write(data, amount);
      if (iWritten == -1)
        return false;
      amount -= iWritten;
      data += iWritten;
    }
  return true;
}

bool PiiRawImageHandler::read(QIODevice* device, char* data, int amount)
{
  bool bWaited = false;
  while (amount > 0)
    {
      int iRead = device->read(data, amount);
      if (iRead == -1)
        return false;
      else if (iRead == 0)
        {
          if (bWaited)
            return false;
          PiiDelay::msleep(10);
          bWaited = true;
        }
      amount -= iRead;
      data += iRead;
    }
  return true;
}

bool PiiRawImageHandler::writeFrame(const void* frameBuffer, unsigned int stride)
{
  if (!device()->isOpen() || !startWriting())
    return false;

  const char* pData = static_cast<const char*>(frameBuffer);
  if (stride <= d->header.width * bytesPerPixel())
    return write(device(), pData, bytesPerFrame());
  else
    {
      int iBytesPerRow = d->header.width * bytesPerPixel();
      for (int r=d->header.height; r--; pData += stride)
        if (!write(device(), pData, iBytesPerRow))
          return false;
    }

  ++d->header.frameCount;
  return true;
}

bool PiiRawImageHandler::readFrame(void* frameBuffer, int stride)
{
  if (!device()->isOpen() || device()->atEnd())
    return false;

  char* pData = static_cast<char*>(frameBuffer);
  if (stride <= bytesPerFrame())
    return read(device(), pData, bytesPerFrame());
  else
    {
      int iBytesPerRow = d->header.width * bytesPerPixel();
      for (int r=d->header.height; r--; pData += stride)
        if (!read(device(), pData, iBytesPerRow))
          return false;
    }
  return true;
}

bool PiiRawImageHandler::seekFrame(int frameIndex)
{
  if (!device()->isOpen())
    return false;

  if (frameIndex < 0)
    {
      // Negative index and total number unknown
      if (frameCount() < 0)
        return false;
      // Index from the end
      frameIndex += frameCount();
      if (frameIndex < 0)
        return false;
    }
  else if (frameIndex >= frameCount())
    return false;

  bool bSuccess = device()->seek(sizeof(Header) + bytesPerFrame() * frameIndex);
  if (bSuccess)
    d->iFrameIndex = frameIndex;
  return bSuccess;
}

bool PiiRawImageHandler::readHeader() const
{
  Data* d = const_cast<PiiRawImageHandler*>(this)->d;
  if (d->bHeaderRead)
    return true;
  d->bHeaderRead = true;
  return
    device()->read(reinterpret_cast<char*>(&d->header), sizeof(Header)) == sizeof(Header) &&
    d->header.magic == Header::magicValue;
}

bool PiiRawImageHandler::startWriting()
{
  if (d->bHeaderWritten)
    return true;
  d->bHeaderWritten = true;
  return device()->write(reinterpret_cast<const char*>(&d->header), sizeof(Header)) == sizeof(Header);
}

bool PiiRawImageHandler::endWriting()
{
  if (!device()->seek(0))
    return false;
  d->bHeaderWritten = false;
  return device()->write(reinterpret_cast<const char*>(&d->header), sizeof(Header));
}

unsigned int PiiRawImageHandler::bitsPerPixel() const { return d->header.bitsPerPixel; }
void PiiRawImageHandler::setImageFormat(PiiCamera::ImageFormat format) { d->header.imageFormat = format; }
PiiCamera::ImageFormat PiiRawImageHandler::imageFormat() const { return d->header.imageFormat; }
void PiiRawImageHandler::setFrameCount(int frameCount) { d->header.frameCount = frameCount; }
int PiiRawImageHandler::frameCount() const { return d->header.frameCount; }

int PiiRawImageHandler::bytesPerPixel() const
{
  return d->header.bitsPerPixel / 8;
}

int PiiRawImageHandler::bytesPerFrame() const
{
  return d->header.height * d->header.width * bytesPerPixel();
}
