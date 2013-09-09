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

#ifndef _PIIOBJECTCAPTURER_H
#define _PIIOBJECTCAPTURER_H

#include <PiiDefaultOperation.h>
#include <QVariantList>

/**
 * Emits received objects as Qt signals.
 *
 * @inputs
 *
 * @in sync - sync input. Any type. If this input is not connected,
 * each incoming object (or a set of objects) will be sent once
 * received. If this input is connected, every object related to the
 * sync object will be sent once everything has been received.
 *
 * @in inputX - reads in objects of any type. X ranges from 1 to
 * #dynamicInputCount - 1. @p input0 can also be accessed as @p input.
 * 
 * @ingroup PiiBasePlugin
 */
class PiiObjectCapturer : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * The number of dynamic inputs (in addition to @p sync). The
   * default is one.
   */
  Q_PROPERTY(int dynamicInputCount READ dynamicInputCount WRITE setDynamicInputCount);

  /**
   * Specifies how input objects are collected to lists if @p sync is
   * connected. The default is @p OneListPerInput.
   */
  Q_PROPERTY(ListMode listMode READ listMode WRITE setListMode);
  Q_ENUMS(ListMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Different ways of collecting many objects to lists.
   *
   * @lip OneListPerInput - one list is maintained for each input. 
   * Objects related to one @p sync object are collected to
   * input-specific lists. These lists are put into a compound list
   * whose length becomes #dynamicInputCount.
   *
   * @lip OneListPerCycle - all input objects are collected to a list
   * on each processing round so that the object received from @p
   * input0 becomes the first element. Each list holds
   * #dynamicInputCount elements. These lists are put into a compound
   * list whose length is the number of processing cycles related to
   * one @p sync object.
   */
  enum ListMode { OneListPerInput, OneListPerCycle };
  
  PiiObjectCapturer();
  ~PiiObjectCapturer();
    
  /**
   * Aliases @p input to @p input0.
   */
  PiiInputSocket* input(const QString &name) const;

  void check(bool reset);

signals:
  /**
   * Emitted for each incoming object if the @p sync input is not
   * connected and if #dynamicInputCount is one.
   */
  void objectCaptured(const PiiVariant& object);
  /**
   * Emitted for each incoming object if the @p sync input is not
   * connected and if #dynamicInputCount is greater than one. Each
   * element in the list will be a PiiVariant. The first object is the
   * one read from @p input0 and so on.
   */
  void objectsCaptured(const QVariantList& objects);
  /**
   * Emitted when everything related to the object in the @p sync
   * input has been received.
   *
   * @param syncObject the object received in the @p sync input
   *
   * @param objects all collected objects as a list of lists that
   * contain PiiVariants.
   */
  void objectsCaptured(const PiiVariant& syncObject, const QVariantList& objects);
  
protected:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiInputSocket *pSyncInput;
    PiiVariant syncObject;
    QList<QVariantList> lstObjects;
    int iDynamicInputCount;
    ListMode listMode;
  };
  PII_D_FUNC;

  void process();
  void syncEvent(SyncEvent* event);

  void setDynamicInputCount(int dynamicInputCount);
  int dynamicInputCount() const;

  void setListMode(ListMode listMode);
  ListMode listMode() const;

private:
  void initObjectList();
  void clearObjects();
};


#endif //_PIIOBJECTCAPTURER_H
