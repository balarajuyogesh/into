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

#include "PiiMatrixUtil.h"
#include <QRegExp>
#include <QStringList>

namespace Pii
{
  PiiMatrix<double> matlabParse(const QString& str)
  {
    QString strCopy(str.trimmed());
    if (strCopy.size() > 0 && strCopy[0] == '[')
      strCopy.remove(0,1);
    if (strCopy.size() > 0 && strCopy[strCopy.size()-1] == ']')
      strCopy.chop(1);
    // Remove line break markers (ellipsis in Matlab)
    strCopy.replace("...", "");
    // Rows are separated by semicolons or newlines
    QStringList strlstRows = strCopy.split(QRegExp(" *; *\\r?\\n?|\\r?\\n"), QString::SkipEmptyParts);
    // Elements are separated with either spaces or commas. Any
    // sequence of separators with nothing else in between will be
    // treated as one separator.
    QRegExp reElementSep("[, \t]+");
    QList<QList<double> > dlstRows;
    int iMaxCols = 0;
    for (int r=0; r<strlstRows.size(); r++)
      {
        QStringList strlstRow = strlstRows[r].split(reElementSep, QString::SkipEmptyParts);
        int iColCnt = strlstRow.size();
        // Find maximum width
        if (iColCnt > iMaxCols)
          iMaxCols = iColCnt;
        QList<double> dlstRow;
        for (int c=0; c<strlstRow.size(); c++)
          dlstRow << strlstRow[c].toDouble();
        dlstRows << dlstRow;
      }

    if (iMaxCols == 0)
      return PiiMatrix<double>();

    // Store values into the result matrix
    PiiMatrix<double> result(dlstRows.size(), iMaxCols);
    for (int r=0; r<dlstRows.size(); r++)
      for (int c=0; c<dlstRows[r].size(); c++)
        result(r,c) = dlstRows[r][c];
    return result;
  }
}
