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

#include "PiiMeasureBar.h"

#include <PiiMath.h>
#include <PiiUtil.h>

#include <math.h>

#include <QPainter>
#include <QPen>
#include <QRect>
#include <QRegExp>
#include <QStyleOptionFrame>
#include <QMouseEvent>


PiiMeasureBar::Data::Data(Qt::Orientation o) :
  orientation(o),
  tickPosition(PiiMeasureBar::TicksTopOrRight | PiiMeasureBar::TicksBottomOrLeft),
  dPhysicalPixelSize(1.0),
  dPixelSize(1.0),
  dScale(1.0),
  dStartPos(0),
  iMouseLocation(0),
  bDrawMouseLocation(false),
  bDrawMouseText(false),
  bDrawUnitNames(true),
  bShowAbsoluteValues(false),
  iCurrBigScaleIndex(-1),
  iCurrUnitIndex(-1),
  pFormatter(defaultFormatter())
{
}


PiiMeasureBar::PiiMeasureBar(QWidget* parent) :
  QFrame(parent),
  d(new Data(Qt::Vertical))
{
  init();
}

PiiMeasureBar::PiiMeasureBar(Qt::Orientation orientation, QWidget* parent) :
  QFrame(parent),
  d(new Data(orientation))
{
  init();
}

PiiMeasureBar::~PiiMeasureBar()
{
  delete d;
}

void PiiMeasureBar::init()
{
  setMinimumSize(24,24);
  setAutoFillBackground(true);
  setMouseTracking(true);
}

QVariantList PiiMeasureBar::minTickDistances() const
{
  return Pii::vectorToVariants<int>(d->lstMinTickDistances);
}

void PiiMeasureBar::setMinTickDistances(const QVariantList& minDistances)
{
  d->lstMinTickDistances = Pii::variantsToVector<int>(minDistances);
  updateLineLengths();
}

QVariantList PiiMeasureBar::tickScales() const
{
  return Pii::vectorToVariants<double>(d->lstTickScales);
}

void PiiMeasureBar::setTickScales(const QVariantList& tickScales)
{
  d->lstTickScales = Pii::variantsToVector<double>(tickScales);
  updateStepList();
}

void PiiMeasureBar::setTickNames(const QStringList& tickNames)
{
  d->lstTickNames = tickNames;
  updateStepList();
}

void PiiMeasureBar::setDrawUnitNames(bool status) { d->bDrawUnitNames = status; }

void PiiMeasureBar::setTickPosition(TickPosition tickPosition)
{
  d->tickPosition = tickPosition;
}

void PiiMeasureBar::updatePixelSize()
{
  d->dPixelSize = d->dPhysicalPixelSize/d->dScale;
  updateStepList();
  update();
}

void PiiMeasureBar::setPhysicalPixelSize(double size)
{
  if (size != d->dPhysicalPixelSize)
    {
      d->dPhysicalPixelSize = size;
      updatePixelSize();
    }
}

void PiiMeasureBar::setScale(double scale)
{
  if (scale != d->dScale)
    {
      d->dScale = scale;
      updatePixelSize();
    }
}

void PiiMeasureBar::setStartPos(double length)
{
  d->dStartPos = length;
  update();
}

void PiiMeasureBar::setMouseLocation(const QPoint& location)
{
  setMouseLocation(d->orientation == Qt::Vertical ? location.y() : location.x());
}

void PiiMeasureBar::setMouseLocation(int location)
{
  d->iMouseLocation = location;
  update();
}

/* This function updates the member variable d->lstLineLengths based on
   the content of the member variable d->lstMinTickDistances.
   d->lstLineLengths is filled with the numbers starting from 2 upwards
   with the interval of 2. The amount of numbers in the list will be
   equal to the amount of numbers in d->lstMinTickDistances. */
void PiiMeasureBar::updateLineLengths()
{
  d->lstLineLengths.clear();
  for (int i = 0; i<d->lstMinTickDistances.size(); i++)
    d->lstLineLengths << i*2+2;
}

