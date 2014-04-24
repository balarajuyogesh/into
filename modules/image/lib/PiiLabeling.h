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

#ifndef _PIILABELING_H
#define _PIILABELING_H

#include "PiiImageGlobal.h"
#include <PiiMatrix.h>
#include <QVector>
#include <QPair>
#include <functional>

namespace PiiImage
{
  /**
   * A class that collects object labels as the [labelImage()] function
   * processes the input image. This is the default implementation
   * which does nothing.
   *
   * @see ObjectSizeLimiter
   */
  class DefaultLabelingLimiter
  {
  public:
    inline void setInitialLabels(const QVector<int>&) {}
    inline void addLabel() {}
    inline void addPixel(int) {}
    inline void limitLabels(QVector<int>&) {}
  };

  /**
   * An object size limiter for [labelImage()]. This class counts the
   * occurrences of each label and removes all objects smaller than
   * *minimumSize*. If *minimumSize* is -N, retains the N largest
   * objects.
   */
  class ObjectSizeLimiter
  {
  public:
    /**
     * Creates a size limiter that only accepts objects whose size is
     * at least `minimumSize`. Negative value retains N biggest
     * objects.
     */
    inline ObjectSizeLimiter(int minimumSize) : _iMinimumSize(minimumSize) {}
    /**
     * Called by labelImage() to store the intial set of labels. The
     * labels are just cloned and stored.
     */
    inline void setInitialLabels(const QVector<int>& labels)
    {
      _vecCounts = labels;
    }
    /**
     * Extends the list of stored labels by one.
     */
    inline void addLabel()
    {
      _vecCounts.append(0);
    }
    /**
     * The labelImage() function found a pixel it labeled as `label`.
     * Increases the label's histogram.
     */
    inline void addPixel(int label)
    {
      ++_vecCounts[label];
    }
    /**
     * Retains all labels with more than `minimumSize` histogram
     * entries. Sets all other labels to zero. The input vector labels
     * maps labels to final label indices.
     */
    inline void limitLabels(QVector<int>& labels)
    {
      const int iLabelCnt = labels.size();
      if (_iMinimumSize == 0)
        return;
      else if (_iMinimumSize > 0)
        {
          QVector<int> vecTotals(iLabelCnt);
          for (int l=0; l<iLabelCnt; ++l)
            vecTotals[labels[l]] += _vecCounts[l];

          // Zero out all labels whose total count is smaller than
          // threshold.
          for (int l=0; l<iLabelCnt; ++l)
            if (vecTotals[labels[l]] < _iMinimumSize)
              labels[l] = 0;
        }
      else if (_iMinimumSize == -1)
        {
          QVector<int> vecTotals(iLabelCnt);
          for (int l=0; l<iLabelCnt; ++l)
            vecTotals[labels[l]] += _vecCounts[l];

          // Find the maximum pixel count
          int iMaxIndex = -1, iMaxCount = 0;
          for (int l=0; l<iLabelCnt; ++l)
            {
              if (vecTotals[l] > iMaxCount)
                {
                  iMaxCount = vecTotals[l];
                  iMaxIndex = l;
                }
            }
          // Zero out all labels that point to anything else than the
          // biggest one.
          for (int l=0; l<iLabelCnt; ++l)
            if (labels[l] != iMaxIndex)
              labels[l] = 0;
        }
      else if (-_iMinimumSize < iLabelCnt)
        {
          QVector<QPair<int,int> > vecTotals(iLabelCnt);
          // Store label indices (we are going to sort the array)
          for (int l=0; l<iLabelCnt; ++l)
            vecTotals[l].second = l;
          for (int l=0; l<iLabelCnt; ++l)
            vecTotals[labels[l]].first += _vecCounts[l];
          std::sort(vecTotals.begin(), vecTotals.end(), std::greater<QPair<int,int> >());
          int iRetainedCount = -_iMinimumSize;
          // Optimization: don't waste time in comparing to zeros in
          // the inner loop below.
          while (iRetainedCount && vecTotals[iRetainedCount-1].first == 0)
            --iRetainedCount;
          if (iRetainedCount)
            {
              // Zero out everything except the N biggest ones
              for (int l=0; l<iLabelCnt; ++l)
                {
                  for (int i=0; i<iRetainedCount; ++i)
                    if (vecTotals[i].second == labels[l]) // this is one of the big boys
                      goto next_l;
                  labels[l] = 0;
                next_l:;
                }
            }
        }
    }

