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
#include <QStack>

namespace PiiImage
{
  // Marks a sequence of detected object pixels into the label buffer.
  // Returns the length of the run.
  int markToBuffer(LabelInfo& info, int rowIndex, int start, int end)
  {
    // Mark the run into the label buffer
    int* pRunRow = info.matLabels[rowIndex];
    const int iEnd = end + info.iConnectivityShift;
    const int iLabel = info.iLabelIndex;
    for (int c = start; c < iEnd; ++c)
      pRunRow[c] = iLabel;
    return iEnd - start;
  }

  // Clears the current label out of existence.
  void clearLabel(LabelInfo& info, const BoundingBox& box)
  {
    for (int r = box.minRow; r <= box.maxRow; ++r)
      {
        int* pRow = info.matLabels[r];
        const int iLabel = info.iLabelIndex;
        for (int c = box.minCol; c < box.maxCol + info.iConnectivityShift; ++c)
          if (pRow[c] == iLabel)
            pRow[c] = 0;
      }
  }

  struct RecursiveCall
  {
    int rowIndex;
    int start;
    int end;
  };

  // On row rowIndex, find all runs that overlap with the range
  // start-end.
  int connectRuns(LabelInfo& info, int rowIndex, int start, int end, BoundingBox& box)
  {
    QStack<RecursiveCall> localStack;
    RecursiveCall currentCall = { rowIndex, start, end };
    localStack.push(currentCall);

    int iSize = 0;

    while (!localStack.isEmpty())
      {
        currentCall = localStack.pop();
        // Out of image boundaries...
        if (currentCall.rowIndex < 0 || currentCall.rowIndex >= info.lstRuns.size())
          continue; // to next item in the local stack

        if (currentCall.rowIndex < box.minRow)
          box.minRow = currentCall.rowIndex;
        else if (currentCall.rowIndex > box.maxRow)
          box.maxRow = currentCall.rowIndex;

        // Go through all runs on this row and find the overlapping ones
        for (RunNode* pNode = info.lstRuns[currentCall.rowIndex].first; pNode != 0; )
          {
            // No overlap
            if (currentCall.start > pNode->end ||
                currentCall.end < pNode->start)
              {
                pNode = pNode->next;
                continue; // to the next node
              }

            // Invalidate the current run to prevent loops in "recursion"
            const int iEnd = pNode->end;
            pNode->end = -1;
            pNode->seed = false;
            const int iStart = pNode->start;
            if (iStart < box.minCol)
              box.minCol = iStart;
            if (iEnd > box.maxCol)
              box.maxCol = iEnd;
            // Mark the run and push "recursion" calls to the stack
            iSize += markToBuffer(info, currentCall.rowIndex, iStart, iEnd);
            RecursiveCall call;
            call.rowIndex = currentCall.rowIndex - 1;
            call.start = iStart;
            call.end = iEnd;
            localStack.push(call);
            call.rowIndex = currentCall.rowIndex + 1;
            localStack.push(call);

            // Destroy current node
            info.lstRuns[currentCall.rowIndex].remove(pNode);
            RunNode* pNodeToDelete = pNode;
            pNode = pNode->next;
            delete pNodeToDelete;
          }
      }
    return iSize;
  }
}
