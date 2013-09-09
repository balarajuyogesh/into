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

#include "PiiImageDisplay.h"
#include <QPainter>
#include <QDialog>
#include <QPushButton>

#include "ui_imagedisplay.h"
#include <PiiYdinTypes.h>

class PiiImageDisplay::Data
{
public:
  Ui_ImageDisplay ui;
};

PiiImageDisplay::PiiImageDisplay(QWidget *parent) :
  QFrame(parent),
  d(new Data)
{
  init();
}

PiiImageDisplay::~PiiImageDisplay()
{
  delete d;
}

void PiiImageDisplay::init()
{
  Ui_ImageDisplay* ui = &d->ui;
  ui->setupUi(this);

  connect(ui->_pImageScrollArea, SIGNAL(areaSelected(QRect,int)), this, SIGNAL(areaSelected(QRect,int)));
  connect(ui->_pImageScrollArea, SIGNAL(clicked(QPoint,int)), this, SIGNAL(clicked(QPoint,int)));
  connect(ui->_pImageScrollArea->imageViewport(), SIGNAL(scaleChanged(double, double)),
          this, SLOT(viewportScaleChanged(double, double)));
  connect(ui->_pImageScrollArea->imageViewport(), SIGNAL(fitModeChanged(int)),
          SLOT(enableScrollBars(int)));
  
  initMeasureBars();
}

void PiiImageDisplay::initMeasureBars()
{
  Ui_ImageDisplay* ui = &d->ui;
  ui->_pHorizontalMeasureBar->setOrientation(Qt::Horizontal);
  ui->_pHorizontalMeasureBar->setLineWidth(1);
  ui->_pHorizontalMeasureBar->setFrameShape(QFrame::NoFrame);
  ui->_pHorizontalMeasureBar->setContentsMargins(0,0,0,0);
  ui->_pHorizontalMeasureBar->setTickPosition(PiiMeasureBar::TicksBottomOrLeft);
  ui->_pHorizontalMeasureBar->setScale(ui->_pImageScrollArea->imageViewport()->xScale());
  ui->_pHorizontalMeasureBar->setStartPos(0);

  connect(ui->_pImageScrollArea, SIGNAL(horizontalStartPosChanged(double)),
          ui->_pHorizontalMeasureBar, SLOT(setStartPos(double)));
  connect(ui->_pImageScrollArea->imageViewport(), SIGNAL(mouseLocationChanged(QPoint)),
          ui->_pHorizontalMeasureBar, SLOT(setMouseLocation(QPoint)));
  connect(ui->_pImageScrollArea, SIGNAL(mouseOnView(bool)),
          ui->_pHorizontalMeasureBar, SLOT(setDrawMouseLocation(bool)));
  

  ui->_pVerticalMeasureBar->setOrientation(Qt::Vertical);
  ui->_pVerticalMeasureBar->setLineWidth(1);
  ui->_pVerticalMeasureBar->setFrameShape(QFrame::NoFrame);
  ui->_pVerticalMeasureBar->setContentsMargins(0,0,0,0);
  ui->_pVerticalMeasureBar->setTickPosition(PiiMeasureBar::TicksTopOrRight);
  ui->_pVerticalMeasureBar->setScale(ui->_pImageScrollArea->imageViewport()->yScale());
  ui->_pVerticalMeasureBar->setStartPos(0);
  
  connect(ui->_pImageScrollArea, SIGNAL(verticalStartPosChanged(double)),
          ui->_pVerticalMeasureBar, SLOT(setStartPos(double)));
  connect(ui->_pImageScrollArea->imageViewport(), SIGNAL(mouseLocationChanged(QPoint)),
          ui->_pVerticalMeasureBar, SLOT(setMouseLocation(QPoint)));
  connect(ui->_pImageScrollArea, SIGNAL(mouseOnView(bool)),
          ui->_pVerticalMeasureBar, SLOT(setDrawMouseLocation(bool)));
  
  QVariantList minDistances, tickScales;
  QStringList tickNames;
  
  minDistances << 10 << 25 << 50;
  tickScales << 1 << 5 << 10 << 50 << 100 << 500 << 1000 << 5000 << 10000 << 50000 << 100000;
  tickNames  << "px" << "" << "-" << "" << "-" << "" << "-" << "" << "-" << "" << "-";
  
  ui->_pHorizontalMeasureBar->setMinTickDistances(minDistances);
  ui->_pHorizontalMeasureBar->setTickScales(tickScales);
  ui->_pHorizontalMeasureBar->setTickNames(tickNames);
  ui->_pHorizontalMeasureBar->setDrawUnitNames(false);
  
  ui->_pVerticalMeasureBar->setMinTickDistances(minDistances);
  ui->_pVerticalMeasureBar->setTickScales(tickScales);
  ui->_pVerticalMeasureBar->setTickNames(tickNames);
  ui->_pVerticalMeasureBar->setDrawUnitNames(false);
}

