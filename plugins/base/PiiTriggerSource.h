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

#ifndef _PIITRIGGERSOURCE_H
#define _PIITRIGGERSOURCE_H

#include <PiiBasicOperation.h>
#include <QMutex>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>

/**
 * A source operation that can be triggered with Qt's signals. This
 * operation is useful when one needs to send values from a user
 * interface to the analysis engine.
 *
 * Outputs
 * -------
 *
 * @out trigger - any object received into the [trigger()] slot.
 *
 */
class PiiTriggerSource : public PiiBasicOperation
{
  Q_OBJECT;
  
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiTriggerSource();

  void stop();

  void pause();

  void start();

  void interrupt();

  void reconfigure(const QString& propertySetName = QString());
  
public slots:
  /**
   * Sends an `int` (1) to the output socket.
   */
  void trigger();
  /**
   * Sends a PiiVariant to the output socket.
   */
  void trigger(const PiiVariant& value);
  /**
   * Sends an `int` to the output socket.
   */
  void trigger(int value);
  /**
   * Sends a `double` to the output socket.
   */
  void trigger(double value);
  /**
   * Sends a QString to the output socket.
   */
  void trigger(const QString& value);
  /**
   * Sends the point as a 1-by-2 PiiMatrix<int> (x,y).
   */
  void trigger(const QPoint& value);
  /**
   * Sends the point as a 1-by-2 PiiMatrix<double> (x,y).
   */
  void trigger(const QPointF& value);
  /**
   * Sends the rectangle as a 1-by-4 PiiMatrix<int>
   * (x,y,width,height).
   */
  void trigger(const QRect& value);
  /**
   * Sends the rectangle as a 1-by-4 PiiMatrix<double>
   * (x,y,width,height).
   */
  void trigger(const QRectF& value);

private:
  /**
   * Sends a given value to the output socket.
   */
  template <class T> void emitValue(T value);

  /// @internal
  class Data : public PiiBasicOperation::Data
  {
  public:
    Data();
    
    QMutex stateMutex;
  };
  PII_D_FUNC;

};

#endif //_PIITRIGGERSOURCE_H
