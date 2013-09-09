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

#include "PiiLabeling.h"

namespace PiiImage
{
  // Mark a sequence of detected object pixels into the label buffer
  void markToBuffer(LabelInfo& info, int rowIndex, int start, int end)
  {
    // Mark the run into the label buffer
    int* runRow = info.matLabels[rowIndex];
    for (int c = start; c < end + info.iConnectivityShift; ++c)
      runRow[c] = info.iLabelIndex;
  }

  // On row rowIndex, find all runs that overlap with the range
  // start-end.
  void connectRunsRecursively(LabelInfo& info, int rowIndex, int start, int end)
  {
    // Out of image boundaries...
    if (rowIndex < 0 || rowIndex >= info.lstRuns.size())
      return;

    // Go through all runs on this row and find the overlapping ones
    for (RunNode* pNode = info.lstRuns[rowIndex].first; pNode != 0;)
      {
        // No overlap
        if (start > pNode->end ||
            end < pNode->start)
          {
            pNode = pNode->next;
            continue;
          }

        // Invalidate the current run to prevent loops in recursion
        int end = pNode->end;
        pNode->end = -1;
        pNode->seed = false;
        // Mark and recurse
        markToBuffer(info, rowIndex, pNode->start, end);
        connectRunsRecursively(info, rowIndex - 1, pNode->start, end);
        connectRunsRecursively(info, rowIndex + 1, pNode->start, end);
        // Destroy the node
        info.lstRuns[rowIndex].remove(pNode);
        RunNode* pNodeToDelete = pNode;
        pNode = pNode->next;
        delete pNodeToDelete;
      }
  }
}
