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

#ifndef _PIIYDINUTIL_H
#define _PIIYDINUTIL_H

#include "PiiYdin.h"
#include "PiiOperationCompound.h"
#include <QString>

namespace PiiYdin
{
  enum IllustrationFlag
    {
      ShowOnlyOperation = 0x0,
      ShowInputQueues = 0x1,
      ShowOutputStates = 0x2,
      ShowState = 0x3,
      ShowAll = -1
    };

  Q_DECLARE_FLAGS(IllustrationFlags, IllustrationFlag);
  Q_DECLARE_OPERATORS_FOR_FLAGS(IllustrationFlags);

  /**
   * Creates an ascii-graphics illustration of an operation. The
   * result can be printed on a console. This function is mainly
   * useful for debugging purposes.
   */
  PII_YDIN_EXPORT QString illustrateOperation(PiiOperation* op, IllustrationFlags flags = ShowAll);

  /**
   * Prints out an illustration of *op* and its child operations (if
   * any).
   *
   * @param op the operation to dump to debug output
   *
   * @param flags options that control the appearance of dumped
   * operations
   *
   * @param level nesting level, used in recursive calls
   */
  PII_YDIN_EXPORT void dumpOperation(PiiOperation *op, IllustrationFlags flags = ShowAll, int level = 0);

  /**
   * Dumps the state of an operation and its child operations. This
   * function is mainly useful for debugging purposes.
   *
   * @param stream write state information to this stream
   *
   * @param op the operation whose state is to be dumped
   *
   * @param indent indentation depth
   */
  template <class Stream> void dumpState(Stream stream, PiiOperation* op, int indent = 0)
  {
    for (int i=indent; i--; )
      stream << ' ';

    stream << op->metaObject()->className() << "(" << op->objectName() << "): " << PiiOperation::stateName(op->state()) << "\n";
    PiiOperationCompound* compound = qobject_cast<PiiOperationCompound*>(op);
    if (compound != 0)
      {
        QList<PiiOperation*> children = compound->childOperations();
        ++indent;
        for (int i=0; i<children.size(); ++i)
          dumpState(stream, children[i], indent);
      }
  }

}

#endif //_PIIYDINUTIL_H