  private:
    QVector<int> _vecCounts;
    int _iMinimumSize;
  };

  template <class Matrix, class UnaryOp, class Limiter>
  void labelImage(const Matrix& mat,
                  PiiMatrix<int>& labels,
                  UnaryOp rule,
                  Limiter limiter,
                  int* labelCount = 0);

  /**
   * Labels an image using 4-connectivity. This function uses the
   * two-pass algorithm found in most computer vision textbooks.
   *
   * @param mat a matrix to be labeled. All non-zero values are
   * treated as objects.
   *
   * @param rule find objects by applying this unary predicate. All
   * pixels to which `op(pixel)` returns true are treated as
   * object pixels.
   *
   * @param limiter a template class that selects which objects to
   * accept for labeling
   *
   * @return a labeled image, whose maximum value equals to
   * `labelCount`
   *
   * @param labelCount an optional output-value parameter that stores
   * the number of labels found
   *
   * ~~~(c++)
   * PiiMatrix<float> img;
   * int iLabelCount = 0;
   * // Every pixel with an intensity greater than 0.5 is an object.
   * PiiImage::labelImage(img,
   *                      std::bind2nd(std::greater<float>(), 0.5f),
   *                      PiiImage::DefaultLabelingLimiter(),
   *                      &iLabelCount));
   * ~~~
   *
   * @see DefaultLabelingLimiter
   * @see ObjectSizeLimiter
   */
  template <class Matrix, class UnaryOp, class Limiter>
  PiiMatrix<int> labelImage(const Matrix& mat,
                            UnaryOp rule,
                            Limiter limiter,
                            int* labelCount = 0)
  {
    PiiMatrix<int> matLabels(mat.rows(), mat.columns());
    labelImage(mat, matLabels, rule, limiter, labelCount);
    return matLabels;
  }

