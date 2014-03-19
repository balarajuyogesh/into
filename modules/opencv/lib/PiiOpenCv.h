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

#ifndef _PIIOPENCVIMAGE_H
#define _PIIOPENCVIMAGE_H

#include <cv.h>

/**
 * Definitions for OpenCV-related routines.
 */
namespace PiiOpenCv
{
  /// @hide
  template <class T> struct IplImageDepthTrait;
  template <> struct IplImageDepthTrait<char> { enum { intValue = IPL_DEPTH_8S }; };
  template <> struct IplImageDepthTrait<unsigned char> { enum { intValue = IPL_DEPTH_8U }; };
  template <> struct IplImageDepthTrait<short> { enum { intValue = IPL_DEPTH_16S }; };
  template <> struct IplImageDepthTrait<unsigned short> { enum { intValue = IPL_DEPTH_16U }; };
  template <> struct IplImageDepthTrait<int> { enum { intValue = IPL_DEPTH_32S }; };
  template <> struct IplImageDepthTrait<unsigned int> { enum { intValue = IPL_DEPTH_32S }; };
  template <> struct IplImageDepthTrait<float> { enum { intValue = IPL_DEPTH_32F }; };
  template <> struct IplImageDepthTrait<double> { enum { intValue = IPL_DEPTH_64F }; };

  template <class T> struct IplImageChannelsTrait { enum { intValue = 1 }; };
  template <class T> struct IplImageChannelsTrait<PiiColor<T> > { enum { intValue = 3 }; };
  template <class T> struct IplImageChannelsTrait<PiiColor4<T> > { enum { intValue = 4 }; };

  template <class T> struct CvMatTraits;
  template <> struct CvMatTraits<char> { enum { matrixType = CV_8SC1 }; };
  template <> struct CvMatTraits<unsigned char> { enum { matrixType = CV_8UC1 }; };
  template <> struct CvMatTraits<short> { enum { matrixType = CV_16SC1 }; };
  template <> struct CvMatTraits<unsigned short> { enum { matrixType = CV_16UC1 }; };
  template <> struct CvMatTraits<int> { enum { matrixType = CV_32SC1 }; };
  template <> struct CvMatTraits<float> { enum { matrixType = CV_32FC1 }; };
  template <> struct CvMatTraits<double> { enum { matrixType = CV_64FC1 }; };

  inline void releaseCvArr(IplImage* ptr) { cvReleaseImageHeader(&ptr); }
  inline void releaseCvArr(CvMat* ptr) { cvFree(&ptr); }

  inline bool hasData(IplImage* ptr) { return ptr->imageData != 0; }
  inline bool hasData(CvMat* ptr) { return ptr->data.ptr != 0; }
  /// @endhide
}

/// @internal
template <class Derived, class OpenCvType> class PiiConstCvArr
{
public:
  ~PiiConstCvArr() { PiiOpenCv::releaseCvArr(_pCvArr); }

  operator const CvArr* () const { setData(); return reinterpret_cast<const CvArr*>(_pCvArr); }
  operator const OpenCvType* () const { setData(); return _pCvArr; }

protected:
  PiiConstCvArr(OpenCvType* ptr) : _pCvArr(ptr) {}

  void setData()
  {
    if (_pCvArr != 0 && !PiiOpenCv::hasData(_pCvArr))
      cvSetData(this->_pCvArr, self()->row(0), self()->stride());
  }
  void setData() const
  {
    if (_pCvArr != 0 && !PiiOpenCv::hasData(_pCvArr))
      cvSetData((CvArr*)this->_pCvArr, (void*)self()->row(0), self()->stride());
  }

  OpenCvType* _pCvArr;

private:
  inline Derived* self() { return static_cast<Derived*>(this); }
  inline const Derived* self() const { return static_cast<const Derived*>(this); }
};

/// @internal
template <class Derived, class OpenCvType> class PiiCvArr : public PiiConstCvArr<Derived, OpenCvType>
{
public:
  operator CvArr* () { this->setData(); return reinterpret_cast<CvArr*>(this->_pCvArr); }
  operator OpenCvType* () { this->setData(); return this->_pCvArr; }

protected:
  PiiCvArr(OpenCvType* ptr) : PiiConstCvArr<Derived, OpenCvType>(ptr) {}
};


/**
 * A class that makes it possible to reuse the data in a PiiMatrix in
 * an IplImage. Once created, the wrapper can be substituted to CvArr*
 * or IplImage* in OpenCV function calls. The `isConst` template
 * parameter determines if the shared data can be modified.
 *
 * ~~~(c++)
 * PiiMatrix<int> threshold(const PiiMatrix<int>& input)
 * {
 *   PiiMatrix<int> matResult(50,50);
 *   cvAdaptiveThreshold(PiiOpenCv::iplImage(cvSource),
 *                       PiiOpenCv::iplImage(matResult),
 *                       15.0);
 *   return matResult;
 * }
 * ~~~
 *
 * ! This class is intended to be used only as a temporary object
 * and should not be copied. Unless C++11 support is enabled, the
 * compiler allows you to create a copy of an PiiIplImage object, but
 * the CvArr pointer of the copied object will be zero after copying.
 *
 * ~~~(c++)
 * PiiMatrix<int> matTest(10,10);
 * PiiIplImage<int> iplImage(matTest);
 * PiiIplImage<int> iplImage2(iplImage); // moves data from iplImage to iplImage2
 * cvFunction(iplImage); // Crash, the data is null.
 * ~~~
 */
