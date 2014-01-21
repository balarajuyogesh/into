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

#include "PiiImageScrollArea.h"
#include "PiiImageViewport.h"

#include <PiiYdinTypes.h>
#include <PiiMath.h>
#include <PiiQImage.h>
#include <PiiColor.h>
#include <PiiImageDisplay.h>

#include <QScrollBar>
#include <QtDebug>
#include <QSize>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>

PiiImageScrollArea::Data::Data() :
  pImageViewport(0),
  bCanMoveDisplay(true),
  bDragging(false),
  displayType(PiiImageDisplay::Normal),
  dStartX(0),
  dStartY(0)
{
  lstImages << 0;
}

PiiImageScrollArea::Data::~Data()
{
  qDeleteAll(lstImages);
}

PiiImageScrollArea::PiiImageScrollArea(Data *dd, QImage* image, QWidget* parent) :
  QAbstractScrollArea(parent), d(dd)
{
  d->pImageViewport = new PiiImageViewport(image);
  init();
}

PiiImageScrollArea::PiiImageScrollArea(Data *dd, const QImage& image, QWidget* parent) :
  QAbstractScrollArea(parent), d(dd)
{
  d->pImageViewport = new PiiImageViewport(image);
  init();
}

PiiImageScrollArea::PiiImageScrollArea(Data *data, QWidget* parent) :
  QAbstractScrollArea(parent), d(data)
{
  d->pImageViewport = new PiiImageViewport;
  init();
}

PiiImageScrollArea::PiiImageScrollArea(QImage* image, QWidget* parent) :
  QAbstractScrollArea(parent), d(new Data)
{
  d->pImageViewport = new PiiImageViewport(image);
  init();
}

PiiImageScrollArea::PiiImageScrollArea(const QImage& image, QWidget* parent) :
  QAbstractScrollArea(parent), d(new Data)
{
  d->pImageViewport = new PiiImageViewport(image);
  init();
}

PiiImageScrollArea::PiiImageScrollArea(QWidget* parent) :
  QAbstractScrollArea(parent), d(new Data)
{
  d->pImageViewport = new PiiImageViewport;
  init();
}

PiiImageScrollArea::~PiiImageScrollArea()
{
  delete d;
}

void PiiImageScrollArea::init()
{
  d->bCanMoveDisplay = true;
  d->bDragging = false;

  setMouseTracking(true);
  d->pImageViewport->setParent(viewport());
  connect(d->pImageViewport, SIGNAL(visibleAreaChanged(int, int, int, int)),
          this, SLOT(visibleAreaChanged(int, int, int, int)));
  connect(d->pImageViewport, SIGNAL(areaSelected(const QRect&,int)), this, SIGNAL(areaSelected(const QRect&,int)));
  connect(d->pImageViewport, SIGNAL(clicked(const QPoint&,int)), this, SIGNAL(clicked(const QPoint&,int)));
}


PiiImageViewport* PiiImageScrollArea::imageViewport() const
{
  return d->pImageViewport;
}

void PiiImageScrollArea::enterEvent(QEvent *e)
{
  QAbstractScrollArea::enterEvent(e);
  emit mouseOnView(true);
}

void PiiImageScrollArea::leaveEvent(QEvent *e)
{
  QAbstractScrollArea::leaveEvent(e);
  emit mouseOnView(false);
}

void PiiImageScrollArea::mouseMoveEvent(QMouseEvent *event)
{
  QPoint newPoint = event->pos();

  // Middle button is for dragging the image
  if (d->bDragging && (event->buttons() & Qt::MidButton))
    {
      QPoint mousePrev = d->mouseCurrPoint;

      double zoomFactor = d->pImageViewport->zoom();
      int xmove = int(double(newPoint.x()-mousePrev.x())/zoomFactor);
      int ymove = int(double(newPoint.y()-mousePrev.y())/zoomFactor);

      // This if makes sure, that moving of the image works also when
      // the image has been zoomed out very much, a.
      if (xmove != 0 || ymove != 0)
        {
          horizontalScrollBar()->setValue(horizontalScrollBar()->value()-xmove);
          verticalScrollBar()->setValue(verticalScrollBar()->value()-ymove);
          d->mouseCurrPoint = newPoint;
        }
    }
}

void PiiImageScrollArea::mousePressEvent(QMouseEvent *event)
{
  if (event->button() & Qt::MidButton)
    {
      d->mouseCurrPoint = event->pos();
      d->bDragging = true;
    }
}

void PiiImageScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() & Qt::MidButton)
    {
      d->bDragging = false;
    }
}

void PiiImageScrollArea::resizeEvent (QResizeEvent* event)
{
  if (d->pImageViewport != 0)
    {
      // Disable painting temporarily, to avoid flickering.
      setUpdatesEnabled(false);
      d->pImageViewport->resize(viewport()->size());
      setUpdatesEnabled(true);
    }
  QAbstractScrollArea::resizeEvent(event);
}

void PiiImageScrollArea::wheelEvent(QWheelEvent * event)
{
  if (0 == (Qt::ControlModifier & event->modifiers()))
    {
      int step = event->delta() < 0 ? -10 : 10;
      verticalScrollBar()->setValue(verticalScrollBar()->value() - step);
    }
}

double PiiImageScrollArea::startX() const
{
  return d->dStartX;
}

