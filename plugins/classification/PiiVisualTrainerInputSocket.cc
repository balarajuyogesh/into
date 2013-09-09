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

#include "PiiVisualTrainerInputSocket.h"
#include <QtAlgorithms>

PiiVisualTrainerInputSocket::PiiVisualTrainerInputSocket(const QString& name, LayerType layerType) :
  PiiInputSocket(name),
  _layerType(layerType), _gridSize(10,10), _iValueCnt(0), _iCellCount(100)
{
  resetValues();
}

void PiiVisualTrainerInputSocket::storeValue(int index, double value)
{
  if (index < 0 || index >= _iCellCount)
    return;
  
  _iValueCnt++;

  switch (_layerType)
    {
    case ClassLayer:
      // Build a histogram of different values
      {
        int label = int(value);
        while (label >= _lstLabels[index].size())
          _lstLabels[index] << 0;
        _lstLabels[index][label]++;
        //qDebug("_lstLabels[%i][%i] = %i", index, label, _lstLabels[index][label]);
      }
      break;
    case FloatLayer:
      // Calculate average value for the label iteratively
      {
        double mu = 1.0 / _iValueCnt;
        _lstAverages[index] = _lstAverages[index] * (1.0 - mu) + value * mu;
      }
      break;
    }
}

void PiiVisualTrainerInputSocket::resetValues()
{
  _iCellCount = _gridSize.width() * _gridSize.height();
  _lstLabels.clear();
  while (_lstLabels.size() < _iCellCount)
    _lstLabels << QList<int>();
  _lstAverages.resize(_iCellCount);
  qFill(_lstAverages.begin(), _lstAverages.end(), 0.0);
  _iValueCnt = 0;
}

void PiiVisualTrainerInputSocket::setLayerType(LayerType layerType)
{
  _layerType = layerType;
  resetValues();
}

void PiiVisualTrainerInputSocket::setGridSize(QSize gridSize)
{
  _gridSize = gridSize;
  resetValues();
}

double PiiVisualTrainerInputSocket::label(int index)
{
  switch (_layerType)
    {
    case ClassLayer:
      {
        // Find the label with the maximum number of votes.
        int maximum = 0, maxIndex = 0;
        QList<int>& labels = _lstLabels[index];
        for (int i=labels.size(); i--; )
          {
            if (labels[i] > maximum)
              {
                maximum = labels[i];
                maxIndex = i;
              }
          }
        if (maximum == 0)
          return -1;
        return maxIndex;
      }
    case FloatLayer:
      // Return average label
      return _lstAverages[index];
    }
  return 0;
}
