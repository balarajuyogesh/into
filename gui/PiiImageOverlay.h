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

#ifndef PIIIMAGEOVERLAY_H
#define PIIIMAGEOVERLAY_H

#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QRect>
#include <QLine>
#include <QPainterPath>

#include "PiiGui.h"

/**
 * Image overlay is a layer that is drawn on PiiImageDisplay
 * Each overlay has a paint method that is called by display.
 *
 */
class PII_GUI_EXPORT PiiImageOverlay
{
public:
  virtual ~PiiImageOverlay();
  /**
   * Paint the overlay to `painter`. If `filled` is `true`, the
   * overlay shape shall be filled with. Otherwise just its boundary
   * will be drawn.
   */
  virtual void paint(QPainter* painter, bool filled = true) = 0;
  /**
   * Returns true if the overlay intersects *r*. This is used for
   * paint optimization. The default implementation returns `true`.
   */
  virtual bool intersects(const QRect& r);

  /**
   * Returns true if the overlay contains the point *p*. The default
   * implementation returns `false`.
   */
  virtual bool contains(const QPoint& p);

  void setToolTipText(const QString& text);
  QString toolTipText() const;

  void setEnabled(bool enabled);
  bool enabled() const;

protected:
  /// @internal
  class Data
  {
  public:
    Data();
    ~Data();
    QString strToolTipText;
    bool bEnabled;
  } *d;
  /// @internal
  PiiImageOverlay(Data* d);

  PiiImageOverlay();
};

/**
 * Geometric overlays are drawn with a pen and optionally filled with
 * a brush. They represent arbitrary geometric shapes.
 *
 */
class PII_GUI_EXPORT PiiGeometricOverlay : public PiiImageOverlay
{
public:
  PiiGeometricOverlay();
  PiiGeometricOverlay(const QBrush& bg, const QPen& border);
  ~PiiGeometricOverlay();

  void setBrush(const QBrush& brush);
  QBrush brush() const;
  void setPen(const QPen& pen);
  QPen pen() const;

protected:
  /// @internal
  class Data : public PiiImageOverlay::Data
  {
  public:
    Data();
    Data(const QBrush& bg, const QPen& border);

    QBrush brush;
    QPen pen;
  };
  PII_D_FUNC;
  /// @internal
  PiiGeometricOverlay(Data* d);
};

class PII_GUI_EXPORT PiiRectangleOverlay : public PiiGeometricOverlay
{
public:
  PiiRectangleOverlay();
  PiiRectangleOverlay(const QRect& rectangle);
  ~PiiRectangleOverlay();

  void paint(QPainter* p, bool filled = true);

  void setRectangle(const QRect& rectangle);
  QRect rectangle() const;
  QRect& rectangle();

  bool intersects(const QRect& r);
  bool contains(const QPoint& r);

protected:
  /// @internal
  class Data : public PiiGeometricOverlay::Data
  {
  public:
    Data();
    Data(const QRect& rectangle);
    QRect rectangle;
  };
  PII_D_FUNC;
};


class PII_GUI_EXPORT PiiPolygonOverlay : public PiiGeometricOverlay
{
public:
  PiiPolygonOverlay();
  PiiPolygonOverlay(const QPainterPath& s);
  ~PiiPolygonOverlay();

  void paint(QPainter* p, bool filled = true);

  void setShape(const QPainterPath& shape);
  QPainterPath shape() const;

  bool intersects(const QRect& r);
  bool contains(const QPoint& r);

private:
  /// @internal
  class Data : public PiiGeometricOverlay::Data
  {
  public:
    Data();
    Data(const QPainterPath& shape);
    QPainterPath shape;
  };
  PII_D_FUNC;
};

class PII_GUI_EXPORT PiiEllipseOverlay : public PiiRectangleOverlay
{
public:
  PiiEllipseOverlay();
  PiiEllipseOverlay(const QRect& rectangle);
  ~PiiEllipseOverlay();

  void paint(QPainter* p, bool filled = true);

  bool intersects(const QRect& r);
  bool contains(const QPoint& r);
};


class PII_GUI_EXPORT PiiCrossOverlay : public PiiImageOverlay
{
public:
  PiiCrossOverlay();
  PiiCrossOverlay(const QPoint& point);
  ~PiiCrossOverlay();

  void paint(QPainter* p, bool filled = true);

  void setPen(const QPen& pen);
  QPen pen() const;

  bool intersects(const QRect& r);
  bool contains(const QPoint& r);

private:
  /// @internal
  class Data : public PiiImageOverlay::Data
  {
  public:
    Data();
    Data(const QPoint& point);
    QPen pen;
    QPoint point;
  };
  PII_D_FUNC;
};

class PII_GUI_EXPORT PiiLineOverlay : public PiiImageOverlay
{
public:
  PiiLineOverlay();
  PiiLineOverlay(const QLine& line);
  void paint(QPainter* p, bool filled = true);

  void setPen(const QPen& pen);
  QPen pen() const;

  QLine line() const;
  void setLine(const QLine& line);

private:
  /// @internal
  class Data : public PiiImageOverlay::Data
  {
  public:
    Data();
    Data(const QLine& line);
    QPen pen;
    QLine line;
  };
  PII_D_FUNC;
};

class PII_GUI_EXPORT PiiStringOverlay : public PiiImageOverlay
{
public:
  PiiStringOverlay();
  PiiStringOverlay(const QRect& rect, const QString& text);
  void paint(QPainter* p, bool filled = true);

  void setRectangle(const QRect& rect);
  void setText(const QString& text);
  void setFont(const QFont& font);
  void setTextFlags(int flags);
  void setShowBorders(bool show);

  void setPen(const QPen& pen);
  QPen pen() const;

  bool intersects(const QRect& r);
  bool contains(const QPoint& r);

private:
  /// @internal
  class Data : public PiiImageOverlay::Data
  {
  public:
    Data();
    Data(const QRect& rect, const QString& text);
    QFont font;
    QPen pen;
    QRect rect;
    QString strText;
    bool bShowBorders;
    int flags;
  };
  PII_D_FUNC;
};
#endif
