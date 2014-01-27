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

#include "PiiImageViewport.h"
#include "PiiImageOverlay.h"
#include <PiiMath.h>

#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QPaintEvent>
#include <QToolTip>
#include <QtDebug>
#include <QPen>
#include <QColor>

PiiImageViewport::Data::Data() :
  dZoomFactor(1.0),
  dZoomStep(1.1),
  dMinZoomFactor(.01),
  dMaxZoomFactor(100),
  pixelSize(1.0, 1.0),
  dXScale(1.0), dYScale(1.0), dAspectRatio(1.0),
  bShowOverlayColoring(true),
  pUpdater(0),
  pAdapter(0),
  selectionMode(Area),
  bDrawGrid(false),
  dHLineStart(0),
  dHLineStep(0),
  dVLineStart(0),
  dVLineStep(0),
  iHLineCount(0),
  iVLineCount(0),
  pPopupMenu(0)
{
  lstLayers << new Layer;
}

PiiImageViewport::Data::~Data()
{
  pUpdater->stop();
  pUpdater->wait();
  delete pUpdater;
  qDeleteAll(lstLayers);
}

PiiImageViewport::PiiImageViewport(QImage* pImage, QWidget *parent) :
  QWidget(parent),
  d(new Data)
{
  init();
  setImage(pImage); //Sets the pointer to the QImage object
}

PiiImageViewport::PiiImageViewport(const QImage& image, QWidget *parent) :
  QWidget(parent),
  d(new Data)
{
  init();
  setImage(image); //Sets the QImage object
}

PiiImageViewport::PiiImageViewport(QWidget *parent) :
  QWidget(parent),
  d(new Data)
{
  init();
  setImage(0);
}

PiiImageViewport::PiiImageViewport(Data *dd, QImage* pImage, QWidget *parent) :
  QWidget(parent),
  d(dd)
{
  init();
  setImage(pImage); //Sets the pointer to the QImage object
}

PiiImageViewport::PiiImageViewport(Data *dd, const QImage& image, QWidget *parent) :
  QWidget(parent),
  d(dd)
{
  init();
  setImage(image); //Sets the QImage object
}

PiiImageViewport::PiiImageViewport(Data *dd, QWidget *parent) :
  QWidget(parent),
  d(dd)
{
  init();
  setImage(0);
}

PiiImageViewport::~PiiImageViewport()
{
  delete d;
}

void PiiImageViewport::setPixelSize(const QSizeF& pixelSize)
{
  d->pixelSize = pixelSize;
  if (d->pixelSize.width() <= 0)
    d->pixelSize.setWidth(1);
  if (d->pixelSize.height() <= 0)
    d->pixelSize.setHeight(1);

  d->dAspectRatio = d->pixelSize.width() / d->pixelSize.height();
  updateZoomFactors();
}

void PiiImageViewport::setDrawGrid(bool drawGrid)
{
  d->bDrawGrid = drawGrid;
  update();
}

bool PiiImageViewport::drawGrid() const { return d->bDrawGrid; }

void PiiImageViewport::updateZoomFactors()
{
  if (d->dAspectRatio >= 1)
    {
      d->dXScale = d->dZoomFactor * d->dAspectRatio;
      d->dYScale = d->dZoomFactor;
    }
  else
    {
      d->dXScale = d->dZoomFactor;
      d->dYScale = d->dZoomFactor / d->dAspectRatio;
    }

  emit scaleChanged(d->dXScale, d->dYScale);
}

/* Initializes the class. Is called from the constructors.
 */
