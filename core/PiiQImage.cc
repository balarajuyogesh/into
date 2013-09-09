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

#include "PiiQImage.h"

#include <QVector>
#include <QMap>
#include <QPaintEngine>

#include <QAtomicInt>

int iQImagePtrTypeId = qRegisterMetaType<QImagePtr>("QImagePtr");

// Taken from src/gui/image/qimage.cpp
struct PrivateQImageData
{
  QAtomicInt ref;

  int width;
  int height;
  int depth;
  int nbytes;               // number of bytes data
#if QT_VERSION >= 0x050000
  qreal devicePixelRatio;
#endif
  QVector<QRgb> colortable;
  uchar *data;
#if QT_VERSION < 0x050000
  uchar **jumptable;
#endif
  QImage::Format format;
  int bytes_per_line;
  int ser_no;               // serial number
#if (QT_VERSION >= 0x040200)
  int detach_no;
#endif

  qreal  dpmx;                // dots per meter X (or 0)
  qreal  dpmy;                // dots per meter Y (or 0)
  QPoint  offset;           // offset in pixels
  uint own_data : 1;
#if (QT_VERSION >= 0x040302)
  uint ro_data : 1;
#endif
  uint has_alpha_clut : 1;
#if QT_VERSION >= 0x050000
  uint is_cached : 1;
  uint is_locked : 1;
#endif
};

class PrivateQImage : public QPaintDevice
{
public:
  PrivateQImageData *d;
};


namespace Pii
{
  bool hasOwnData(QImage* image)
  {
    PrivateQImageData* d = ((PrivateQImage*)image)->d;
    if (d)
      return (bool)d->own_data;
    else
      return false;
  }
  
  void setQImageData(QImage* image, uchar* data)
  {
    PrivateQImageData* d = ((PrivateQImage*)image)->d;
    d->data = data;
  }
  
  void setQImageWidth(QImage* image, int width)
  {
    PrivateQImageData* d = ((PrivateQImage*)image)->d;
    d->width = width;
    d->bytes_per_line = width * d->depth;
    d->nbytes = d->height * d->bytes_per_line;
  }

  void setQImageRowLength(QImage* image, int bytesPerLine)
  {
    PrivateQImageData* d = ((PrivateQImage*)image)->d;
    d->bytes_per_line = bytesPerLine;
    d->nbytes = d->height * d->bytes_per_line;
  }

  void setQImageFormat(QImage* image, QImage::Format format)
  {
    PrivateQImageData *d = ((PrivateQImage*)image)->d;
    d->format = format;
  }
  
  uchar* releaseQImageBits(QImage* image)
  {
    PrivateQImageData* d = reinterpret_cast<PrivateQImage*>(image)->d;
    //qDebug("sizeof(QImage): %ld. sizeof(PrivateQImage): %ld", sizeof(QImage), sizeof(PrivateQImage));
    /*qDebug("Releasing data of %p. Width: %d, Height: %d, Format: %d, Bpl: %d, Serial: %d, Own: %d",
           image,
           d->width, d->height, d->format, d->bytes_per_line, d->ser_no, d->own_data);
    */
    if (d != 0)
      {
        d->own_data = false;
        return d->data;
      }
    return 0;
  }

  void convertToGray(QImage& img)
  {
    if (img.depth() == 8 && img.isGrayscale())
      return;
    
    const int iRows = img.height(), iCols = img.width();
    QImage gsImg(iCols, iRows, QImage::Format_Indexed8);
    QRgb color;
    unsigned char *gsData;
    for (int r=0; r<iRows; ++r)
      {
        gsData = (unsigned char*)gsImg.scanLine(r);
        for (int c=0; c<iCols; ++c)
          {
            color = img.pixel(c, r);
            //gray value is the average of r, g, and b.
            gsData[c] = (unsigned char)((qRed(color) + qGreen(color) + qBlue(color))/3);
          }
      }
    img = gsImg;
  }

  void convertToRgba(QImage& img)
  {
    if (img.format() == QImage::Format_RGB32)
      return;
    
    const int iRows = img.height(), iCols = img.width();
    QImage rgbImg(iCols, iRows, QImage::Format_RGB32);
    QRgb *rgbData;
    
    for (int r=0; r<iRows; ++r)
      {
        rgbData = (QRgb*)rgbImg.scanLine(r);
        for (int c=0; c<iCols; ++c)
          rgbData[c] = img.pixel(c, r);
      }
    img = rgbImg;
  }

  PiiGrayQImage* readGrayImage(const QString& fileName)
  {
    QImage img;
    if (!img.load(fileName))
      return 0;
    
    convertToGray(img);
    return PiiGrayQImage::create(img);
  }
  
  PiiColorQImage* readColorImage(const QString& fileName)
  {
    QImage img;
    if (!img.load(fileName))
      return 0;

    convertToRgba(img);
    return PiiColorQImage::create(img);
  }
}
