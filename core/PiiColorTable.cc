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

#include "PiiColorTable.h"

namespace Pii
{
  static QVector<QRgb> createGrayColorTable()
  {
    QVector<QRgb> vecGray;
    vecGray.reserve(256);
    for (int i=0; i<256; ++i)
      vecGray.push_back(qRgb(i,i,i));
    return vecGray;
  }

  const QVector<QRgb>& grayColorTable()
  {
    static QVector<QRgb> vecGray(createGrayColorTable());
    return vecGray;
  }
}
