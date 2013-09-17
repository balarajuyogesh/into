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

#ifndef _PIIVISUALSOMCLASSIFIER_H
#define _PIIVISUALSOMCLASSIFIER_H

#include <PiiOperationCompound.h>

/**
 * A visually trainable SOM-based classifier.
 *
 * @children
 *
 * @child som - [PiiSomOperation<double>](PiiSomOperation).
 * Input features are connected to to this operation.
 *
 * @child trainer - PiiVisualTrainer. Use the trainer's 
 * `configurator` UI to visually assign labels to the SOM map.
 *
 *
 * Inputs
 * ------
 *
 * @in features - feature vectors to be classified. An alias for
 * som.features.
 *
 * @in boundaries - boundaries of features if multi-feature vectors
 * are used. Optional. An alias for som.boundaries.
 *
 * @in image - a large image to be shown on the visual trainer. An
 * alias for trainer.image.
 *
 * @in subimage - a sub-image to be shown on the trainer's map. An
 * alias for trainer.subimage.
 *
 * @in location - the location of the sub-image within `image`. An
 * alias for trainer.location.
 *
 * @in filename - the name of the image. An alias for
 * trainer.filename (see PiiVisualTrainer).
 *
 * Outputs
 * -------
 *
 * @out labelX - classification results. Aliases for trainer.labelX.
 *
 * To set the properties of the internal operations do this:
 *
 * ~~~
 * PiiOperation* vs = engine.createOperation("PiiVisualSomClassifier");
 * vs->setProperty("som.iterationNumber", 0);
 * vs->setProperty("trainer.bufferSize", 100);
 * ~~~
 *
 */
class PiiVisualSomClassifier : public PiiOperationCompound
{
  Q_OBJECT

  /**
   * Reflects the `layers` property of the internal PiiVisualTrainer.
   */
  Q_PROPERTY(QVariantList layers READ layers WRITE setLayers STORED false);

  /**
   * Reflects the `size` property of the internal PiiSomOperation and
   * the `gridSize` property of the internal PiiVisualTrainer. 
   * Setting the map size automatically changes the learning
   * parameters of SOM using some rules of thumb.
   */
  Q_PROPERTY(QSize mapSize READ mapSize WRITE setMapSize STORED false);

  /**
   * Select controlled/uncontrolled training to the som.
   */
  Q_PROPERTY(bool controlledTraining READ controlledTraining WRITE setControlledTraining STORED false);

  
  friend struct PiiSerialization::Accessor;
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;
  PiiVisualSomClassifier(PiiSerialization::Void);

  template <class Archive> void serialize(Archive& archive, const unsigned int /*version*/)
  {
    PII_D;
    PII_SERIALIZE_BASE(archive, PiiOperationCompound);
    PiiSerialization::serializeProperties(archive, *this);
    archive & PII_NVP("som", d->pSom);
    archive & PII_NVP("trainer", d->pTrainer);
    if (Archive::InputArchive)
      connectSignals();
  }

public:
  /**
   * Create a new visual classifier.
   */
  PiiVisualSomClassifier();
  
  void setMapSize(QSize mapSize);
  QSize mapSize() const;
  /**
   * Updates the visual trainer and exposes all of its outputs.
   */
  void setLayers(const QVariantList& layers);
  QVariantList layers() const;

  void setControlledTraining(bool controlledTraining);
  bool controlledTraining() const;

  /**
   * Create a clone of this compound. 
   *
   * @return a deep copy of the operation-compound.
   *
   */
  PiiVisualSomClassifier* clone() const;

private:
  void initTrainer();
  /**
   * This function initialize som. If the controlledTraining is true,
   * the som is PiiVisualSomOperation, and if false then the som is 
   * PiiSomOperation<double>.
   */
  void initSom(bool controlledTraining);
  void createOutputs();
  void connectSignals();
  
  /// @internal
  class Data : public PiiOperationCompound::Data
  {
  public:
    Data();
    PiiOperation* pSom, *pTrainer;
  };
  PII_D_FUNC;
};

#endif //_PIIVISUALSOMCLASSIFIER_H