void PiiImageDisplay::viewportScaleChanged(double x, double y)
{
  Ui_ImageDisplay* ui = &d->ui;
  ui->_pHorizontalMeasureBar->setScale(x);
  ui->_pVerticalMeasureBar->setScale(y);
}

void PiiImageDisplay::setDisplayType(DisplayType type)
{
  d->ui._pImageScrollArea->setDisplayType(type);
}

PiiImageDisplay::DisplayType PiiImageDisplay::displayType() const
{
  return (DisplayType)d->ui._pImageScrollArea->displayType();
}

void PiiImageDisplay::setShowHorizontalMeasureBar(bool show)
{
  Ui_ImageDisplay* ui = &d->ui;
  ui->_pHorizontalMeasureBar->setVisible(show);
  ui->_pCornerFrame->setVisible(showHorizontalMeasureBar() && showVerticalMeasureBar());
}

void PiiImageDisplay::setShowVerticalMeasureBar(bool show)
{
  Ui_ImageDisplay* ui = &d->ui;
  ui->_pVerticalMeasureBar->setVisible(show);
  ui->_pCornerFrame->setVisible(showHorizontalMeasureBar() && showVerticalMeasureBar());
}

bool PiiImageDisplay::showVerticalMeasureBar() const
{
  return d->ui._pVerticalMeasureBar->isVisible();
}

bool PiiImageDisplay::showHorizontalMeasureBar() const
{
  return d->ui._pHorizontalMeasureBar->isVisible();
}

void PiiImageDisplay::setImage(const PiiVariant& image, int layer)
{
  d->ui._pImageScrollArea->setImage(image,layer);
}

PiiMeasureBar* PiiImageDisplay::horizontalMeasureBar() const
{
  return d->ui._pHorizontalMeasureBar;
}

PiiMeasureBar* PiiImageDisplay::verticalMeasureBar() const
{
  return d->ui._pVerticalMeasureBar;
}

PiiImageViewport* PiiImageDisplay::imageViewport() const
{
  return d->ui._pImageScrollArea->imageViewport();
}

PiiImageScrollArea* PiiImageDisplay::imageScrollArea() const
{
  return d->ui._pImageScrollArea;
}

void PiiImageDisplay::enableScrollBars(int mode)
{
  Ui_ImageDisplay* ui = &d->ui;
  Qt::ScrollBarPolicy policy = mode == PiiImageViewport::FitToView ? Qt::ScrollBarAlwaysOff : Qt::ScrollBarAsNeeded;
  ui->_pImageScrollArea->setVerticalScrollBarPolicy(policy);
  ui->_pImageScrollArea->setHorizontalScrollBarPolicy(policy);
}

QDialog* PiiImageDisplay::dialog(const PiiVariant& image)
{
  QDialog* pDialog = new QDialog;
  QVBoxLayout* pVBox = new QVBoxLayout(pDialog);
  PiiImageDisplay* pDisplay = new PiiImageDisplay;
  pVBox->addWidget(pDisplay);
  QHBoxLayout* pHBox = new QHBoxLayout;
  pHBox->addStretch(1);
  QPushButton* pCloseButton = new QPushButton(tr("Close"));
  pHBox->addWidget(pCloseButton);
  pVBox->addLayout(pHBox);
  connect(pCloseButton, SIGNAL(clicked()), pDialog, SLOT(accept()));
  pDisplay->setImage(image);
  return pDialog;
}

QDialog* PiiImageDisplay::dialog(const PiiMatrix<uchar>& image)
{
  return dialog(PiiVariant(image));
}

QDialog* PiiImageDisplay::dialog(const PiiMatrix<PiiColor4<uchar> >& image)
{
  return dialog(PiiVariant(image));
}