template <class T, bool isConst=false> class PiiIplImage :
  public PiiMatrix<T>,
  public Pii::If<isConst, PiiConstCvArr<PiiIplImage<T>,IplImage>, PiiCvArr<PiiIplImage<T>,IplImage> >::Type
{
  typedef typename Pii::If<isConst, PiiConstCvArr<PiiIplImage<T>,IplImage>, PiiCvArr<PiiIplImage<T>,IplImage> >::Type BaseType;
public:
  /**
   * Creates a new IplImage wrapper that shares the pixel data with
   * *matrix*. If the `isConst` template parameter is `false`, the
   * shared data can be modified by OpenCV function calls. That is,
   * the class can be casted to a non-const CvArr pointer. If
   * `isConst` is `true`, a copy of the data will be created if it is
   * still shared when a pointer to a CvArr is requested.
   */
  PiiIplImage(const PiiMatrix<T>& matrix) :
    PiiMatrix<T>(matrix),
    BaseType(cvCreateImageHeader(cvSize(matrix.columns(), matrix.rows()),
                                 PiiOpenCv::IplImageDepthTrait<T>::intValue,
                                 PiiOpenCv::IplImageChannelsTrait<T>::intValue))
  {}

#ifdef PII_CXX11
  PiiIplImage(PiiIplImage&& other) :
    PiiMatrix<T>(other),
    BaseType(0)
  {
    std::swap(this->_pCvArr, other._pCvArr);
  }

  PiiIplImage(PiiMatrix<T>&& matrix) :
    PiiMatrix<T>(matrix),
    BaseType(cvCreateImageHeader(cvSize(matrix.columns(), matrix.rows()),
                                 PiiOpenCv::IplImageDepthTrait<T>::intValue,
                                 PiiOpenCv::IplImageChannelsTrait<T>::intValue))
  {}

  PII_DISABLE_COPY(PiiIplImage);
#else
  PiiIplImage(const PiiIplImage& other) :
    PiiMatrix<T>(other),
    BaseType(0)
  {
    std::swap(this->_pCvArr, const_cast<PiiIplImage&>(other)._pCvArr);
  }

private: PiiIplImage& operator= (const PiiIplImage& other);
#endif
};

/**
 * A class that makes it possible to reuse the data in a PiiMatrix in
 * an CvMat. Works similarly to PiiIplImage, but stores the data in a
 * CvMat.
 */
template <class T, bool isConst=false> class PiiCvMat :
  public PiiMatrix<T>,
  public Pii::If<isConst, PiiConstCvArr<PiiCvMat<T>,CvMat>, PiiCvArr<PiiCvMat<T>,CvMat> >::Type
{
  typedef typename Pii::If<isConst, PiiConstCvArr<PiiCvMat<T>,CvMat>, PiiCvArr<PiiCvMat<T>,CvMat> >::Type BaseType;
public:
  /**
   * Creates a new CvMat wrapper that shares the pixel data with
   * *matrix*.
   */
  PiiCvMat(const PiiMatrix<T>& matrix) :
    PiiMatrix<T>(matrix),
    BaseType(cvCreateMatHeader(matrix.rows(), matrix.columns(), PiiOpenCv::CvMatTraits<T>::matrixType))
  {}

#ifdef PII_CXX11
  PiiCvMat(PiiCvMat&& other) :
    PiiMatrix<T>(other),
    BaseType(0)
  {
    std::swap(this->_pCvArr, other._pCvArr);
  }

  PiiCvMat(PiiMatrix<T>&& matrix) :
    PiiMatrix<T>(matrix),
    BaseType(cvCreateMatHeader(matrix.rows(), matrix.columns(), PiiOpenCv::CvMatTraits<T>::matrixType))
  {}
#else
  PiiCvMat(const PiiCvMat& other) :
    PiiMatrix<T>(other),
    BaseType(0)
  {
    std::swap(this->_pCvArr, const_cast<PiiCvMat&>(other)._pCvArr);
  }

private: PiiCvMat& operator= (const PiiCvMat& other);
#endif
};

namespace PiiOpenCv
{
  /**
   * Allocates a new CvMat object with the given data type `T`. This
   * is the same as `cvCreateMat(rows, columns, type)`, but
   * saves you from using cumbersome OpenCV type constants.
   *
   * @param rows the number of rows in the matrix
   *
   * @param columns the number of columns in the matrix
   *
   * @return a newly allocated (one-channel) CvMat structure.
   * Deallocate with cvReleaseMat.
   */
  template <class T> inline CvMat* cvMat(int rows, int columns)
  {
    return cvCreateMat(rows, columns, CvMatTraits<T>::matrixType);
  }

  /**
   * Creates a modifiable wrapper for `mat` that can be used as an
   * IplImage in OpenCV function calls.
   *
   * @relates PiiIplImage
   */
  template <class T> inline PiiIplImage<T> iplImage(PiiMatrix<T>& mat)
  {
    return PiiIplImage<T>(mat);
  }

  /**
   * Creates a const wrapper for `mat` that can be used as an
   * IplImage in OpenCV function calls.
   *
   * @relates PiiIplImage
   */
  template <class T> inline PiiIplImage<T, true> iplImage(const PiiMatrix<T>& mat)
  {
    return PiiIplImage<T, true>(mat);
  }

  /**
   * Creates a modifiable wrapper for `mat` that can be used as a
   * CvMat in OpenCV function calls.
   *
   * @relates PiiCvMat
   */
  template <class T> inline PiiCvMat<T> cvMat(PiiMatrix<T>& mat)
  {
    return PiiCvMat<T>(mat);
  }

  /**
   * Creates a const wrapper for `mat` that can be used as an
   * CvMat in OpenCV function calls.
   *
   * @relates PiiCvMat
   */
  template <class T> PiiCvMat<T, true> cvMat(const PiiMatrix<T>& mat)
  {
    return PiiCvMat<T, true>(mat);
  }
}

#endif //_PIIOPENCVIMAGE_H
