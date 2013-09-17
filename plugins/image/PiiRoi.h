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

#ifndef _PIIROI_H
#define _PIIROI_H

#include <PiiMatrix.h>
#include <PiiExecutionException.h>
#include <PiiVariant.h>
#include <PiiRectangle.h>
#include <PiiInputSocket.h>
#include <PiiOutputSocket.h>
#include <PiiYdinTypes.h>
#include "PiiImageGlobal.h"


/**
 * @file
 * Utilities for ROI (region-of-interest) processing
 */

namespace PiiImage
{
  /**
   * Default region-of-interest function object for feature extraction
   * and image analysis methods that support ROI processing. This
   * function returns `true` for all pixels, and reduces to a no-op
   * with compiler optimizations turned on.
   */
  struct DefaultRoi
  {
    /**
     * Check if a pixel is within a region-of-interest. Each ROI
     * functor must implement a function with this signature
     * (inline/const are optional). This function returns `true`.
     *
     * @param r the row coordinate of a pixel
     *
     * @param c the column coordinate of a pixel
     */
    inline bool operator() (int r, int c) const { Q_UNUSED(r); Q_UNUSED(c); return true; }
  };

  /**
   * A region-of-interest function object that returns `true` if the
   * alpha channel has a non-zero value at (r,c) and `false`
   * otherwise. If there is no alpha channel in the color type `T`,
   * returns always `true`.
   */
  template <class T> struct AlphaRoi : DefaultRoi
  {
    AlphaRoi(const PiiMatrix<T>&) {}
  };

  template <class T> struct AlphaRoi<PiiColor4<T> >
  {
    AlphaRoi(const PiiMatrix<PiiColor4<T> >& img) : _image(img) {}
    
    inline bool operator() (int r, int c) const { return _image(r,c).rgbaA != 0; }

  private:
    const PiiMatrix<PiiColor4<T> > _image;
  };

  /**
   * Converts any integer matrix to a boolean matrix. If `obj` is an
   * invalid variant, an empty matrix will be returned.
   *
   * @exception PiiExecutionException& if the type of the object is
   * not suitable for a ROI.
   */
  PII_IMAGE_EXPORT PiiMatrix<bool> toRoiMask(const PiiVariant& obj);

  /**
   * Creates a binary ROI mask out of a set of rectangular regions.
   *
   * @param rows the number of rows in the result image
   *
   * @param columns the number of columns in the result image
   *
   * @param rectangles a N-by-4 matrix in which each row represents a
   * rectangle (x, y, width, height). If a rectangle exceeds the
   * boundaries of the result image, it will be ignored.
   *
   * @return a *rows* -by- *columns* binary image in which all
   * pixels that are in any of the *rectangles* will be set to 
   * `true`.
   */
  PII_IMAGE_EXPORT PiiMatrix<bool> createRoiMask(int rows, int columns,
                                                 const PiiMatrix<int>& rectangles);

  /**
   * Returns `true` if any two of the given *rectangles* overlap
   * each other and `false` otherwise.
   */
  PII_IMAGE_EXPORT bool overlapping(const PiiMatrix<int>& rectangles);

  /// @internal
  PII_IMAGE_EXPORT extern const char* roiRectangleSizeError;
  /// @internal
  PII_IMAGE_EXPORT extern const char* roiMaskSizeError;

  /**
   * Returns the alpha channel of `image` as a boolean mask. Non-zero
   * entries in the alpha channel will be `true` in the returned
   * mask.
   */
  template <class T> PiiMatrix<bool> alphaToMask(const PiiMatrix<PiiColor4<T> >& image);
  
  /**
   * @internal
   *
   * Reads a ROI object from *input* and handles *image* based on it
   * and *roiType*. Uses *process* to actually perform the image
   * processing operation.
   */
  template <class T, class Processor> void handleRoiInput(PiiInputSocket* input,
                                                          RoiType roiType,
                                                          const PiiMatrix<T>& image,
                                                          Processor& process);
}

#include "PiiRoi-templates.h"

#endif //_PIIROI_H
