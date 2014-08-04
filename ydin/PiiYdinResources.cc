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

#include "PiiYdinResources.h"
#include <cstring>

namespace PiiYdin
{
  QString resourceConnectorName(const QString& resource1Name,
                                const QString& resource2Name,
                                const QString& role)
  {
    using namespace Pii;
    int id = resourceDatabase()->findFirst(subject == resource1Name &&
                                           predicate == role &&
                                           object == resource2Name);
    if (id == -1)
      return 0;

    QList<QString> lstConnectors = resourceDatabase()->select(object,
                                                              resourceIdToInt(subject) == id &&
                                                              predicate == connectorPredicate);
    if (lstConnectors.size() == 0)
      return QString();

    return lstConnectors[0];
  }

  static int pointerOffset(int id)
  {
    using namespace Pii;
    QList<int> lstOffsets = resourceDatabase()->select(resourceStringTo<int>(object),
                                                       resourceIdToInt(subject) == id &&
                                                       predicate == offsetPredicate);
    return lstOffsets.size() == 0 ? 0 : lstOffsets[0];
  }

  int pointerOffset(const char* superClass, const char* subClass)
  {
    // Superclass and subclass are the same.
    if (!std::strcmp(superClass, subClass))
      return 0;

    using namespace Pii;
    // First, we search all direct superclasses of the resource.
    QList<PiiResourceStatement> lstSuperClasses = resourceDatabase()->select(subject == subClass &&
                                                                             predicate == classPredicate);
    for (int i=0; i<lstSuperClasses.size(); ++i)
      if (lstSuperClasses[i].object() == superClass)
        return pointerOffset(lstSuperClasses[i].id());

    // Not a direct superclass. Recurse.
    for (int i=0; i<lstSuperClasses.size(); ++i)
      {
        int iOffset = pointerOffset(superClass, qPrintable(lstSuperClasses[i].object()));
        if (iOffset >= 0)
          return iOffset + pointerOffset(lstSuperClasses[i].id());
      }
    return -1;
  }
}

