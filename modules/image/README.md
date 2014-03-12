Image processing
================

The image plug-in contains operations and functions image processing
and for reading and writing images. See the [PiiImage] namespace for
low-level image processing functions.

Dependencies
------------

- [PiiDspPlugin] - Provides basic signal processing operations such as
convolution, correlation and Fourier transform.


Region of Interest
------------------

Region of interest (ROI for short) is a way of telling image
processing operations the parts of an image that need to be
processed. On the lowest level, ROIs are implemented as function
objects that return `true` for each pixel that needs to be
analyzed and `false` for the others. The actual implementation of
a ROI can vary; the only requirement is that the ROI class has the
() operator defined for two integer arguments (row and column
coordinates of a pixel, in this order). For example, PiiMatrix is a
valid ROI.

Image processing operations use the ROI as a template. With
compiler optimizations turned on, using the default ROI
(PiiImage::DefaultRoi) imposes no computational overhead.

ROIs are used with many image processing and analysis operations.
Some examples:

~~~(c++)
// Calculate histogram over an elliptical area that just fits into an image
PiiMatrix<int> histogram =
  PiiHistogram::histogram(image,
                          PiiBinary::createMask(PiiBinary::Elliptical,
                                                image.rows(),
                                                image.columns()));

// Calculate LBP histogram of an arbitrary set of pixels (mask is a binary matrix)
PiiMatrix<int> lbpHistogram = PiiLbp::basicLbp<PiiLbp::Histogram>(image, mask);
~~~

Many analysis operations (PiiLbpOperation, PiiHistogramOperation)
have an optional `roi` input that accepts all integer-valued
matrices as input. Best performance is achieved with 8-bit data
types such as `bool` and @p (unsigned) `char`. The size of the
ROI mask read from this input must match the size of the image
currently in analysis.

Additionally, the operations are able to handle rectangular ROIs. 
In a rectangular ROI, the interesting region is represented by a
set of rectangular areas. The rectangles are stored into an N-by-4
PiiMatrix<int> in which each row describes a rectangle as (x, y,
width, height). PiiImage::RoiType is used to select between these
representations of a ROI.

Operations with a `roi` input support both mask and rectangle
ROIs. By default, the `roiType` property of such an operation is
set to `AutoRoi`. In this mode, the type of the ROI is determined
by the input: a N-by-4 PiiMatrix<int> is treated as a rectangular
ROI, and all others as a mask roi. If the `roiType` property is
set to `MaskRoi`, and the `roi` input is not connected, the alpha
channel of a four-channel color image will be used as a ROI mask.

Note that the use of a ROI may change the behaviour of a feature
vector. For example, the sum of a histogram changes with different
regions of interest. This must be taken into account if the
features are used in classification.



