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

#include "PiiRoi.h"
#include <PiiInputSocket.h>
#include <PiiYdinTypes.h>

namespace PiiImage
{
  const char* roiRectangleSizeError =
    QT_TRANSLATE_NOOP("PiiRoi", "Region-of-interest rectangle (%1, %2), (%3-by-%4) exceeds image boundaries (%5-by-%6).");
  const char* roiMaskSizeError =
    QT_TRANSLATE_NOOP("PiiRoi", "Region-of-interest mask size (%1-by-%2) doesn't match image size (%3-by-%4).");

  PiiMatrix<bool> toRoiMask(const PiiVariant& obj)
  {
    switch (obj.type())
      {
      case PiiYdin::BoolMatrixType:
      case PiiYdin::CharMatrixType:
      case PiiYdin::UnsignedCharMatrixType:
        // HACK: All 8-bit data types can be treated as bool
        return obj.valueAs<PiiMatrix<bool> >();
      case PiiYdin::ShortMatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<short> >());
      case PiiYdin::IntMatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<int> >());
      case PiiYdin::Int64MatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<qint64> >());
      case PiiYdin::UnsignedShortMatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<short> >());
      case PiiYdin::UnsignedIntMatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<int> >());
      case PiiYdin::UnsignedInt64MatrixType:
        return PiiMatrix<bool>(obj.valueAs<PiiMatrix<quint64> >());
      default:
        PII_THROW(PiiExecutionException,
                  QCoreApplication::translate("PiiRoi",
                                              "Cannot convert an object of type 0x%1 "
                                              "to a region-of-interest mask.").arg(obj.type(), 0, 16));
      }
    return PiiMatrix<bool>();
  }
}
