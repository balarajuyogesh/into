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

#ifndef _PIICAMERA_H
#define _PIICAMERA_H

/**
 * Camera-related utility functions and definitions.
 *
 * @ingroup PiiCameraPlugin
 */
namespace PiiCamera
{
  /**
   * Supported image formats.
   *
   * @lip InvalidFormat - the image format is unrecognized or not yet
   * known.
   *
   * @lip MonoFormat - the image is monochrome (gray-scale)
   *
   * @lip BayerRGGBFormat - the image is Bayer-encoded in BGGR color
   * order.
   *
   * @lip BayerGBRGFormat - the image is Bayer-encoded in GBRG color
   * order.
   *
   * @lip BayerGRBGFormat - the image is Bayer-encoded in GRBG color
   * order.
   *
   * @lip Yuv422Format - the image is in YUV422-format.
   *
   * @lip Yuv411Format - the image is in YUV411-format.
   *
   * @lip RgbFormat - the image is in RGB format
   *
   * @lip BgrFormat - the image is in BGR format.
   */
  enum ImageFormat
    {
      InvalidFormat = 0,
      MonoFormat = 1,
      BayerRGGBFormat,
      BayerBGGRFormat,
      BayerGBRGFormat,
      BayerGRBGFormat,
      Yuv411Format,
      Yuv422Format,
      RgbFormat = 16,
      BgrFormat
    };

  /**
   * Supported camera types.
   *
   * @lip AreaScan - the camera type is area scan.
   *
   * @lip LineScan - the camera type is line scan.
   */
  enum CameraType
    {
      AreaScan = 0,
      LineScan
    };
}

#endif //_PIICAMERA_H
