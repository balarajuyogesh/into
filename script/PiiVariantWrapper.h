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

#ifndef _PIIVARIANTWRAPPER_H
#define _PIIVARIANTWRAPPER_H

#include <PiiVariant.h>

class PiiVariantScriptObject : public QObject
{
  Q_OBJECT
public:
  PiiVariantScriptObject(const PiiVariant& var) : variant(var) {}

  Q_INVOKABLE QString toString() const;
  Q_INVOKABLE int toInt() const;
  Q_INVOKABLE double toDouble() const;
  Q_INVOKABLE bool toBool() const;

  PiiVariant variant;
};

class QScriptEngine;
void initPiiVariant(QScriptEngine* engine);

#endif //_PIIVARIANTWRAPPER_H