/* This function updates the member variable d->lstSteps. */
void PiiMeasureBar::updateStepList()
{
  d->lstSteps.clear();

  // Protect from stupid users.
  if (d->lstTickScales.isEmpty() || d->lstTickNames.size() != d->lstTickScales.size())
    return;

  // Initialize the big scale index to -1 meaning that it doesn't
  // contain any real value. If the new big scale that meet
  // the requirements, is not found (e.g. d->lstTickScales is empty) -1
  // is the final value.
  int tempBigScaleIndex = -1;

  // Find out the greatest scale
  for (int j = 0; j<d->lstTickScales.size(); ++j)
    {
      if (d->lstTickScales[j] / d->dPixelSize > d->lstMinTickDistances.last() &&
          (d->lstTickNames.isEmpty() || !d->lstTickNames[j].isEmpty()))
        {
          d->lstSteps << d->lstTickScales[j];
          tempBigScaleIndex = j;
          break;
        }
    }

  // If no scale fulfilled the requirement for the greatest scale,
  // take just the last one from the list.
  if (d->lstSteps.isEmpty())
    {
      d->lstSteps << d->lstTickScales.last();
      tempBigScaleIndex = d->lstTickScales.size()-1;
    }

  // Find out the smaller ticks.
  for (int i=d->lstMinTickDistances.size()-2; i>=0; i--)
    {
      for (int j=0; j<d->lstTickScales.size(); j++)
        {
          //d->lstTickScales[j] = d->dPixelSize*d->lstSmallestStep[i]
          // Check if the tick scale fullfills the minimum pixel distance requirement.
          if (d->lstTickScales[j] / d->dPixelSize > d->lstMinTickDistances[i])
            {
              // Check, if all the bigger ticks scales are divisible with
              // this tick, and it is also smaller than all other tick
              // scales.
              bool bIsSmaller = true; //
                               //tells, if this tick is smaller than
                               //all previous ticks
              bool bIsDivisible = true; //
                                        // tell, if this tick is
                                        // divisible with all stored steps.
              for (int k=0; k<d->lstSteps.size(); k++)
                {
                  if (d->lstTickScales[j] >= d->lstSteps[k])
                    {
                      // greater than equal than other tick, not valid.
                      bIsSmaller = false;
                      break;
                    }
                  double stepRatio = d->lstSteps[k]/d->lstTickScales[j];
                  if (false == Pii::almostEqualRel(floor(stepRatio)-stepRatio,0.0))
                    {
                      // step not divisible by greater step, not valid
                      bIsDivisible = false;
                      break;
                    }
                }
              // Because the ticks are ordered in d->lstTickScales, we
              // can stop searching for this round of j-loop
              if (bIsSmaller == false)
                break;
              if (bIsDivisible)
                {
                  d->lstSteps.insert(0, d->lstTickScales[j]);
                  break;
                }
              // If the tick wasn't divisible, we can still continue
              // the j-loop, until there is a match or the tick is too
              // big.
            }
        }
    }

  d->iCurrBigScaleIndex = tempBigScaleIndex;

  updateUnitIndex();
}

/* This function updates the member variable d->iUnitIndex. In the
   function it is assumed that the member variable d->iCurrBigScaleIndex
   has been correctly set.*/
void PiiMeasureBar::updateUnitIndex()
{
  // Negative value as a unit index means that
  // no unit will be shown.
  d->iCurrUnitIndex = -1;

  // Make sure that the big scale index is valid and initialized (not -1).
  if (d->iCurrBigScaleIndex<0 || d->iCurrBigScaleIndex >= d->lstTickNames.size())
    return;

  int tempUnitIndex = d->iCurrBigScaleIndex;

  // Find out the unit index.
  if (!d->lstTickNames[tempUnitIndex].isEmpty())
    {
      if (d->lstTickNames[tempUnitIndex] == "-")
        {
          for (tempUnitIndex--; tempUnitIndex>=0; --tempUnitIndex)
            {
              if (!d->lstTickNames[tempUnitIndex].isEmpty() &&
                  d->lstTickNames[tempUnitIndex] != "-" &&
                  d->lstTickNames[tempUnitIndex] != "+")
                {
                  d->iCurrUnitIndex = tempUnitIndex;
                  break;
                }
            }
        }
      else if (d->lstTickNames[tempUnitIndex] == "+")
        {
          for (tempUnitIndex++; tempUnitIndex<d->lstTickNames.size(); ++tempUnitIndex)
            {
              if (!d->lstTickNames[tempUnitIndex].isEmpty() &&
                  d->lstTickNames[tempUnitIndex] != "-" &&
                  d->lstTickNames[tempUnitIndex] != "+")
                {
                  d->iCurrUnitIndex = tempUnitIndex;
                  break;
                }
            }
        }
      else
        {
          // Here it is assumed that the unit name corresponding to
          // tempUnitIndex is not empty.
          d->iCurrUnitIndex = tempUnitIndex;
        }
    }
}

