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

#include "PiiOperationCompoundWrapper.h"
#include "PiiQml.h"

#include <PiiOperationCompound.h>

namespace PiiOperationCompoundWrapper
{
  PII_STATIC_TR_FUNC(PiiOperationCompound)

  static PII_DECLARE_V8_FUNCTION(addOperations)
  {
    PII_CHECK_THIS_TYPE(PiiOperationCompound, addOperations);
    PII_CHECK_AT_LEAST_ONE_ARGUMENT(addOperations);

    for (int i=0; i<args.Length(); ++i)
      {
        PiiOperation* pOperation;
        PII_CHECK_ARGUMENT_TYPE(addOperations, i, pOperation, PiiOperation);
        pThis->addOperation(pOperation);
      }
    return v8::Undefined();
  }

  void init(QV8Engine* engine, v8::Handle<v8::Object> globalObject)
  {
    v8::Local<v8::Object> prototype(PII_PROTO_OF("PiiOperationCompound"));
    prototype->Set(v8::String::New("addOperations"), PII_V8_FUNCTION(addOperations));
  }
}
