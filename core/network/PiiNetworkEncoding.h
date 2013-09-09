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

#ifndef _PIINETWORKENCODING_H
#define _PIINETWORKENCODING_H

#include "PiiNetwork.h"

#include <PiiSerializationUtil.h>
#include <PiiGenericTextInputArchive.h>
#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericBinaryInputArchive.h>
#include <PiiGenericBinaryOutputArchive.h>

/// @cond null
namespace PiiNetwork
{
  template <class T> QByteArray toByteArray(const T& obj, EncodingFormat format)
  {
    switch (format)
      {
      case TextFormat:
        return PiiSerialization::toByteArray<PiiGenericTextOutputArchive>(obj);
      case BinaryFormat:
        return PiiSerialization::toByteArray<PiiGenericBinaryOutputArchive>(obj);
      }
    return QByteArray();
  }

  template <class T> T fromByteArray(const QByteArray& data)
  {
    T result;
    if (data.startsWith(PII_TEXT_ARCHIVE_ID))
      PiiSerialization::fromByteArray<PiiGenericTextInputArchive>(data, result);
    else // if (data.startsWith(PII_BINARY_ARCHIVE_ID))
      PiiSerialization::fromByteArray<PiiGenericBinaryInputArchive>(data, result);
    return result;
  }
}
/// @endcond

#endif //_PIINETWORKENCODING_H