double PiiImageScrollArea::startY() const
{
  return d->dStartY;
}

void PiiImageScrollArea::visibleAreaChanged(int x, int y, int width, int height)
{
  d->dStartX = x;
  d->dStartY = y;

  emit horizontalStartPosChanged(x);
  emit verticalStartPosChanged(y);

  // It is assumed, that the image has already been moved to the
  // correct position in the image viewport. That's why we set the
  // value of this member variable temporary to false.
  d->bCanMoveDisplay = false;

  int xPageStep = qMin(width, d->pImageViewport->image()->width());
  int yPageStep = qMin(height, d->pImageViewport->image()->height());
  int xVal = qMax(0, x);
  int yVal = qMax(0, y);

  horizontalScrollBar()->setPageStep(xPageStep);
  verticalScrollBar()->setPageStep(yPageStep);

  horizontalScrollBar()->setRange(0, d->pImageViewport->image()->width() - xPageStep);
  verticalScrollBar()->setRange(0, d->pImageViewport->image()->height() - yPageStep);

  horizontalScrollBar()->setValue(xVal);
  verticalScrollBar()->setValue(yVal);

  d->bCanMoveDisplay = true;
}

void PiiImageScrollArea::scrollContentsBy(int dx, int dy)
{
  if (d->bCanMoveDisplay)
    {
      // When x-scrollbar is moved right, the parameter dx will get
      // negative values. Also when the y-scrollbar is moved down, dy will
      // get negative values. That's why we must invert the values before
      // calling the corresponding functions of PiiImageViewport.
      if (dx != 0)
        d->pImageViewport->moveCurrX(-dx);
      if (dy != 0)
        d->pImageViewport->moveCurrY(-dy);
    }
}

void PiiImageScrollArea::setImage(const PiiVariant& image, int layer)
{
  if (layer < 0 || layer > d->lstImages.size())
    return;

  // Ensure we have a pointer for each displayed layer.
  for (int i=d->lstImages.size(); i<d->pImageViewport->layerCount(); ++i)
    d->lstImages << 0;

  if (!image.isValid())
    {
      d->pImageViewport->setImage(0, layer);
      if (layer == 0)
        {
          delete d->lstImages[0];
          d->lstImages[0] = 0;
        }
      else
        delete d->lstImages.takeAt(layer);
    }
  else
    {
      switch (image.type())
        {
          PII_INTEGER_MATRIX_CASES_M(grayImage, (image, layer));
          PII_UNSIGNED_MATRIX_CASES_M(grayImage, (image, layer));
          PII_FLOAT_MATRIX_CASES_M(grayImage, (image, layer));
        case PiiYdin::UnsignedCharColorMatrixType:
          colorImage<PiiColor<unsigned char> >(image, layer);
          break;
        case PiiYdin::UnsignedCharColor4MatrixType:
          colorImage<PiiColor4<unsigned char> >(image, layer);
          break;
        case PiiYdin::BoolMatrixType: // booleans are treaded as unsigned chars
          grayImage<unsigned char>(image, layer);
          break;
        default:
          break;
        }
    }
}

template <class T> void PiiImageScrollArea::scaledImage(const PiiMatrix<T>& image, int layer)
{
  PiiMatrix<unsigned char> result(PiiMatrix<unsigned char>::uninitialized(image.rows(), image.columns()));
  T minimum, maximum;
  Pii::minMax(image, &minimum, &maximum);
  if (minimum == maximum)
    {
      view(PiiMatrix<unsigned char>(image), layer);
      return;
    }
  float scale = 255.0f / (maximum - minimum);
  int iRows = image.rows(), iCols = image.columns();
  for (int r=0; r<iRows; ++r)
    {
      const T* sourceRow = image.row(r);
      unsigned char* resultRow = result.row(r);
      for (int c=0; c<iCols; ++c)
        resultRow[c] = (unsigned char)(scale * (sourceRow[c] - minimum));
    }
  view(result, layer);
}

template <class T> void PiiImageScrollArea::grayImage(const PiiVariant& obj, int layer)
{
  if (d->displayType == PiiImageDisplay::AutoScale)
    scaledImage(obj.valueAs<PiiMatrix<T> >(), layer);
  else
    view(PiiMatrix<unsigned char>(obj.valueAs<PiiMatrix<T> >()), layer);
}

template <class T> void PiiImageScrollArea::floatImage(const PiiVariant& obj, int layer)
{
  if (d->displayType == PiiImageDisplay::AutoScale)
    scaledImage<T>(obj, layer);
  else
    view(PiiMatrix<unsigned char>(obj.valueAs<PiiMatrix<T> >() * 255), layer);
}

template <class T> void PiiImageScrollArea::colorImage(const PiiVariant& obj, int layer)
{
  view(PiiMatrix<PiiColor4<unsigned char> >(obj.valueAs<PiiMatrix<T> >()), layer);
}

template <class T> void PiiImageScrollArea::view(const PiiMatrix<T>& image, int layer)
{
  if (!image.isEmpty())
    {
      QImage *pImage = Pii::createQImage(image);
      d->pImageViewport->setImage(pImage, layer);
      delete d->lstImages[layer];
      d->lstImages[layer] = pImage;
    }
}

void PiiImageScrollArea::setDisplayType(int type) { d->displayType = type; }
int PiiImageScrollArea::displayType() { return d->displayType; }
