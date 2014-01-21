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

#ifndef _PIIIMAGESCROLLAREA_H
#define _PIIIMAGESCROLLAREA_H

#include <QAbstractScrollArea>
#include <QImage>

#include <PiiVariant.h>
#include <PiiMatrix.h>
#include <PiiGui.h>

class PiiImageViewport;
class QWidget;

class PII_GUI_EXPORT PiiImageScrollArea : public QAbstractScrollArea
{
  Q_OBJECT
  Q_PROPERTY(int displayType READ displayType WRITE setDisplayType);

public:
  PiiImageScrollArea(QImage *image, QWidget* parent = 0);
  PiiImageScrollArea(const QImage& image, QWidget* parent = 0);
  PiiImageScrollArea(QWidget *parent = 0);

  ~PiiImageScrollArea();

  void setDisplayType(int type);
  int displayType();

  PiiImageViewport* imageViewport() const;

  double startX() const;
  double startY() const;

protected:
  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent* event);
  void wheelEvent(QWheelEvent *event);
  void scrollContentsBy ( int dx, int dy );


  /// @internal
  class Data
  {
  public:
    Data();
    virtual ~Data();

    QList<QImage*> lstImages;

    PiiImageViewport* pImageViewport;

    // A flag, which determines, if the image display can be moved, if
    // the values of the scrollbars changes.
    bool bCanMoveDisplay;

    // Contains the dragging (moving the scrolls with the middle mouse
    // button) state of the display.
    bool bDragging;

    // This member variable is used in dragging functionality (moving
    // the image when the mouse middle button is pressed.
    QPoint mouseCurrPoint;

    // The display type.
    int displayType;

    double dStartX, dStartY;
  } *d;

  /// @internal
  PiiImageScrollArea(Data *d, QImage *image, QWidget* parent = 0);
  /// @internal
  PiiImageScrollArea(Data *d, const QImage& image, QWidget* parent = 0);
  /// @internal
  PiiImageScrollArea(Data *d, QWidget *parent = 0);

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

  /**
   * Send start pos when it changed. In pixels.
   */
  void verticalStartPosChanged(double);
  void horizontalStartPosChanged(double);
  void mouseOnView(bool);

public slots:
  /**
   * Sets the displayed image. If `image` is not of a recognized
   * type, this function does nothing.
   */
  void setImage(const PiiVariant& image, int layer = 0);

protected slots:
  void visibleAreaChanged(int x, int y, int width, int height);

private:
  template <class T> void grayImage(const PiiVariant& obj, int layer);
  template <class T> void floatImage(const PiiVariant& obj, int layer);
  template <class T> void colorImage(const PiiVariant& obj, int layer);
  template <class T> void scaledImage(const PiiMatrix<T>& image, int layer);
  template <class T> void view(const PiiMatrix<T>& image, int layer);

  /**
   * Initialize a viewport widget and variables.
   */
  void init();
};

#endif //_PIIIMAGESCROLLAREA_H
