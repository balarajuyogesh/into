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

#include "PiiTriggerButton.h"
#include <PiiQtWidgets.h>

PiiTriggerButton::Data::Data(PiiTriggerButton::Buttons b) :
  buttons(b)
{
}


PiiTriggerButton::PiiTriggerButton(Buttons buttons) :
  d(new Data(buttons))
{
  QHBoxLayout* pLayout = new QHBoxLayout(this);
  d->pLeft = new QToolButton;
  d->pLeft->setArrowType(Qt::LeftArrow);
  pLayout->addWidget(d->pLeft);
  d->pRight = new QToolButton;
  d->pRight->setArrowType(Qt::RightArrow);
  pLayout->addWidget(d->pRight);
  setLayout(pLayout);

  QSignalMapper* pMapper = new QSignalMapper(this);
  pMapper->setMapping(d->pLeft, -1);
  pMapper->setMapping(d->pRight, 1);
  connect(d->pLeft, SIGNAL(clicked()), pMapper, SLOT(map()));
  connect(d->pRight, SIGNAL(clicked()), pMapper, SLOT(map()));
  connect(pMapper, SIGNAL(mapped(int)), this, SIGNAL(triggered(int)));

  setButtons(buttons);
}

PiiTriggerButton::~PiiTriggerButton()
{
  delete d;
}

void PiiTriggerButton::setButtons(Buttons buttons)
{
  d->buttons = buttons;
  d->pLeft->setVisible(buttons & Left);
  d->pRight->setVisible(buttons & Right);
}

PiiTriggerButton::Buttons PiiTriggerButton::buttons() const
{
  return d->buttons;
}
