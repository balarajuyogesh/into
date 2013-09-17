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

#ifndef _PIIIMAGEDISPLAY_H
#define _PIIIMAGEDISPLAY_H

#include "PiiGui.h"
#include "PiiImageViewport.h"
#include <PiiVariant.h>
#include <PiiColor.h>
#include <PiiMatrix.h>
#include <QFrame>

class Ui_ImageDisplay;
class PiiMeasureBar;
class PiiImageScrollArea;
class QDialog;


/**
 * A class provides an image display window.
 *
 * The display automatically converts image colors to 8 bits per
 * channel. If the actual dynamic range of the color channels is
 * greater, one must scale the channels appropriately before sending
 * them to the display. Floating point color channels are assumed to
 * be at the range [0,1] and scaled accordingly.
 *
 */
class PII_GUI_EXPORT PiiImageDisplay : public QFrame
{
  Q_OBJECT

  /**
   * The display type. Color and gray-scale images are correctly shown
   * with the `Normal` type. To automatically scale images to the
   * full gray-scale range, set the displayType to `AutoScale`.
   */
  Q_PROPERTY(DisplayType displayType READ displayType WRITE setDisplayType);
  Q_ENUMS(DisplayType);

  /**
   * Show or hide measure bars. The default value is true.
   */
  Q_PROPERTY(bool showHorizontalMeasureBar READ showHorizontalMeasureBar WRITE setShowHorizontalMeasureBar);
  Q_PROPERTY(bool showVerticalMeasureBar READ showVerticalMeasureBar WRITE setShowVerticalMeasureBar);
  
public:
  /**
   * Display types.
   *
   * - `Normal` - images will be displayed without modification.
   *
   * - `AutoScale` - gray-level images will be scaled to fill the
   * full 8-bit dynamic range. This is handy if you need to display
   * binary images or floating-point images that are not in the
   * correct scale.
   */
  enum DisplayType { Normal, AutoScale };

  PiiImageDisplay(QWidget* parent = 0);
  ~PiiImageDisplay();

  void setDisplayType(DisplayType type);
  DisplayType displayType() const;

  void setShowHorizontalMeasureBar(bool show);
  bool showHorizontalMeasureBar() const;

  void setShowVerticalMeasureBar(bool show);
  bool showVerticalMeasureBar() const;
  
  /**
   * Returns the horizontal measure bar.
   */
  PiiMeasureBar* horizontalMeasureBar() const;

  /**
   * Returns the vertical measure bar.
   */
  PiiMeasureBar* verticalMeasureBar() const;

  /**
   * Returns the image view port.
   */
  PiiImageViewport* imageViewport() const;

  /**
   * Returns the image scroll area.
   */
  PiiImageScrollArea* imageScrollArea() const;

  /**
   * Creates a dialog containing a PiiImageDisplay that shows the
   * given *image*.
   *
   * ~~~
   * QDialog* pDialog = PiiImageDisplay::dialog(image); // image can be a matrix or a variant
   * pDialog->exec();
   * delete pDialog;
   * ~~~
   */
  static QDialog* dialog(const PiiVariant& image);
  static QDialog* dialog(const PiiMatrix<uchar>& image);
  static QDialog* dialog(const PiiMatrix<PiiColor4<uchar> >& image);

signals:
  /**
   * Refer to the documentation of the corresponding signal in the class
   * PiiImageViewport. 
   */
  void areaSelected(const QRect&,int);

  /**
   * Refer to the documentation of the corresponding signal in the class
   * PiiImageViewport. 
   */
  void clicked(const QPoint&,int);
  
public slots:
  /**
   * Sets the displayed image. If `image` is not of a recognized
   * type, this function does nothing.
   */
  void setImage(const PiiVariant& image, int layer = 0);

private slots:
  void viewportScaleChanged(double x, double y);
  void enableScrollBars(int mode);

private:
  void init();
  void initMeasureBars();

  class Data;
  Data* d;
};

#endif //_PIIIMAGEDISPLAY_H
