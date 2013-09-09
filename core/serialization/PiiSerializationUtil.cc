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

#include "PiiSerializationUtil.h"
#include "PiiSerializableExport.h"
#include <PiiConstCharWrapper.h>

PII_SERIALIZABLE_EXPORT(QObject);

namespace PiiSerialization
{
  const PiiMetaObject* metaObjectPointer(const QObject* obj)
  {
    // Static map of already-created PiiMetaObjects.
    static QHash<PiiConstCharWrapper, PiiMetaObject*> map;

    // First see if we already have fetched a meta-object for this
    // type.
    const QMetaObject* pQMetaObj = obj->metaObject();
    const char* pClassName = pQMetaObj->className();
    const PiiMetaObject* pMetaObj = map.value(pClassName, 0);
    if (pMetaObj != 0)
      return pMetaObj;

    unsigned uiVersion = 0;
    // Find the version class info.
    for (int i=0; i<pQMetaObj->classInfoCount(); ++i)
      {
        if (!strcmp(pQMetaObj->classInfo(i).name(), "version"))
          {
            uiVersion = QString(pQMetaObj->classInfo(i).value()).toUInt();
            break;
          }
      }
    // Now we have all the information. Create a new meta-object and
    // store it to the map.
    PiiMetaObject* pNewObj = new PiiMetaObject(pClassName, uiVersion, true);
    map.insert(pClassName, pNewObj);
    return pNewObj;
  }
}
