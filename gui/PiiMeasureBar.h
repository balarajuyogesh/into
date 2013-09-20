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

#ifndef _PIIMEASUREBAR_H
#define _PIIMEASUREBAR_H

#include <QFrame>
#include <QVector>
#include <QStringList>
#include <QVariant>

#include "PiiGui.h"

class QRect;

/**
 * PiiMeasureBar is a measuring tape that can be attached to the side
 * of another widget. It maps pixels on the screen to real pixels (in
 * case of scaling) or to physical units such as meters.
 *
 * PiiMeasureBar supports an unlimited number of tick levels (major,
 * semi-major, minor, ...). It automatically selects the most suitable
 * set of ticks shown based on [minTickDistances], [tickScales], and
 * [tickNames]. A tick can be used as a major tick only if it has a
 * name (or there are no names whatsoever), and if the distance
 * between ticks on the display exceeds the limit imposed by
 * [minTickDistances]. Semi-major, minor, sub-minor and other possible
 * ticks will be drawn between major ticks if they wouldn't be placed
 * too close to each other.
 *
 */
class PII_GUI_EXPORT PiiMeasureBar : public QFrame
{
  Q_OBJECT

  /**
   * This property holds the orientation of the measure widget.
   *
   * The orientation must be Qt::Vertical (the default) or Qt::Horizontal
   */
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation);

  /**
   * Pixel size in logical units. This property is used as a scale
   * factor from pixel size on the display to logical units shown on
   * the measure bar. For example, if the size of the measure bar is
   * 100 px and it spans a physical width of 3.14159 meters, 
   * `pixelSize` should be set to 0.0314159.
   */
  //Q_PROPERTY(double pixelSize READ pixelSize WRITE setPixelSize);

  Q_PROPERTY(double physicalPixelSize READ physicalPixelSize WRITE setPhysicalPixelSize);
  Q_PROPERTY(double scale READ scale WRITE setScale);
  
  /**
   * The value of the topmost/leftmost pixel in the measure bar in
   * logical units.
   */
  Q_PROPERTY(double startPos READ startPos WRITE setStartPos);

  /**
   * A list of integers that specifies the minimum distances (in
   * pixels) between ticks at different levels. The length of this
   * list determines the number of tick levels.
   *
   * ~~~(c++)
   * // Minor ticks must be at least 5 pixels apart.
   * // Semi-major ticks must be at least 20 pixels apart.
   * // Major ticks must be at least 100 pixels apart.
   * pBar->setMinTickDistances(QVariantList() << 5 << 20 << 100);
   * ~~~
   */
  Q_PROPERTY(QVariantList minTickDistances READ minTickDistances WRITE setMinTickDistances);

  /**
   * Scales of possible ticks in logical units. The ticks that will
   * actually be shown will be selected based on current [pixelSize].
   *
   * ~~~(c++)
   * // Assume our logical units are meters.
   * // Ticks can be placed on millimeters, centimeters, decimeters, meters
   * // decameters, hectometers, and kilometers.
   * pBar->setTickScales(QVariantList() << 1e-3 << 0.01 << 0.1 << 1 << 10 << 100 << 1e3);
   * ~~~
   */
  Q_PROPERTY(QVariantList tickScales READ tickScales WRITE setTickScales);

  /**
   * Names for ticks. If this property is not set, any tick can be
   * used as a major tick. If tick names are set, the number of names
   * must match the number of scales. Otherwise, the names will be
   * cleared.
   *
   * ~~~(c++)
   * // Only mm, cm, m, and km can be used as major ticks.
   * pBar->setTickNames(QStringList() << "mm" << "cm" << "" << "m" << "" << "" << "km");
   * ~~~
   *
   * If the unit name is either - or +, the tick can work as major
   * tick, but the name and scale of the preceding (-) or following
   * (+) named unit will be used.
   *
   * ~~~(c++)
   * // Only mm, m, and km can be used as major tick units.
   * // Decameters and hectometers can be major ticks, but will be shown in meters.
   * // Centimeters can be major ticks, but will be shown in millimeters.
   * // Decimeters can be major ticks, but will be shown in meters.
   * pBar->setTickNames(QStringList() << "mm" << "-" << "+" << "m" << "-" << "-" << "km");
   * ~~~
   *
   * If the name of a tick is a single space (" "), the tick can work
   * as a major tick, but it will be shown without a unit name.
   */
  Q_PROPERTY(QStringList tickNames READ tickNames WRITE setTickNames);

  /**
   * If this property is `true` (the default), unit names will be
   * shown with major ticks. Otherwise, only numbers will be shown.
   */
  Q_PROPERTY(bool drawUnitNames READ drawUnitNames WRITE setDrawUnitNames);

  /**
   * If this property is `true`, we will show absolute values.
   * Default value of this property is false.
   */
  Q_PROPERTY(bool showAbsoluteValues READ showAbsoluteValues WRITE setShowAbsoluteValues);
  
  /**
   * Tick positions. Ticks can be drawn on either or both sides of the
   * measure bar. The default is on both sides.
   */
  Q_PROPERTY(TickPosition tickPosition READ tickPosition WRITE setTickPosition);
  
  /**
   * Enables or disables mouse location indicators. Use the
   * [setMouseLocation()] slot to change the location to which the
   * indicator is drawn. The default value is `false`.
   */
  Q_PROPERTY(bool drawMouseLocation READ drawMouseLocation WRITE setDrawMouseLocation);

  /**
   * Enables or disables text of the mouse location indicators. Use the
   * [setMouseLocation()] slot to change the location to which the
   * indicator is drawn. The default value is `false`. This works
   * only if drawMouseLocation is true.
   */
  Q_PROPERTY(bool drawMouseText READ drawMouseText WRITE setDrawMouseText);

  Q_ENUMS(UnitSystem);
  Q_FLAGS(TickPositionFlags);
  
