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

#ifndef _PIIIMAGEANNOTATOR_H
#define _PIIIMAGEANNOTATOR_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>
#include <PiiQImage.h>
#include <PiiColor.h>
#include <QPair>
#include <QBrush>
#include <QPen>


/**
 * An operation that draws graphics primitives on an image. Supported
 * annotations include text, points, lines, rectangles, ellipses, and
 * circles.
 *
 * Inputs
 * ------
 *
 * @in image - the image to be annotated
 *
 * @in annotation - annotation to be drawn on the image. Different
 * annotations are defined with different data structures (see
 * [AnnotationType]). This input is optional.
 *
 * @in type - the type of the annotation as an integer. This input can
 * be used if the type may change at run time. See [AnnotationType] for
 * valid values. This input is optional and can only be connected if
 * `annotation` is connected.
 *
 * Outputs
 * -------
 *
 * @out image - the annotated image output
 *
 */
class PiiImageAnnotator : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Annotation type. If the `type` input is not connected, all items
   * received from the `annotation` input are assumed to be of this
   * type. The default is `Auto`.
   */
  Q_PROPERTY(AnnotationType annotationType READ annotationType WRITE setAnnotationType);
  Q_ENUMS(AnnotationType);

  /**
   * The font used for textual annotations.
   */
  Q_PROPERTY(QFont font READ font WRITE setFont);

  /**
   * The brush used for annotations.
   */
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush);

  /**
   * The pen used for annotations.
   */
  Q_PROPERTY(QPen pen READ pen WRITE setPen);

  /**
   * Text position. This property has an effect only with `Text` type
   * annotations. The string read from the `annotation` input will be
   * placed on the given coordinates. The default is (0,0).
   */
  Q_PROPERTY(QPoint textPosition READ textPosition WRITE setTextPosition);

  /**
   * List of annotations to be always drawn. Each entry in this list
   * must be a QVariantMap that stores the properties of the
   * annotation.
   */
  Q_PROPERTY(QVariantList annotations READ annotations WRITE setAnnotations);

  /**
   * Disable/enable operation. If operation has disabled, we will only
   * emit the image object forward.
   */
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:

  PiiImageAnnotator();
  ~PiiImageAnnotator();

  /**
   * Supported annotation types.
   *
   * - `Text` - the input is read as a QString. Primitive types will
   * be automatically converted to strings. See [textPosition].
   *
   * - `Point` - the input is a N-by-2 matrix in which each row stores
   * point coordinates (x,y).
   *
   * - `Line` - the input is a N-by-4 matrix in which each row stores
   * the start and end coordinates of the line (x1,y1,x2,y2).
   *
   * - `Rectangle` - the input is a N-by-4 matrix in which each row
   * stores the upper left corner coordinates, width, and height
   * (x,y,w,h).
   *
   * - `Ellipse` - the input is a N-by-4 matrix in which each row
   * stores a rectangle framing the ellipse (x,y,w,h)
   *
   * - `Circle` - the input is a N-by-3 matrix in which each row
   * stores the center point and radius of a circle (x,y,r).
   *
   * - `Auto` - the number of columns in the input determines the
   * type. 2 columns is a point, 3 columns a circle, and 4 columns a
   * rectangle.
   *
   * All numeric types are accepted as coordinates.
   */
  enum AnnotationType { Text, Point, Line, Rectangle, Ellipse, Circle, Auto };

  AnnotationType annotationType() const;
  void setAnnotationType(AnnotationType annotationType);

  void setFont(const QFont& font);
  QFont font() const;

  QBrush brush() const;
  void setBrush(QBrush brush);

  QPen pen() const;
  void setPen(QPen pen);

  void setTextPosition(const QPoint& textPosition);
  QPoint textPosition() const;

  void setAnnotations(const QVariantList& annotations);
  QVariantList annotations() const;

  void setEnabled(bool enabled);
  bool enabled() const;

  void check(bool reset);

protected:
  void process();

private:

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();

    template <class T> void annotate(const PiiVariant& obj);
    template <class T> void annotate(QImage* image, const PiiVariant& annotation, AnnotationType type);
    void drawText(QImage* image, const QString& text);
    void drawAnnotations(QImage*, const QVariantList& annotations);
    template <class T, class U> void drawAnnotation(QPainter* painter, const QVariantMap& annotation);

    AnnotationType annotationType;
    QFont font;
    QBrush brush;
    QPen pen;
    QPoint textPosition;
    QVariantList lstAnnotations;

    PiiInputSocket* pImageInput, *pAnnotationInput, *pTypeInput;
    bool bAnnotationConnected, bTypeConnected;

    PiiOutputSocket* pImageOutput;
    bool bEnabled;
  };
  PII_D_FUNC;

};


#endif //_PIIIMAGEANNOTATOR_H