  /**
   * This version writes the labels to a preallocated output image
   * *labels*. The size of the output image must be the same as the
   * input, and it must be initialized to zeros.
   */
  template <class Matrix, class UnaryOp, class Limiter>
  void labelImage(const Matrix& mat,
                  PiiMatrix<int>& labels,
                  UnaryOp rule,
                  Limiter limiter,
                  int* labelCount = 0)
  {
    if (mat.isEmpty())
      {
        if (labelCount) *labelCount = 0;
        return;
      }

    const int iCols = mat.columns();
    const int iRows = mat.rows();
    int iLabelIndex = 0;

    QVector<int> vecLabels(1);
    vecLabels.reserve(64);
    limiter.setInitialLabels(vecLabels);


    int *pCurrent, *pUp, *pLeft;

    typename Matrix::const_row_iterator sourceRow = mat.rowBegin(0);
    pCurrent = labels[0];
    pLeft = pCurrent-1;

#define PII_CREATE_NEW_LABEL                          \
    do {                                              \
      vecLabels.append(pCurrent[c] = ++iLabelIndex);  \
      limiter.addLabel();                             \
    } while(false)


    int c = 0;
    // If first pixel is an object pixel, create a new label
    if (rule(*sourceRow))
      {
        PII_CREATE_NEW_LABEL;
        limiter.addPixel(pCurrent[c]);
      }
    ++sourceRow;
    // Handle the rest of the first row separately
    for (c=1; c<iCols; ++c, ++sourceRow)
      {
        if (rule(*sourceRow))
          {
            if (pLeft[c] != 0)
              pCurrent[c] = pLeft[c];
            else
              PII_CREATE_NEW_LABEL;
            limiter.addPixel(pCurrent[c]);
          }
      }
    // The rest of the matrix can be handled without vertical bounds
    // checking.
    for (int r=1; r<iRows; ++r)
      {
        sourceRow = mat.rowBegin(r);
        pUp = pCurrent;
        pCurrent = labels[r];
        pLeft = pCurrent-1;
        c = 0;
        // Handle first pixel
        if (rule(*sourceRow))
          {
            // Non-zero label above this one -> make this one similar
            if (pUp[c] != 0)
              pCurrent[c] = pUp[c];
            else
              PII_CREATE_NEW_LABEL;
            limiter.addPixel(pCurrent[c]);
          }
        ++sourceRow;
        for (c=1; c<iCols; ++c, ++sourceRow)
          {
            // If there is no object at this pixel, do nothing.
            if (!rule(*sourceRow))
              continue;

            int iLeft = pLeft[c];
            int iUp = pUp[c];

            // Most common case first -> both neighbors equal
            if (iLeft == iUp)
              {
                // Both are non-zero
                if (iLeft != 0)
                  pCurrent[c] = iLeft;
                // Both are zero -> corner
                else
                  PII_CREATE_NEW_LABEL;
              }
            // If either neighbor is zero ...
            else if (iLeft * iUp == 0)
              // ... the other neighbor determines our label
              pCurrent[c] = iLeft + iUp;
            // Neighbors are non-zero and different -> mark them equal
            else
              {
                // It doesn't matter which one we choose at this
                // phase. Let's take the one above us.
                Pii::fillIf(vecLabels.begin(), vecLabels.end(),
                            std::bind2nd(std::equal_to<int>(), vecLabels[iLeft]),
                            vecLabels[iUp]);
                pCurrent[c] = iUp;
                /*std::cout << r << "," << c << ": up = " << iUp << ", left = " << iLeft << ", labels = ";
                for (int i=0;i<vecLabels.size(); ++i)
                  std::cout << vecLabels[i];
                  std::cout << "\n";
                */
              }
            limiter.addPixel(pCurrent[c]);
          }
      }

    limiter.limitLabels(vecLabels);

    // If no labels were joined, labels are like
    // 0 1 2 3 4 5 6
    // If 3 neighbors 1 and 6 neighbors 5, then labels are
    // 0 1 2 1 4 5 5
    // Move labels so that objects are numbered sequentially (no
    // skipped labels). In the above case:
    // 0 1 2 1 3 4 4

    iLabelIndex = 0;
    for (int i=0; i<vecLabels.size(); ++i)
      {
        // If the step from last is over one, there is a gap.
        if (vecLabels[i] > iLabelIndex+1)
          {
            // Move all matching labels down to current iLabelIndex+1
            ++iLabelIndex;
            for (int j=i+1; j<vecLabels.size(); ++j)
              if (vecLabels[j] == vecLabels[i])
                vecLabels[j] = iLabelIndex;
            vecLabels[i] = iLabelIndex;
          }
        // No gap -> mark the maximum
        else if (vecLabels[i] > iLabelIndex)
          iLabelIndex = vecLabels[i];
      }

    //Finally, alter mapped labels
    for (int r=0; r<iRows; ++r)
      {
        int* pRow = labels[r];
        for (int c=0; c<iCols; ++c)
          pRow[c] = vecLabels[pRow[c]];
      }

    if (labelCount != 0)
      *labelCount = iLabelIndex;
  }

  /**
   * Labels an image using 4-connectivity.
   *
   * @param mat a matrix to be labeled. All non-zero values are
   * treated as objects.
   *
   * @param labelCount an optional output-value parameter that stores
   * the number of labels found
   *
   * @return a labeled image, whose maximum value equals to `labelCount`
   */
  template <class Matrix> PiiMatrix<int> labelImage(const Matrix& mat,
                                                    int* labelCount = 0)
  {
    typedef typename Matrix::value_type T;
    return labelImage(mat, std::bind2nd(std::not_equal_to<T>(), T(0)), DefaultLabelingLimiter(), labelCount);
  }

  /**
   * Labels all 4-connected objects whose size (in pixels) is larger
   * than `sizeLimit`.
   *
   * @param mat a matrix to be labeled. All non-zero values are
   * treated as objects.
   *
   * @param sizeLimit only label objects larger than this. Smaller
   * objects will be set to zero. If *sizeLimit* is zero, only the
   * largest object will be retained.
   *
   * @param labelCount an optional output-value parameter that stores
   * the number of labels found
   *
   * @return a labeled image, whose maximum value equals to `labelCount`
   */
  template <class Matrix> PiiMatrix<int> labelLargerThan(const Matrix& mat,
                                                         int sizeLimit, int* labelCount = 0)
  {
    typedef typename Matrix::value_type T;
    return labelImage(mat, std::bind2nd(std::not_equal_to<T>(), T(0)),
                      ObjectSizeLimiter(sizeLimit ? sizeLimit+1 : -1),
                      labelCount);
  }