void PiiImageViewport::init()
{
  setObjectName("imageViewport");
  d->pAdapter = this;

  // Speed up painting a bit
  setAttribute(Qt::WA_OpaquePaintEvent);

  d->pNoFitAction = new QAction(tr("&No Fit"), this);
  d->pNoFitAction->setShortcut(QKeySequence(tr("Ctrl+1")));
  d->pNoFitAction->setShortcutContext(Qt::WidgetShortcut);
  d->pNoFitAction->setCheckable(true);
  connect(d->pNoFitAction, SIGNAL(toggled(bool)), this, SLOT(setNoFit(bool)));

  d->pFitToViewAction = new QAction(tr("&Fit to View"), this);
  d->pFitToViewAction->setCheckable(true);
  d->pFitToViewAction->setShortcut(QKeySequence(tr("Ctrl+2")));
  d->pFitToViewAction->setShortcutContext(Qt::WidgetShortcut);
  connect(d->pFitToViewAction, SIGNAL(toggled(bool)), this, SLOT(setFitToView(bool)));

  d->pFillViewAction = new QAction(tr("Fill &View"), this);
  d->pFillViewAction->setCheckable(true);
  d->pFillViewAction->setShortcut(QKeySequence(tr("Ctrl+3")));
  d->pFillViewAction->setShortcutContext(Qt::WidgetShortcut);
  connect(d->pFillViewAction, SIGNAL(toggled(bool)), this, SLOT(setFillView(bool)));

  QActionGroup* fitActionGroup = new QActionGroup(this);
  fitActionGroup->addAction(d->pFitToViewAction);
  fitActionGroup->addAction(d->pFillViewAction);
  fitActionGroup->addAction(d->pNoFitAction);
  d->pNoFitAction->setChecked(true);

  d->pZoomInAction = new QAction(tr("Zoom In"), this);
  d->pZoomInAction->setShortcut(QKeySequence("+"));
  d->pZoomInAction->setShortcutContext(Qt::WidgetShortcut);
  connect(d->pZoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

  d->pZoomOutAction = new QAction(tr("Zoom Out"), this);
  d->pZoomOutAction->setShortcut(QKeySequence("-"));
  d->pZoomOutAction->setShortcutContext(Qt::WidgetShortcut);
  connect(d->pZoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

  d->pActualSizeAction = new QAction(tr("Set Actual Size"), this);
  d->pActualSizeAction->setShortcut(QKeySequence("="));
  d->pActualSizeAction->setShortcutContext(Qt::WidgetShortcut);
  connect(d->pActualSizeAction, SIGNAL(triggered()), this, SLOT(actualSize()));

  d->pShowOverlayColoringAction = new QAction(tr("&Overlay Coloring"), this);
  d->pShowOverlayColoringAction->setCheckable(true);
  d->pShowOverlayColoringAction->setChecked (d->bShowOverlayColoring);
  connect(d->pShowOverlayColoringAction, SIGNAL(triggered(bool)), this, SLOT(setShowOverlayColoring(bool)));

  addAction(d->pNoFitAction);
  addAction(d->pFitToViewAction);
  addAction(d->pFillViewAction);
  addAction(d->pZoomInAction);
  addAction(d->pZoomOutAction);
  addAction(d->pActualSizeAction);

  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  setFocusPolicy(Qt::ClickFocus);
  setMouseTracking(true);
  setCursor(Qt::CrossCursor);

  d->pUpdater = new PiiImageViewportUpdater(this);
  connect(d->pUpdater, SIGNAL(imageReady()), this, SLOT(update()));

  d->pUpdater->startThread();
}

void PiiImageViewport::setShowOverlayColoring(bool val)
{
  d->bShowOverlayColoring = val;
  updateImage();
}

void PiiImageViewport::addOverlay(PiiImageOverlay* overlay)
{
  d->overlays.append(overlay);
}

void PiiImageViewport::setOverlays(const QList<PiiImageOverlay*>& overlays)
{
  d->overlays.clear();
  d->overlays = overlays;
  updateImage();
}

void PiiImageViewport::removeOverlay(PiiImageOverlay* overlay)
{
  //If value is zero, everything is removed.
  if(overlay == 0)
    d->overlays.clear();
  else
    d->overlays.removeAll(overlay);
}

QMenu* PiiImageViewport::popupMenu(const QPoint&) const
{
  if (d->pPopupMenu)
    return d->pPopupMenu;

  d->pPopupMenu = new QMenu(tr("&Display"), const_cast<PiiImageViewport*>(this));

  d->pPopupMenu->addAction(d->pZoomInAction);
  d->pPopupMenu->addAction(d->pZoomOutAction);
  d->pPopupMenu->addAction(d->pActualSizeAction);
  d->pPopupMenu->addSeparator();
  d->pPopupMenu->addAction(d->pNoFitAction);
  d->pPopupMenu->addAction(d->pFitToViewAction);
  d->pPopupMenu->addAction(d->pFillViewAction);

  d->pShowOverlayColoringAction->setChecked (d->bShowOverlayColoring);

  if (d->overlays.size() > 0)
    {
      d->pPopupMenu->addSeparator();
      d->pPopupMenu->addAction(d->pShowOverlayColoringAction);
    }

  return d->pPopupMenu;
}

void PiiImageViewport::setImage(QImage* image, int layer)
{
  if (layer < 0 || layer > d->lstLayers.size())
    return;

  d->imageLock.lock();

  // It is assumed here, that the image object given as a parameter
  // won't be deleted during life time of this object.
  d->lstLayers[layer]->setImage(image);

  if (layer == 0)
    d->imageRect = d->lstLayers[0]->pImage->rect();

  d->imageLock.unlock();

  checkFitMode();
  focusImage();
  updateImage();
}

void PiiImageViewport::setImage(const QImage& image, int layer)
{
  d->imageLock.lock();

  // Make a shallow copy of the image, enabling the implicit memory sharing.
  d->lstLayers[layer]->setImage(image);
  if (layer == 0)
    d->imageRect = d->lstLayers[0]->pImage->rect();

  d->imageLock.unlock();

  checkFitMode();
  focusImage();
  updateImage();
}

void PiiImageViewport::setOpacity(double opacity, int layer)
{
  if (layer >= 0 && layer < d->lstLayers.size())
    {
      d->lstLayers[layer]->dOpacity = opacity;
      updateImage();
    }
}

double PiiImageViewport::opacity(int layer) const
{
  if (layer >= 0 && layer < d->lstLayers.size())
    return d->lstLayers[layer]->dOpacity;
  return 0;
}

void PiiImageViewport::setLayerVisible(bool show, int layer)
{
  if (layer >= 0 && layer < d->lstLayers.size())
    {
      d->lstLayers[layer]->bVisible = show;
      updateImage();
    }
}

bool PiiImageViewport::isLayerVisible(int layer) const
{
  return layer >= 0 && layer < d->lstLayers.size() && d->lstLayers[layer]->bVisible;
}


void PiiImageViewport::addLayer(QImage* image, qreal opacity)
{
  d->imageLock.lock();
  d->lstLayers << new Layer(image, opacity);
  d->imageLock.unlock();
  updateImage();
}

void PiiImageViewport::addLayer(const QImage& image, qreal opacity)
{
  d->imageLock.lock();
  d->lstLayers << new Layer(image, opacity);
  d->imageLock.unlock();
  updateImage();
}

void PiiImageViewport::removeLayer(int index)
{
  if (index > 0 && index < d->lstLayers.size())
    {
      delete d->lstLayers.takeAt(index);
      updateImage();
    }
}

int PiiImageViewport::layerCount() const
{
  return d->lstLayers.size();
}

void PiiImageViewport::zoomIn()
{
  d->pNoFitAction->setChecked(true);
  setZoom(d->dZoomFactor*d->dZoomStep);
  focusImage(PiiImageViewport::FocusToMouseCursor);
  updateImage();
}

void PiiImageViewport::zoomOut()
{
  d->pNoFitAction->setChecked(true);
  setZoom(d->dZoomFactor/d->dZoomStep);
  focusImage(PiiImageViewport::FocusToMouseCursor);
  updateImage();
}

void PiiImageViewport::actualSize()
{
  d->pNoFitAction->setChecked(true);
  setZoom(1);
  focusImage(PiiImageViewport::FocusToWidgetTopLeft);
  updateImage();
}

void PiiImageViewport::setZoom(double val)
{
  d->dZoomFactor = val;

  // Check that the zoom factor is within the valid range.
  if (d->dZoomFactor < d->dMinZoomFactor)
    d->dZoomFactor = d->dMinZoomFactor;
  else if (d->dZoomFactor > d->dMaxZoomFactor)
    d->dZoomFactor = d->dMaxZoomFactor;

  updateZoomFactors();
}

/* This function cheks, which is current fit mode, and scales the
   image accordingly. Scaling is done by setting the zoom factor to a
   appropriate value. */
void PiiImageViewport::checkFitMode()
{
  if (d->imageRect.isNull())
    return;

  if (d->pFitToViewAction->isChecked())
    scaleToFit();
  else if (d->pFillViewAction->isChecked())
    scaleToFill();
}

void PiiImageViewport::setSelectionMode(SelectionMode selectionMode)
{
  d->selectionMode = selectionMode;
}

void PiiImageViewport::setFitMode(FitMode mode)
{
  switch (mode)
    {
    case NoFit:
      d->pNoFitAction->setChecked(true);
      break;
    case FitToView:
      d->pFitToViewAction->setChecked(true);
      break;
    case FillView:
      d->pFillViewAction->setChecked(true);
      break;
    }
}

PiiImageViewport::SelectionMode PiiImageViewport::selectionMode() const
{
  return d->selectionMode;
}


PiiImageViewport::FitMode PiiImageViewport::fitMode() const
{
  if (d->pNoFitAction->isChecked())
    return NoFit;
  else if (d->pFitToViewAction->isChecked())
    return FitToView;
  else
    return FillView;
}

void PiiImageViewport::fitToView()
{
  setFitMode(FitToView);
}

void PiiImageViewport::setNoFit(bool checked)
{
  if (checked)
    emit fitModeChanged(NoFit);
}

void PiiImageViewport::scaleToFit()
{
  d->dXScale = double(width()) / d->imageRect.width();
  d->dYScale = double(height()) / d->imageRect.height();
  if (d->dXScale / d->dAspectRatio <= d->dYScale)
    {
      d->dZoomFactor = d->dXScale;
      d->dYScale = d->dXScale / d->dAspectRatio;
    }
  else
    {
      d->dZoomFactor = d->dYScale;
      d->dXScale = d->dYScale * d->dAspectRatio;
    }
  emit scaleChanged(d->dXScale, d->dYScale);
}

void PiiImageViewport::scaleToFill()
{
  d->dXScale = double(width()) / d->imageRect.width();
  d->dYScale = double(height()) / d->imageRect.height();
  if (d->dXScale / d->dAspectRatio > d->dYScale)
    {
      d->dZoomFactor = d->dXScale;
      d->dYScale = d->dXScale / d->dAspectRatio;
    }
  else
    {
      d->dZoomFactor = d->dYScale;
      d->dXScale = d->dYScale * d->dAspectRatio;
    }
  emit scaleChanged(d->dXScale, d->dYScale);
}

void PiiImageViewport::setFitToView(bool checked)
{
  if (checked && !d->imageRect.isNull())
    {
      emit fitModeChanged(FitToView);
      scaleToFit();
      focusImage(PiiImageViewport::FocusToWidgetTopLeft);
      updateImage();
    }
  else
    emit fitModeChanged(NoFit);
}

void PiiImageViewport::setFillView(bool checked)
{
  if (checked && !d->imageRect.isNull())
    {
      emit fitModeChanged(FillView);
      scaleToFill();
      focusImage(PiiImageViewport::FocusToWidgetTopLeft);
      updateImage();
    }
  else
    emit fitModeChanged(NoFit);
}

void PiiImageViewport::keyPressEvent(QKeyEvent* event)
{
  if ( event->key() == Qt::Key_PageUp)
    emit pageUpPressed();
  else if ( event->key() == Qt::Key_PageDown)
    emit pageDownPressed();
  else if ( event->key() == Qt::Key_Escape)
    emit escPressed();

  // The event must be delivered always to the parent, because for
  // example pageUp key might have multiple meanings in different
  // situations.
  QWidget::keyPressEvent(event);
}

void PiiImageViewport::mouseMoveEvent(QMouseEvent* event)
{
  emit mouseLocationChanged(event->pos());

  if (d->imageRect.isNull())
    return;

  d->mouseCurrPoint = event->pos();
  QPoint imagePoint = pointFromWidget2Image(d->mouseCurrPoint);

  if (d->selectionMode != NoSelection &&
      event->buttons() & Qt::LeftButton)
    {
      int minX = Pii::min(d->mouseCurrPoint.x(), d->mousePressPoint.x());
      int minY = Pii::min(d->mouseCurrPoint.y(), d->mousePressPoint.y());
      int maxX = Pii::max(d->mouseCurrPoint.x(), d->mousePressPoint.x());
      int maxY = Pii::max(d->mouseCurrPoint.y(), d->mousePressPoint.y());

      // Start selection
      if ((d->selectionArea.isNull()) && (maxX - minX > 3 || maxY - minY > 3))
        {
          if (d->selectionMode == Area)
            d->selectionArea = QRect(minX, minY, maxX-minX, maxY-minY);
          else if (d->selectionMode == Horizontal)
            d->selectionArea = QRect(minX, -1, maxX-minX, height()+2);
          else
            d->selectionArea = QRect(-1,minY, width()+2, maxY-minY);

          update();
        }
      else if (!d->selectionArea.isNull())
        {
          if (d->selectionMode == Area)
            d->selectionArea.setCoords(minX, minY, maxX, maxY);
          else if (d->selectionMode == Horizontal)
            d->selectionArea.setCoords(minX, -1, maxX, height()+2);
          else
            d->selectionArea.setCoords(-1,minY, width()+2, maxY);

          update();
        }

      if (!d->selectionArea.isNull())
        {
          int iWidth = qRound(widthFromWidget2Image(d->selectionArea.width()));
          int iHeight = qRound(heightFromWidget2Image(d->selectionArea.height()));
          QString strToolTip;
          synchronized (d->imageLock)
            strToolTip = d->pAdapter->toolTipForSelection(QRect(imagePoint, QSize(iWidth, iHeight)));
          if (!strToolTip.isEmpty())
            QToolTip::showText(event->globalPos(), strToolTip, this);
        }
    }
  else
    {
      if (d->imageRect.contains(imagePoint))
        {
          QString strToolTip;
          synchronized (d->imageLock)
            strToolTip = d->pAdapter->toolTipForPoint(imagePoint);
          if (!strToolTip.isEmpty())
            QToolTip::showText(event->globalPos(), strToolTip, this);
        }

      QWidget::mouseMoveEvent(event);
    }
}

void PiiImageViewport::mousePressEvent(QMouseEvent* event)
{
  if (d->imageRect.isNull())
    return;

  setFocus(Qt::MouseFocusReason);

  // The mouse press point (in widget coordinates) is stored.
  if (event->button() & Qt::LeftButton)
    d->mousePressPoint = event->pos();

  if (d->selectionArea.isNull() && (event->button() & Qt::RightButton))
    {
      QMenu *pMenu = d->pAdapter->popupMenu(pointFromWidget2Image(event->pos()));
      if (pMenu)
        pMenu->exec(event->globalPos());
    }

  QWidget::mousePressEvent(event);
}

void PiiImageViewport::mouseReleaseEvent(QMouseEvent* event)
{
  if (d->imageRect.isNull())
    return;

  if (!d->selectionArea.isNull() && (event->button() & Qt::LeftButton))
    {
      // Area has been selected, if we get in this if.
      QRect rect = rectFromWidget2Image(d->selectionArea);
      int x = qMax(0,rect.x());
      int y = qMax(0,rect.y());
      int width = qMin(d->imageRect.right()+1, rect.right()) - x;
      int height = qMin(d->imageRect.bottom()+1, rect.bottom()) - y;

      if (d->mousePressPoint.x() > d->mouseCurrPoint.x())
        {
          x += width;
          width = -width;
        }
      if (d->mousePressPoint.y() > d->mouseCurrPoint.y())
        {
          y += height;
          height = -height;
        }

      emit areaSelected(QRect(x,y,width,height), event->modifiers());
      d->selectionArea = QRect();
      updateImage();
    }
  else if (event->button() & Qt::LeftButton)
    // NOTE: Not tested
    emit clicked(pointFromWidget2Image(event->pos()),
                 event->modifiers());

  QWidget::mouseReleaseEvent(event);
}

void PiiImageViewport::paintEvent(QPaintEvent* event)
{
  //QWidget::paintEvent(event);

  d->imageLock.lock();

  QPainter p(this);
  QRect paintRect = event->rect();

  // Paint with background color first. There may be transparent
  // portions in the image.
  p.fillRect(paintRect, palette().brush(backgroundRole()));

  // Draw image only if there is one
  if (!d->prescaledImage.isNull())
    {
      // If the image doesn't fill the widget, we need to draw background.
      // The updated area may only partially overlap the image.
      if (d->prescaledImage.width() < width() ||
          d->prescaledImage.height() < height())
        {
          QRect imageRect = d->prescaledImage.rect();

          // Center the image on the widget
          int leftShift = qMax((width()-imageRect.width())/2, 0),
            topShift = qMax((height()-imageRect.height())/2, 0);
          imageRect.moveLeft(leftShift);
          imageRect.moveTop(topShift);

          // First check if the paint area contains not only image data.
          if (!imageRect.contains(paintRect))
            // Fill the whole area with the background brush.
            p.fillRect(paintRect, palette().brush(backgroundRole()));

          // Does the area to be painted intersect with the image?
          QRect paintImageRect = imageRect & paintRect;
          if (paintImageRect.isValid())
            p.drawImage(paintImageRect, d->prescaledImage, paintImageRect.translated(-leftShift, -topShift));
        }
      // Otherwise just dump the data onto the screen
      else
        p.drawImage(paintRect, d->prescaledImage, paintRect);

      d->imageLock.unlock();

      QRect tempWindow = p.window();
      p.setWindow(d->visibleArea);
      // Draw the overlays.
      for (int i = 0; i < d->overlays.size(); ++i)
        // Some optimization. Draw the overlay only, if it is in the
        // visible area.
        if (d->overlays.at(i)->enabled() && d->overlays.at(i)->intersects(d->visibleArea))
          d->overlays.at(i)->paint((&p), d->bShowOverlayColoring);

      p.setWindow(tempWindow);

      // Draw the selection rectangle with dashed line and color white/black
      if (!d->selectionArea.isNull())
        {
          p.setPen(Qt::NoPen);
          p.setBrush(QColor(0,0,255,10));
          p.drawRect(d->selectionArea);

          QPen pen(QColor(0,0,0));
          p.setPen(pen);
          p.setBrush(Qt::NoBrush);
          p.drawRect(d->selectionArea);

          if (d->selectionMode == Area)
            p.drawLine(d->mousePressPoint, d->mouseCurrPoint);

          pen.setColor(QColor(255,255,255));
          pen.setStyle(Qt::DashLine);
          p.setPen(pen);
          p.drawRect(d->selectionArea);
          if (d->selectionMode == Area)
            p.drawLine(d->mousePressPoint, d->mouseCurrPoint);
        }

      // Draw grid if enabled
      if (d->bDrawGrid)
        {
          QRectF gridRect = QRectF(d->pixelSize.width() * d->visibleArea.x(),
                                   d->pixelSize.height() * d->visibleArea.y(),
                                   d->pixelSize.width() * d->visibleArea.width(),
                                   d->pixelSize.height() * d->visibleArea.height());
          double w = gridRect.width();
          double h = gridRect.height();

          // Calculate horizontal lines if necessary
          if (gridRect.top() != d->previousGridRect.top() ||
              gridRect.bottom() != d->previousGridRect.bottom())
            {
              d->dHLineStep = d->pAdapter->gridSpacing(d->dYScale/d->pixelSize.height(), Pii::Vertically);
              if (d->dHLineStep > 0)
                {
                  d->iHLineCount = h / d->dHLineStep + 2;

                  double dMod = fmod(gridRect.top(), d->dHLineStep);
                  d->dHLineStart = gridRect.top() - dMod;
                }
              else
                d->dHLineStart = d->iHLineCount = 0;
            }

          // Calculate vertical lines if necessary
          if (gridRect.left() != d->previousGridRect.left() ||
              gridRect.right() != d->previousGridRect.right())
            {
              d->dVLineStep = d->pAdapter->gridSpacing(d->dXScale/d->pixelSize.width(), Pii::Horizontally);
              if (d->dVLineStep > 0)
                {
                  d->iVLineCount = w / d->dVLineStep + 2;

                  double dMod = fmod(gridRect.left(), d->dVLineStep);
                  d->dVLineStart = gridRect.left() - dMod;
                }
              else
                d->dVLineStart = d->iVLineCount = 0;
            }
          d->previousGridRect = gridRect;

          QPen pen(Qt::DotLine);
          pen.setColor(QColor(130,130,130));
          p.setPen(pen);
          p.setBrush(Qt::NoBrush);

          // Draw horizontal grid lines
          for (int i=0; i<d->iHLineCount; i++)
            {
              double y = d->dYScale * ((d->dHLineStart + (double)i*d->dHLineStep) / d->pixelSize.height() - (double)d->visibleArea.y());
              p.drawLine(QLineF(0, y, width(), y));
            }

          // Draw vertical grid lines
          for (int i=0; i<d->iVLineCount; i++)
            {
              double x = d->dXScale * ((d->dVLineStart + (double)i*d->dVLineStep) / d->pixelSize.width() - (double)d->visibleArea.x());
              p.drawLine(QLineF(x, 0, x, height()));
            }
        }

    }
  else
    d->imageLock.unlock();

}

double PiiImageViewport::gridSpacing(double pixelsPerUnit, Pii::MatrixDirection) const
{
  // Ensure at leat 25 px on-screen spacing while accepting only powers of ten.
  // pixelsPerUnit * 10^x >= 25
  // 10^x >= 25/pixelsPerUnit
  // x >= log10(25/pixelsPerUnit)
  return pow(10.0, ceil(log10(25.0/pixelsPerUnit)));
}

void PiiImageViewport::resizeEvent(QResizeEvent* event)
{
  QWidget* pParent = parentWidget();
  setUpdatesEnabled(false);
  if (pParent)
    {
      checkFitMode();
    }
  focusImage(PiiImageViewport::FocusToWidgetTopLeft);
  setUpdatesEnabled(true);
  QWidget::resizeEvent(event);
  updateImage();
}

void PiiImageViewport::wheelEvent(QWheelEvent* event)
{
  if(Qt::ControlModifier & event->modifiers())
    {
      int delta = event->delta();
      if (delta > 0)
        zoomIn();
      else if (delta < 0)
        zoomOut();
    }
  else
    QWidget::wheelEvent(event);
}

void PiiImageViewport::showEvent(QShowEvent* event)
{
  d->pUpdater->setEnabled(true);
  QWidget::showEvent(event);
  updateImage();
}

void PiiImageViewport::hideEvent(QHideEvent* event)
{
  d->pUpdater->setEnabled(false);
  QWidget::hideEvent(event);
}

void PiiImageViewport::focusImage(PiiImageViewport::FocusPolicy focusPolicy)
{
  if (d->imageRect.isNull())
    return;

  // Calculate the width and the height of the scaled image in widget pixels.
  int iScaledImageWidth = int(double(d->imageRect.width())*d->dXScale + 0.5);
  int iScaledImageHeight = int(double(d->imageRect.height())*d->dYScale + 0.5);

  QWidget* pParent = parentWidget();
  if (pParent)
    {

      // Calculate the width and height of the current visible area in
      // d->pixmap pixels.
      int iVisibleWidth = int(double(pParent->width())/d->dXScale + 1);
      int iVisibleHeight = int(double(pParent->height())/d->dYScale + 1);
      int iVisibleLeft = 0;
      int iVisibleTop = 0;

      if (iScaledImageWidth > width())
        {
          // The image is wider than the widget
          double dXFocusFactor = 0.0;
          int xFocusPoint = 0;

          if (focusPolicy == PiiImageViewport::FocusToMouseCursor)
            {
              // First, calculate focus point x-coordinate in the
              // coordinates of d->pImage based on the current cursor position.
              dXFocusFactor = double(d->mouseCurrPoint.x())/double(pParent->width());
              int xOffset = int(dXFocusFactor * double(d->visibleArea.width()) + 0.5);
              xFocusPoint = d->visibleArea.x() + xOffset;
            }
          else if (focusPolicy == PiiImageViewport::FocusToWidgetTopLeft)
            {
              xFocusPoint = d->visibleArea.x();
            }


          // Set the new visible left coordinate so, that the
          // x-coordinate in d->pImage, where the cursor point, remain
          // the same.
          iVisibleLeft = xFocusPoint - int(dXFocusFactor*double(iVisibleWidth) + 0.5);
          // Make sure that the visible are won't go outside the image
          // area.
          if (iVisibleLeft + iVisibleWidth > d->imageRect.width())
            iVisibleLeft = d->imageRect.width()-iVisibleWidth;
          if (iVisibleLeft < 0)
            iVisibleLeft = 0;

        }
      else
        {
          // The image is narrower or has equal width compared to the
          // widget.
          iVisibleLeft = (d->imageRect.width()-iVisibleWidth)/2;
        }

      if (iScaledImageHeight > height())
        {
          // The image is taller than the parent widget
          double dYFocusFactor = 0.0;
          int yFocusPoint = 0;

          if (focusPolicy == PiiImageViewport::FocusToMouseCursor)
            {
              // First, calculate focus point y-coordinate in the
              // coordinates of d->pImage based on the current cursor position.
              dYFocusFactor = double(d->mouseCurrPoint.y())/double(pParent->height());
              int yOffset = int(dYFocusFactor*double(d->visibleArea.height()) + 0.5);
              yFocusPoint = d->visibleArea.y()+yOffset;
            }
          else if (focusPolicy == PiiImageViewport::FocusToWidgetTopLeft)
            {
              yFocusPoint = d->visibleArea.y();
            }

          // Set the new visible top coordinate so, that the
          // y-coordinate in d->pImage, where the cursor point, remain
          // the same.
          iVisibleTop = yFocusPoint-int(dYFocusFactor*double(iVisibleHeight) + 0.5);
          // Make sure that the visible area won't go outside the image
          // area.
          if (iVisibleTop + iVisibleHeight > d->imageRect.height())
            iVisibleTop = d->imageRect.height()-iVisibleHeight;
          if (iVisibleTop < 0)
            iVisibleTop = 0;
        }
      else
        {
          // The image is flatter or has equal width compared to the
          // widget.
          iVisibleTop = (d->imageRect.height()-iVisibleHeight)/2;
        }

      QRect newRect = QRect(iVisibleLeft, iVisibleTop, iVisibleWidth, iVisibleHeight);

      if (d->visibleArea != newRect)
        {
          d->visibleArea = newRect;
          // Tell for the parent, that the visible area has been
          // changed.
          emit visibleAreaChanged(d->visibleArea.x(), d->visibleArea.y(), d->visibleArea.width(), d->visibleArea.height());
        }
    }
  else
    {
      d->visibleArea = QRect(0,0,
                             int(double(d->imageRect.width())*d->dXScale + 0.5),
                             int(double(d->imageRect.height())*d->dYScale + 0.5));
    }
}

/* Sets the current x-coordinate of the top-left point of the visible area.
 */
void PiiImageViewport::setCurrX(int x)
{
  //The old width is stored temporarily, because the value of the
  //width is changed when setting the x.
  int tempWidth = d->visibleArea.width();
  d->visibleArea.setX(x);
  //The width is set back to the old value
  d->visibleArea.setWidth(tempWidth);

  emit visibleAreaChanged(d->visibleArea.x(), d->visibleArea.y(), d->visibleArea.width(), d->visibleArea.height());

  updateImage();
}

/* Sets the current y-coordinate of the top-left point of the visible area.
 */
void PiiImageViewport::setCurrY(int y)
{
  int height = d->visibleArea.height();
  d->visibleArea.setY(y);
  // The old height must be set back, because setting the y value above
  // changed the height.
  d->visibleArea.setHeight(height);

  emit visibleAreaChanged(d->visibleArea.x(), d->visibleArea.y(), d->visibleArea.width(), d->visibleArea.height());

  updateImage();
}

/* Moves the current x-coordinate by dx pixels relative to the current
 * x position. The value can be either negative or positive.
 */
void PiiImageViewport::moveCurrX(int dx)
{
  setCurrX(d->visibleArea.x()+dx);
}

/* Moves the current x-coordinate by dx pixels relative to the current
 * x position. The value can be either negative or positive.
 */
void PiiImageViewport::moveCurrY(int dy)
{
  setCurrY(d->visibleArea.y()+dy);
}

/* Transforms the widget x-coordinate the image x-coordinate*/
int PiiImageViewport::xFromWidget2Image(int x) const
{
  return d->visibleArea.x() + int(double(x)/d->dXScale);
}

/* Transforms the widget y-coordinate the image y-oordinate.*/
int PiiImageViewport::yFromWidget2Image(int y) const
{
  return d->visibleArea.y() + int(double(y)/d->dYScale);
}

/* Transforms the widget coordinates to the image coordinates */
QPoint PiiImageViewport::pointFromWidget2Image(const QPoint &pos) const
{
  return QPoint(xFromWidget2Image(pos.x()), yFromWidget2Image(pos.y()));
}

/* Transforms the width in widget coordinates to the width in image coordinates */
double PiiImageViewport::widthFromWidget2Image(int width) const
{
  return double(width)/d->dXScale;
}

/* Transforms the height in widget coordinates to the height in image coordinates */
double PiiImageViewport::heightFromWidget2Image(int length) const
{
  return double(length)/d->dYScale;
}

QString PiiImageViewport::formatToolTipText(const QString& text) const
{
  QStringList lstRows = text.split('\n');
  QString strResult;
  for (int i=0; i<lstRows.size(); ++i)
    {
      int iTabIndex = lstRows[i].indexOf('\t');
      if (iTabIndex == -1)
        strResult += "<tr><td colspan=\"2\">" + lstRows[i] + "</td></tr>";
      else
        strResult += "<tr><td>" + lstRows[i].replace(iTabIndex, 1, "</td><td>") + "</td></tr>";
    }

  return strResult;
}

QString PiiImageViewport::toolTipForPoint(const QPoint& imagePoint) const
{
  QString message("<table>" + formatToolTipText(tr("Location:\t(%1,%2)")).arg(imagePoint.x()).arg(imagePoint.y()));

  if (!d->lstLayers[0]->pImage->isNull())
    {
      QRgb clr = d->lstLayers[0]->pImage->pixel(imagePoint.x(), imagePoint.y());
      if (qRed(clr) == qGreen(clr) && qGreen(clr) == qBlue(clr))
        message += formatToolTipText(tr("Gray level:\t%1")).arg(qRed(clr));
      else
        message += formatToolTipText(tr("Color:\t(%1,%2,%3)")).arg(qRed(clr)).arg(qGreen(clr)).arg(qBlue(clr));
    }

  for (int i=0; i<d->overlays.size(); i++)
    if (d->overlays[i]->contains(imagePoint))
      message += formatToolTipText(d->overlays[i]->toolTipText());

  message += "</table>";
  return message;
}

QString PiiImageViewport::toolTipForSelection(const QRect& area) const
{
  double dAngle = Pii::atan2(area.height(), area.width())*180/M_PI;
  if (d->mousePressPoint.y() < d->mouseCurrPoint.y())
    dAngle = -dAngle;
  return "<table>" +
    formatToolTipText(tr("Location:\t(%1,%2)\n"
                         "Selection:\t(%3 x %4)\n"
                         "Diagonal:\t%5\n"
                         "Angle:\t%6"))
    .arg(area.x()).arg(area.y())
    .arg(area.width()).arg(area.height())
    .arg(Pii::round<int>(Pii::hypotenuse(double(area.width()), double(area.height()))))
    .arg(dAngle, 0, 'f', 1) +
    "</table>";
}

/* Transforms the rectangle in the widget coordinates to the rectangle
   in the image coordinates. This is done by doing the translation and
   scaling. */
QRect PiiImageViewport::rectFromWidget2Image(const QRect &rect) const
{
  return QRect(pointFromWidget2Image(rect.topLeft()), pointFromWidget2Image(QPoint(rect.x()+rect.width(), rect.y()+rect.height())));
}

/* Transforms the image x-coordinate the widget x-coordinate*/
int PiiImageViewport::xFromImage2Widget(int x) const
{
  return int(d->dXScale*(x -d->visibleArea.x()));
}

/* Transforms the image y-coordinate the widget y-oordinate.*/
int PiiImageViewport::yFromImage2Widget(int y) const
{
  return int(d->dYScale*(y -d->visibleArea.y()));
}

/* Transforms the image coordinates to the widget coordinates */
QPoint PiiImageViewport::pointFromImage2Widget(const QPoint &pos) const
{
  return QPoint(xFromWidget2Image(pos.x()), yFromWidget2Image(pos.y()));
}

/* Transforms the rectangle in the image coordinates to the rectangle
   in the widget coordinates. This is done by doing the translation and
   scaling. */
QRect PiiImageViewport::rectFromImage2Widget(const QRect &rect) const
{
  return QRect(pointFromImage2Widget(rect.topLeft()), pointFromImage2Widget(QPoint(rect.x()+rect.width(), rect.y()+rect.height())));
}

QRect PiiImageViewport::startRendering()
{
  d->imageLock.lock();
  return d->visibleArea;
}

void PiiImageViewport::endRendering(QRect visibleArea)
{
  d->prescaledArea = visibleArea;
  d->imageLock.unlock();
}

QImage* PiiImageViewport::image(int layer) const
{
  if (layer >= 0 && layer < d->lstLayers.size())
    return d->lstLayers[0]->pImage;
  return 0;
}

double PiiImageViewport::zoom() const { return d->dZoomFactor; }
double PiiImageViewport::xScale() const { return d->dXScale; }
double PiiImageViewport::yScale() const { return d->dYScale; }
QSizeF PiiImageViewport::pixelSize() const {  return d->pixelSize; }
void PiiImageViewport::updateImage() { d->pUpdater->refresh(); }

/************************* PiiImageViewportUpdater **************************/
PiiImageViewportUpdater::PiiImageViewportUpdater(PiiImageViewport* parent) :
  _pParent(parent),
  _bRunning(false),
  _bEnabled(false)
{
}

void PiiImageViewportUpdater::refresh()
{
  _updateCondition.wakeOne();
}

void PiiImageViewportUpdater::startThread()
{
  _bRunning = true;
  QThread::start();
}

void PiiImageViewportUpdater::stop()
{
  _bRunning = false;
  _updateCondition.wakeOne();
}

void PiiImageViewportUpdater::run()
{
  while (_bRunning)
    {
      if (_bEnabled)
        updateImage();
      _updateCondition.wait();
    }
}

void PiiImageViewportUpdater::updateImage()
{
  PiiImageViewport::Data* d = _pParent->d;
  QRect visibleArea = _pParent->startRendering();
  QImage* image = d->lstLayers[0]->pImage;
  if (image->isNull())
    {
      d->prescaledImage = QImage();
      _pParent->endRendering(visibleArea);
      return;
    }

  // The portion of the image that is actually visible
  QRect visibleImageArea = visibleArea & image->rect();

  // The size of the visible image portion in screen coordinates
  QRect drawingArea(0, 0,
                    int(d->dXScale * visibleImageArea.width()),
                    int(d->dYScale * visibleImageArea.height()));

  // Center the drawing area if necessary
  if (drawingArea.width() > _pParent->width())
    drawingArea.moveLeft(-int((double)(drawingArea.width() - _pParent->width()) / 2.0 + 0.5));
  if (drawingArea.height() > _pParent->height())
    drawingArea.moveTop(-int((double)(drawingArea.height() - _pParent->height()) / 2.0 + 0.5));

  // Resize the buffered pixmap if necessary
  if (drawingArea.width() != d->prescaledImage.width() ||
      drawingArea.height() != d->prescaledImage.height() ||
      (d->prescaledImage.format() != QImage::Format_ARGB32 && d->lstLayers.size() > 1))
    d->prescaledImage = QImage(drawingArea.width(), drawingArea.height(),
                               d->lstLayers.size() > 1 ? QImage::Format_ARGB32 : QImage::Format_RGB32);

  // Scale and draw the visible portion of the image into a pixmap
  QPainter p(&d->prescaledImage);
  if (!d->lstLayers[0]->bVisible || d->lstLayers[0]->dOpacity != 1.0)
    p.fillRect(drawingArea, _pParent->palette().brush(_pParent->backgroundRole()));
  Qt::ImageConversionFlags conversionFlags = Qt::AutoColor;
  for (int i=0; i<d->lstLayers.size(); ++i)
    {
      if (!d->lstLayers[i]->pImage->isNull() && d->lstLayers[i]->bVisible)
        {
          if (!d->lstLayers[0]->dOpacity == 1.0)
            p.fillRect(drawingArea, _pParent->palette().brush(_pParent->backgroundRole()));
          p.setOpacity(d->lstLayers[i]->dOpacity);
          p.drawImage(drawingArea, *d->lstLayers[i]->pImage, visibleImageArea, conversionFlags);
        }
      //conversionFlags = Qt::AutoColor | Qt::NoOpaqueDetection | Qt::OrderedAlphaDither;
    }
  p.end();

  // Store the current visible area so that paintEvent() knows where
  // to place overlays and stuff.
  _pParent->endRendering(visibleArea);

  emit imageReady();
}

void PiiImageViewport::setAdapter(PiiImageViewportAdapter* adapter)
{
  d->pAdapter = adapter ? adapter : this;
}
PiiImageViewportAdapter* PiiImageViewport::adapter() const { return d->pAdapter; }
