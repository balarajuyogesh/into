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

#ifndef _PIIVISUALTRAINERINPUTSOCKET_H
#define _PIIVISUALTRAINERINPUTSOCKET_H

#include <PiiInputSocket.h>
#include <QVector>
#include <QList>
#include <QSize>

/**
 * A special input socket that stores incoming labels. This class also
 * gives a suggestion for a class label if one is requested.
 *
 */
class PiiVisualTrainerInputSocket : public PiiInputSocket
{
  Q_OBJECT

public:
  enum LayerType { ClassLayer, FloatLayer };
  PiiVisualTrainerInputSocket(const QString& name, LayerType type = ClassLayer);

  void storeValue(int index, double value);
  void resetValues();
  double label(int index);

  void setLayerType(LayerType layerType);
  LayerType layerType() const { return _layerType; }
  void setGridSize(QSize gridSize);
  QSize gridSize() const { return _gridSize; }
  
private:
  LayerType _layerType;
  QList<QList<int> > _lstLabels;
  QVector<double> _lstAverages;
  QSize _gridSize;
  int _iValueCnt;
  int _iCellCount;
};

#endif //_PIIVISUALTRAINERINPUTSOCKET_H