void PiiMeasureBar::enterEvent(QEvent *e)
{
  QFrame::enterEvent(e);
  emit mouseOnMeasureBar(true);
}

void PiiMeasureBar::leaveEvent(QEvent *e)
{
  QFrame::leaveEvent(e);
  emit mouseOnMeasureBar(false);
}

void PiiMeasureBar::mouseMoveEvent(QMouseEvent *e)
{
  QFrame::mouseMoveEvent(e);
  emit mouseLocationChanged(e->pos());
}

void PiiMeasureBar::mousePressEvent(QMouseEvent *e)
{
  QFrame::mousePressEvent(e);
  emit mouseClicked(e->pos());
}

void PiiMeasureBar::paintEvent(QPaintEvent* event)
{
  QFrame::paintEvent(event);

  QRect cRect = contentsRect();

  QPainter p(this);

  QStyleOptionFrame option;
  option.initFrom(this);

  QColor foregroundColor(option.palette.windowText().color());
  QPen pen(foregroundColor);
  pen.setWidth(1);
  p.setPen(pen);

  int iBarSize;
  int iBarLength;
  int iLineWidth = lineWidth();

  if (d->orientation == Qt::Horizontal)
    {
      iBarSize = cRect.height();
      iBarLength = width();
    }
  else // vertical
    {
      iBarSize = cRect.width();
      iBarLength = height();
      p.translate(QPoint(width(),0));
      p.rotate(90.0);
    }

  int iFirstTextMargin = iLineWidth;
  int iSecondTextMargin = iLineWidth;
  if (d->tickPosition & TicksTopOrRight)
    iFirstTextMargin += d->lstLineLengths.last();
  if (d->tickPosition & TicksBottomOrLeft)
    iSecondTextMargin += d->lstLineLengths.last();
  int iTextHeight = iBarSize - iFirstTextMargin - iSecondTextMargin;

  double dStartPos = d->dStartPos * d->dPhysicalPixelSize;

  // Draw all steps
  for (int s=0; s<d->lstSteps.size(); ++s)
    {
      double dStep = d->lstSteps[s];
      int iTickCount = int((double)iBarLength * d->dPixelSize / dStep + 0.5);
      double dMod = fmod(dStartPos, dStep);
      double dFirstTick = dStartPos + (Pii::almostEqualRel(dMod,0.0) ? 0 : -dMod);
      bool bDrawText = s == d->lstSteps.size()-1;

      // Draw all ticks
      for (int t=0; t<=iTickCount; ++t)
        {
          // Calculate location in real life and in pixels
          double dRealLocation = t*dStep + dFirstTick;
          double x = (dRealLocation - dStartPos) / d->dPixelSize;

          // Draw ticks
          if (d->tickPosition & TicksTopOrRight)
            p.drawLine(x, iLineWidth, x, iLineWidth+d->lstLineLengths[s]);
          if (d->tickPosition & TicksBottomOrLeft)
            p.drawLine(x, iBarSize-d->lstLineLengths[s]-iLineWidth, x, iBarSize-iLineWidth);

          // Draw text if necessary
          if (bDrawText && d->iCurrUnitIndex >= 0)
            {
              double dTextValue = dRealLocation/d->lstTickScales[d->iCurrUnitIndex];
              if (d->bShowAbsoluteValues)
                dTextValue = qAbs(dTextValue);

              QString strUnit;
              if (d->bDrawUnitNames && d->lstTickNames[d->iCurrUnitIndex] != " ")
                strUnit = d->lstTickNames[d->iCurrUnitIndex];
              QString text = d->pFormatter->formatNumber(dTextValue,
                                                         d->lstTickScales[d->iCurrUnitIndex],
                                                         strUnit);

              int iTextWidth = fontMetrics().width(text);
              QRect textRect(x-iTextWidth/2, iFirstTextMargin, iTextWidth, iTextHeight);

              if (textRect.left() >= 0 && textRect.right() < iBarLength)
                style()->drawItemText(&p, textRect, Qt::AlignCenter,
                                      option.palette, true, text, QPalette::WindowText);
            }
        }
    }

  if (d->bDrawMouseLocation)
    {
      p.setBrush(foregroundColor);

      int x = d->orientation == Qt::Vertical ? d->iMouseLocation + cRect.y() : d->iMouseLocation + cRect.x();
      int h = d->tickPosition == TicksTopOrRight ? 2 : iBarSize - 2;
      int step = iBarSize / 5;
      int iTextY = d->orientation == Qt::Vertical ? (step+2) : cRect.y();
      int iTextHeight = h - step;
      if (d->tickPosition == TicksTopOrRight)
        step = -step;

      QPolygon pol;
      pol << QPoint(x-step, h-step) << QPoint(x+step, h-step) << QPoint(x, h);
      p.drawPolygon(pol);

      if (d->bDrawMouseText)
        {
          double dPos = dStartPos + d->dPixelSize * x;
          QString strSuffix;
          int iDecimals;
          if (dPos < 1000.0)
            {
              iDecimals = 1;
              strSuffix = tr("mm");
            }
          else if (dPos < 1000000.0)
            {
              iDecimals = 2;
              dPos = dPos/1000.0;
              strSuffix = tr("m");
            }
          else
            {
              iDecimals = 3;
              dPos = dPos/1000000.0;
              strSuffix = tr("km");
            }

          if (d->bShowAbsoluteValues)
            dPos = qAbs(dPos);

          QString strPosText = tr("%1").arg(QString::number(dPos, 'f', iDecimals));

          if (d->bDrawUnitNames)
            strPosText.append(tr(" %1").arg(strSuffix));

          QRectF textRect(x-30, iTextY, 60, iTextHeight);
          p.setBrush(QBrush(QColor(20,20,20,200)));
          p.drawRect(textRect);
          p.drawText(textRect, Qt::AlignCenter, strPosText);
        }
    }
}