public:
  /**
   * An interface for classes that format numbers for viewing.
   */
  class NumberFormatter
  {
  public:
    virtual ~NumberFormatter();
    /**
     * Converts *value* to a string that will be shown on the measure
     * bar.
     *
     * @param value the value to be shown
     *
     * @param scale the scale of the value, as given by [tickScales]. 
     * value*scale is the value in logical units.
     *
     * @param unit the unit for the value, as given by [tickNames]. If
     * no unit should be shown on this tick scale or units are
     * disabled, *unit* will be an empty string.
     */
    virtual QString formatNumber(double value, double scale, const QString& unit) const = 0;
  };
  
  /**
   * Determines the side on which the ticks will be drawn. The
   * following values are possible:
   *
   * - `TicksTopOrRight` - the ticks will be drawn either on the
   * right or the top side of the measure bar, depenging on the
   * orientation.
   *
   * - `TicksBottomOrLeft` - the scale will be drawn either on the
   * left or the bottom side of the measure bar, depenging on the
   * orientation.
   */
  enum TickPositionFlag
  {
    NoTicks = 0,
    TicksTopOrRight = 1,
    TicksBottomOrLeft = 2
  };
  Q_DECLARE_FLAGS(TickPosition, TickPositionFlag);

  
  PiiMeasureBar(QWidget* parent = 0);
  PiiMeasureBar(Qt::Orientation orientation, QWidget* parent = 0);
  ~PiiMeasureBar();

  void setOrientation(const Qt::Orientation& orientation);
  Qt::Orientation orientation() const;

  void setMinTickDistances(const QVariantList& distances);
  QVariantList minTickDistances() const;

  QVariantList tickScales() const;
  void setTickScales(const QVariantList& tickScales);

  void setTickNames(const QStringList& tickNames);
  QStringList tickNames() const;
  
  void setDrawUnitNames(bool status);
  bool drawUnitNames() const;

  void setShowAbsoluteValues(bool showAbsoluteValues);
  bool showAbsoluteValues() const;
  
  TickPosition tickPosition() const;
  void setTickPosition(TickPosition tickPosition);

  double physicalPixelSize() const;
  double scale() const;
  double startPos() const;

  bool drawMouseLocation() const;
  bool drawMouseText() const;

  /**
   * Set a custom number formatter. If *formatter* is 0, a default
   * formatter will be used. The default formatter uses
   * QString::number() to convert the number to a string, and
   * optionally appends a space and the unit name to it.
   */
  void setNumberFormatter(NumberFormatter* formatter);
  NumberFormatter* numberFormatter() const;
  
public slots:
  void setPhysicalPixelSize(double size);
  void setScale(double scale);
  
  void setStartPos(double length);

  void setDrawMouseLocation(bool drawMouseLocation);
  void setDrawMouseText(bool drawMouseText);
  
  /**
   * Sets the location of the mouse cursor in pixels. This function
   * stores either the x or y coordinate of the mouse location
   * depending on the orientation of the measure bar.
   */
  void setMouseLocation(const QPoint& location);

  /**
   * Sets the location of the indicator in pixels, counting from the
   * top/left edge.
   */
  void setMouseLocation(int location);

signals:
  void mouseOnMeasureBar(bool);
  void mouseLocationChanged(const QPoint&);
  void mouseClicked(const QPoint&);
  
protected:
  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *event);
  
private:
  class Data
  {
  public:
    Data(Qt::Orientation);
    
    Qt::Orientation orientation;
    TickPosition tickPosition;
    double dPhysicalPixelSize, dPixelSize;
    double dScale;
    double dStartPos; // either in x or y direction
    int iMouseLocation;
    bool bDrawMouseLocation;
    bool bDrawMouseText;
    
    QVector<int> lstMinTickDistances;
    QVector<double> lstTickScales;
    QStringList lstTickNames;
    bool bDrawUnitNames;
    bool bShowAbsoluteValues;
    
    QVector<int> lstLineLengths;
    QVector<double> lstSteps;

    // Contains the index of the biggest scale.
    int iCurrBigScaleIndex;
    // Contains the index of the basic unit. Cannot contain an empty
    // string or unit starting with '-'- or '+'-sign. Has the 
    int iCurrUnitIndex;
    NumberFormatter* pFormatter;
  } *d;

  void init();
  void updateLineLengths();
  void updateStepList();
  void updateUnitIndex();
  void updatePixelSize();
  
  static NumberFormatter* defaultFormatter();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PiiMeasureBar::TickPosition);

#endif //_PIIMEASUREBAR_H