  /// @hide

  // A linked list node for runs of consequtive object pixels on one
  // row.
  struct RunNode
  {
    RunNode(int firstColumn, bool s) :
      start(firstColumn), end(-1), seed(s),
      previous(0), next(0)
    {}

    RunNode(int firstColumn = -1, int lastColumn = -1) :
      start(firstColumn), end(lastColumn), seed(false),
      previous(0), next(0)
    {}

    int start, end;
    bool seed;
    RunNode* previous, *next;
  };

  // The entry point for a linked list of nodes.
  struct RunList
  {
    RunList() : first(0), last(0) {}

    ~RunList()
    {
      while (first != 0)
        delete take(first);
    }

    RunNode* take(RunNode* node)
    {
      remove(node);
      return node;
    }

    void remove(RunNode* node)
    {
      if (node->previous != 0)
        node->previous->next = node->next;
      else
        first = node->next;

      if (node->next != 0)
        node->next->previous = node->previous;
      else
        last = node->previous;
    }

    RunList& operator<< (RunNode* node)
    {
      if (last != 0)
        last->next = node;
      else
        first = node;

      node->previous = last;
      last = node;
      return *this;
    }

    int size() const
    {
      int iCount = 0;
      for (RunNode* ptr = first; ptr != 0; ptr = ptr->next, ++iCount) ;
      return iCount;
    }

    RunNode* first, *last;
  };

  // Keeps track of the current state of the labeling algorithm in
  // recursive calls.
  struct LabelInfo
  {
    LabelInfo(QVector<RunList>& runs, PiiMatrix<int>& labels, int& index, int connectivityShift) :
      lstRuns(runs), matLabels(labels), iLabelIndex(index), iConnectivityShift(connectivityShift)
    {}

    QVector<RunList>& lstRuns;
    PiiMatrix<int>& matLabels;
    int& iLabelIndex;
    int iConnectivityShift;
  };

  PII_IMAGE_EXPORT void connectRunsRecursively(LabelInfo& info, int rowIndex, int start, int end);
  PII_IMAGE_EXPORT void connectRuns(LabelInfo& info, int rowIndex, int start, int end);
  PII_IMAGE_EXPORT void markToBuffer(LabelInfo& info, int rowIndex, int start, int end);

  template <class Matrix, class UnaryOp1, class UnaryOp2>
  void labelImage(const Matrix& mat,
                  PiiMatrix<int>& labels,
                  UnaryOp1 rule1, UnaryOp2 rule2,
                  Connectivity connectivity,
                  int labelIncrement = 1,
                  int* labelCount = 0);
  /// @endhide

  /**
   * Labels connected components. This function uses a recursive
   * algorithm for finding connected components. It supports both
   * 8-connected and 4-connected components. This function performs
   * not only labeling but also hysteresis thresholding.
   *
   * @param mat the matrix to be labeled
   *
   * @param rule1 a unary predicate that determines if a pixel in
   * `mat` is an object pixel candidate. A pixel candidate is not
   * necessarily an object pixel unless at least one pixel in the
   * object matches `rule2`.
   *
   * @param rule2 a unary predicate that each connected component must
   * meet at least once. If a connected component only has pixels that
   * match `rule1`, it won't be labeled. If any of the pixels matches
   * `rule2`, the whole component will be labeled.
   *
   * @param connectivity the connectivity type
   *
   * @param labelIncrement increment the label counter this much every
   * time a new connected component is found. Set to zero to just mark
   * all found objects with ones.
   *
   * @param labelCount an optional output value parameter that stores
   * the maximum label. If `labelIncrement` is one, this value equals
   * to the number of objects labeled.
   *
   * ~~~(c++)
   * PiiMatrix<bool> binaryImg;
   * PiiMatrix<int> labels;
   * // Label all 8-connected non-zero pixels
   * labels = PiiImage::labelImage(binaryImg,
   *                               std::bind2nd(std::equal_to<bool>(), true),
   *                               std::bind2nd(std::equal_to<bool>(), true),
   *                               PiiImage::Connect8);
   *
   * // Hysteresis thresholding:
   * // Mark all 4-connected components whose gray levels are larger than
   * // 50 and there is at least one pixel whose gray level is above 100.
   * PiiMatrix<int> grayImg;
   * labels = PiiImage::labelImage(grayImg,
   *                               std::bind2nd(std::greater<int>(), 50),
   *                               std::bind2nd(std::greater<int>(), 100),
   *                               PiiImage::Connect4,
   *                               0);
   * ~~~
   *
   * @see hysteresisThreshold()
   */
  template <class Matrix, class UnaryOp1, class UnaryOp2>
  PiiMatrix<int> labelImage(const Matrix& mat,
                            UnaryOp1 rule1, UnaryOp2 rule2,
                            Connectivity connectivity,
                            int labelIncrement = 1,
                            int* labelCount = 0)
  {
    PiiMatrix<int> matLabels(mat.rows(), mat.columns());
    labelImage(mat, matLabels, rule1, rule2, connectivity, labelIncrement, labelCount);
    return matLabels;
  }

