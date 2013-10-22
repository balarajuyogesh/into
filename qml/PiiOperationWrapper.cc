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

#include "PiiOperationWrapper.h"
#include "PiiQml.h"

#include <PiiOperation.h>

namespace PiiOperationWrapper
{
  PII_STATIC_TR_FUNC(PiiOperation)

  static PII_DECLARE_V8_FUNCTION(stateName)
  {
    PII_CHECK_ONE_ARGUMENT(stateName);
    v8::Local<v8::Value> state(args[0]);
    if (state->IsUint32() || state->IsInt32())
      {
        const char* pStateName = PiiOperation::stateName(PiiOperation::State(state->Uint32Value()));
        if (pStateName == 0)
          return v8::Undefined();
        return v8::String::New(pStateName);
      }
    PII_INVALID_ARGUMENT_TYPE(stateName, 0, "integer");
  }

  void init(QV8Engine* engine, v8::Handle<v8::Object> globalObject)
  {
    v8::Local<v8::Function> constructor(PII_CONSTRUCTOR_OF("PiiOperation"));
    constructor->Set(v8::String::New("stateName"), PII_V8_FUNCTION(stateName));
  }
}
