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

#include "PiiConfigurationWidget.h"

PiiConfigurationWidget::Data::Data() :
  bAcceptable(true),
  bChanged(false)
{
}

PiiConfigurationWidget::PiiConfigurationWidget(QWidget* parent) :
  QWidget(parent),
  d(new Data)
{}

PiiConfigurationWidget::~PiiConfigurationWidget()
{
  delete d;
}


bool PiiConfigurationWidget::canAccept() const
{
  return d->bAcceptable;
}

void PiiConfigurationWidget::acceptChanges()
{ }

bool PiiConfigurationWidget::canReset() const
{
  return false;
}

void PiiConfigurationWidget::reset()
{ }

bool PiiConfigurationWidget::hasDefaults() const
{
  return false;
}

void PiiConfigurationWidget::setDefaults()
{ }

bool PiiConfigurationWidget::hasChanged() const
{
  return d->bChanged;
}

void PiiConfigurationWidget::setChanged(bool changed)
{
  d->bChanged = changed;
  emit contentsChanged(changed);
}

void PiiConfigurationWidget::setAcceptable(bool acceptable)
{
  d->bAcceptable = acceptable;
  emit acceptStateChanged(acceptable);
}