void PiiMeasureBar::setOrientation(const Qt::Orientation& orientation) { d->orientation = orientation; }
Qt::Orientation PiiMeasureBar::orientation() const { return d->orientation; }
QStringList PiiMeasureBar::tickNames() const { return d->lstTickNames; }
bool PiiMeasureBar::drawUnitNames() const { return d->bDrawUnitNames;}

bool PiiMeasureBar::showAbsoluteValues() const { return d->bShowAbsoluteValues; }
void PiiMeasureBar::setShowAbsoluteValues(bool showAbsoluteValues) { d->bShowAbsoluteValues = showAbsoluteValues; }

PiiMeasureBar::TickPosition PiiMeasureBar::tickPosition() const { return d->tickPosition; }
double PiiMeasureBar::physicalPixelSize() const { return d->dPhysicalPixelSize; }
double PiiMeasureBar::scale() const { return d->dScale; }
void PiiMeasureBar::setDrawMouseLocation(bool drawMouseLocation) { d->bDrawMouseLocation = drawMouseLocation; update(); }
void PiiMeasureBar::setDrawMouseText(bool drawMouseText) { d->bDrawMouseText = drawMouseText; update(); }
bool PiiMeasureBar::drawMouseLocation() const { return d->bDrawMouseLocation; }
bool PiiMeasureBar::drawMouseText() const { return d->bDrawMouseText; }
double PiiMeasureBar::startPos() const { return d->dStartPos; }
void PiiMeasureBar::setNumberFormatter(NumberFormatter* formatter)
{
    d->pFormatter = formatter != 0 ? formatter : defaultFormatter();
}
PiiMeasureBar::NumberFormatter* PiiMeasureBar::numberFormatter() const { return d->pFormatter; }


PiiMeasureBar::NumberFormatter::~NumberFormatter() {}

class DefaultNumberFormatter : public PiiMeasureBar::NumberFormatter
{
public:
  ~DefaultNumberFormatter() {}

  QString formatNumber(double value, double /*scale*/, const QString& unit) const
  {
    QString strResult = QString::number(value);
    if (!unit.isEmpty())
      strResult.append(QString(" %1").arg(unit));
    return strResult;
  }
};

PiiMeasureBar::NumberFormatter* PiiMeasureBar::defaultFormatter()
{
  static DefaultNumberFormatter formatter;
  return &formatter;
}

