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

#ifndef _PIICONFIGURABLE_H
#define _PIICONFIGURABLE_H

#include "PiiGlobal.h"
#include <QVariant>

class PII_CORE_EXPORT PiiConfigurable
{
public:
  virtual ~PiiConfigurable();

  virtual bool setProperty(const char* name, const QVariant& value) = 0;
  virtual QVariant property(const char* name) const = 0;
};

Q_DECLARE_INTERFACE(PiiConfigurable, "com.intopii.PiiConfigurable/1.0");

#endif //_PIICONFIGURABLE_H
