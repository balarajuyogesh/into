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

#ifndef _PIIOBJECTFACTORY_H
#define _PIIOBJECTFACTORY_H

#include <QObject>
#include "PiiGlobal.h"

/**
 * An interface for object factories that are able to create objects
 * based on a class name.
 */
class PII_CORE_EXPORT PiiObjectFactory
{
public:
  virtual ~PiiObjectFactory();

  /**
   * Create an instance of the named object.
   *
   * @param name the class name of the object (case-sensitive)
   *
   * @return a pointer to a newly created object. The caller is
   * responsible for deleting the object. If the object cannot be
   * created, 0 is returned.
   */
  virtual QObject* create(const QString& name) = 0;

  /**
   * List the class names this factory can create.
   *
   * @return the class names of known objects
   */
  virtual QStringList keys() const = 0;
};

#endif //_PIIOBJECTFACTORY_H
