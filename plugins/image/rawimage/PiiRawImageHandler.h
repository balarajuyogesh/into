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

#ifndef _PIIRAWIMAGEHANDLER_H
#define _PIIRAWIMAGEHANDLER_H

#include <QFile>
#include <QImageIOHandler>
#include <QVariant>

#include "../PiiImageGlobal.h"
#include <PiiCamera.h>

/**
 * Provides support for the *praw* image format. The praw format is a
 * simple raw image format that is mainly intended for dumping raw
 * camera data to the hard drive.
 *
 * @ingroup Image
 */
class PII_IMAGE_EXPORT PiiRawImageHandler : public QImageIOHandler
{
public:
  PiiRawImageHandler();
  PiiRawImageHandler(const QString& fileName);
  ~PiiRawImageHandler();

  bool canRead() const;
  static bool canRead(QIODevice* device);
  int currentImageNumber() const;
  QRect currentImageRect() const;
  int imageCount() const;
  bool jumpToImage(int imageNumber);
  bool jumpToNextImage();
  bool read(QImage* image);
  bool write(const QImage& image);
  bool supportsOption(ImageOption option) const;
  QVariant option(ImageOption option) const;
  
  /**
   * Writes a frame into the device. @p data points to the beginning
   * of raw image data. <tt>width*height*bytesPerPixel</tt> bytes will
   * be written into the file.
   *
   * @param data a pointer to the beginning of image data
   *
   * @param stride the number of bytes in each row. If this value is
   * less than the width of the image, the width will be used as the
   * stride.
   *
   * @return true if writing was successful, false otherwise
   */
  bool writeFrame(const void* data, unsigned int stride = 0);

  /**
   * Reads a frame from the device.
   *
   * @param buffer a preallocated image buffer. Must be able to store
   * at least <tt>width*height*bytesPerPixel</tt> bytes.
   *
   * @param stride the number of bytes in each row. If this value is
   * less than the width of the image, the width will be used as the
   * stride.
   *
   * @return true if reading was successful, false otherwise
   */
  bool readFrame(void* buffer, int stride = 0);

  /**
   * Reads raw image header from the stream. If the header has already
   * been read, returns @p true.
   */
  bool readHeader() const;
  /**
   * Starts writing a sequence of frames. This function writes an
   * initial image header to the stream. If you know the number of
   * frames in advance, you can use the #setFrameCount() function to
   * put it to the header. Otherwise, you need to use #endWriting() to
   * finish the header. If no explicit call is made, this function
   * will be called automatically when the first frame is written.
   */
  bool startWriting();
  /**
   * Ends writing a sequence of frames. This function seeks back to
   * the beginning of the device and writes the final image header
   * there. All changes made to the header will be made permanent. It
   * is not necessary to call this function if you knew and set the
   * parameters before the first frame was written.
   *
   * @note The underlying IO device must be random-access
   * (non-sequential) device.
   */
  bool endWriting();
  
  /**
   * Seeks to the given frame index.
   * 
   * @param frameIndex seek to the beginning of this frame. If
   * frameIndex is negative, seeks from the end of the sequence.
   * 
   * @return true if seek was successful, false otherwise.
   */
  bool seekFrame(int frameIndex);

  /**
   * Sets the size of stored frames. The value will be stored to the
   * file header and must be correct.
   */
  void setFrameSize(const QSize& frameSize);

  /**
   * Returns the size of stored frames. If the header hasn't been
   * read, returns (0,0).
   */
  QSize frameSize() const;

  /**
   * Sets the number of bits per pixel.
   */
  void setBitsPerPixel(unsigned int bitsPerPixel);
  /**
   * Returns the number of bits per pixel.
   */
  unsigned int bitsPerPixel() const;

  /**
   * Sets image format.
   */
  void setImageFormat(PiiCamera::ImageFormat format);
  /**
   * Returns image format.
   */
  PiiCamera::ImageFormat imageFormat() const;

  /**
   * Sets the number of frames in the rawImage. This function may be
   * used to set the frame count beforehand to avoid a call to
   * #endWriting().
   */
  void setFrameCount(int frameCount);

  /**
   * Returns the number of frames in the sequence. -1 means unknown.
   */
  int frameCount() const;

  /**
   * Returns the number of bytes each frame occupies. The buffers used
   * with #readFrame() and #writeFrame() must have a capacity of at
   * least this many bytes.
   */
  int bytesPerFrame() const;

  /**
   * Returns the number of bytes each pixel takes up.
   */
  int bytesPerPixel() const;

private:
  /**
   * RawImage file header. Note that all integers are in little-endian
   * format (LSB first).
   *
   * @internal
   */
  struct Header
  {
    enum { magicValue = 0x31415927 };
    
    Header() :
      magic(magicValue),
      width(0), height(0),
      bitsPerPixel(8),
      imageFormat(PiiCamera::MonoFormat),
      frameCount(0)
    {}
    
    /**
     * Magic number: 0x31415927.
     */
    unsigned int magic;
    /**
     * Frame width.
     */
    unsigned int width;
    /**
     * Frame height.
     */
    unsigned int height;
    /**
     * Number of bits per pixel. Only one byte is used for storing the
     * value. The other three are reserved for future use. Perhaps a
     * version number?
     */
    unsigned int bitsPerPixel;
    /**
     * Pixel format.
     */
    PiiCamera::ImageFormat imageFormat;
    /**
     * The number of frames in the file.
     */
    int frameCount;
  };

  /// @internal
  class Data
  {
  public:
    Data();
    virtual ~Data();

    Header header;
    bool bHeaderRead, bHeaderWritten;
    bool iFrameIndex;
  } *d;

  static bool read(QIODevice* device, char* data, int amount);
  static bool write(QIODevice* device, const char* data, int amount);
};

#endif //_PIIRAWIMAGEHANDLER_H