  /**
   * This version writes the labels to a preallocated output image
   * *labels*. The size of the output image must be the same as the
   * input, and it must be initialized to zeros.
   */
  template <class Matrix, class UnaryOp1, class UnaryOp2>
  void labelImage(const Matrix& mat,
                  PiiMatrix<int>& labels,
                  UnaryOp1 rule1, UnaryOp2 rule2,
                  Connectivity connectivity,
                  int labelIncrement = 1,
                  int* labelCount = 0)
  {
    QVector<RunList> lstRuns(mat.rows());
    int iLabelIndex = labelIncrement == 0 ? 1 : 0;
    int iConnectivityShift = connectivity == Connect8 ? 0 : 1;

    LabelInfo info(lstRuns, labels, iLabelIndex, iConnectivityShift);

    const int iRows = mat.rows(), iCols = mat.columns();

    for (int r=0; r<iRows; ++r)
      {
        typename Matrix::const_row_iterator sourceRow = mat.rowBegin(r);
        for (int c=0; c<iCols; ++c)
          {
            // A sequence of possible object pixels starts (left edge)
            if (rule1(sourceRow[c]))
              {
                RunNode *pRun = new RunNode(c, rule2(sourceRow[c]));
                // Find the right edge
                ++c;
                while (c < iCols && rule1(sourceRow[c]))
                  {
                    // If any of the pixels within the sequence meets
                    // the second rule, the sequence can
                    // work as a "seed" for hysteresis.
                    if (rule2(sourceRow[c])) pRun->seed = true;
                    ++c;
                  }
                // End index is one past the last pixel with
                // 8-connected objects and at the last pixel with
                // 4-connected ones.
                pRun->end = c - iConnectivityShift;
                // Add run to the list of runs on this row
                lstRuns[r] << pRun;
              }
          }
      }

    // Now we have a list of runs for each row. Connect.
    for (int rowIndex=0; rowIndex<lstRuns.size(); ++rowIndex)
      {
        // Go through all connected runs on this row
        for (RunNode* pNode = lstRuns[rowIndex].first; pNode != 0;)
          {
            // This run can work as a seed for a new label
            if (pNode->seed)
              {
                // Next label...
                iLabelIndex += labelIncrement;
                // Invalidate the node to prevent loops in recursion
                int end = pNode->end;
                pNode->end = -1;
                pNode->seed = false;
                // Mark and connect recursively
                markToBuffer(info, rowIndex, pNode->start, end);
                connectRuns(info, rowIndex - 1, pNode->start, end);
                connectRuns(info, rowIndex + 1, pNode->start, end);
                // This run has been handled now. Get rid of it.
                lstRuns[rowIndex].remove(pNode);
                RunNode* pNodeToDelete = pNode;
                pNode = pNode->next;
                delete pNodeToDelete;
              }
            else
              pNode = pNode->next;
          }
      }

    // Store return-value parameter if needed
    if (labelCount != 0)
      *labelCount = iLabelIndex;
  }
}

#endif //_PIILABELING_H
