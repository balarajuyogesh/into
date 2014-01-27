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

#include "PiiFlowController.h"

PiiFlowController::Data::Data() :
  iActiveInputGroup(0)
{
}

PiiFlowController::Data::~Data()
{
}

PiiFlowController::PiiFlowController() :
  d(new Data)
{
}

PiiFlowController::PiiFlowController(Data* data) :
  d(data)
{
}

PiiFlowController::~PiiFlowController()
{
  delete d;
}

int PiiFlowController::activeInputGroup() const
{
  return d->iActiveInputGroup;
}

void PiiFlowController::setActiveInputGroup(int group)
{
  d->iActiveInputGroup = group;
}

bool PiiFlowController::hasSyncEvents() const
{
  return false;
}

void PiiFlowController::sendSyncEvents(SyncListener* /*listener*/)
{
}

void PiiFlowController::setPropertySetName(const QString& propertySetName) { d->strPropertySetName = propertySetName; }
QString PiiFlowController::propertySetName() const { return d->strPropertySetName; }

PiiFlowController::SyncListener::~SyncListener() {}
