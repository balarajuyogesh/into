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

#ifndef _PIIFIBERBUNDLEGENERATOR_H
#define _PIIFIBERBUNDLEGENERATOR_H

#include "PiiTextureGenerator.h"

/**
 * A texture generator that produces a texture composed of bundles of
 * infinitely long fibers (such as nylon) seen in transmitted light
 * (back-lighting).
 *
 */
class PII_CAMERAEMULATOR_EXPORT PiiFiberBundleGenerator : public PiiTextureGenerator
{
  Q_OBJECT

  /**
   * The width of an individual fiber bundle or "band", in pixels. The
   * default value is 200.
   */
  Q_PROPERTY(int bundleWidth READ getBundleWidth WRITE setBundleWidth);

  /**
   * The total number of fiber bundles. The default value is 8.
   */
  Q_PROPERTY(int bundleCount READ getBundleCount WRITE setBundleCount);

  /**
   * The maximum amount of pixels a fiber bundle can move horizontally
   * between two scan lines. The default value is 0.05. The smaller
   * the value the straighter the fibers.
   */
  Q_PROPERTY(double elasticity READ getElasticity WRITE setElasticity);

  /**
   * The maximum number of pixels a fiber bundle can deviate from its
   * original position either left or right. The default value is 15.
   */
  Q_PROPERTY(int maxMovement READ getMaxMovement WRITE setMaxMovement);

  /**
   * The maximum allowed change in fiber thickness between two scan
   * lines. The default value is 0.3 (gray levels).
   */
  Q_PROPERTY(double thicknessFluctuation READ getThicknessFluctuation WRITE setThicknessFluctuation);

  /**
   * The minimum thickness of a fiber bundle. The default value is 60.
   */
  Q_PROPERTY(int minThickness READ getMinThickness WRITE setMinThickness);

  /**
   * The maximum thickness of a fiber bundle. The default value is 90.
   */
  Q_PROPERTY(int maxThickness READ getMaxThickness WRITE setMaxThickness);

  /**
   * Smoothing flag. If set to `true` (the default), the generated
   * texture will be run through a low-pass filter.
   */
  Q_PROPERTY(bool smooth READ getSmooth WRITE setSmooth);

public:
  PiiFiberBundleGenerator();
  ~PiiFiberBundleGenerator();

  void setBundleWidth(int bundleWidth) { _iBundleWidth = bundleWidth; }
  int getBundleWidth() const { return _iBundleWidth; }
  void setBundleCount(int bundleCount) { _iBundleCount = bundleCount; }
  int getBundleCount() const { return _iBundleCount; }
  void setElasticity(double elasticity) { _dElasticity = elasticity; }
  double getElasticity() const { return _dElasticity; }
  void setMaxMovement(int maxMovement) { _iMaxMovement = maxMovement; }
  int getMaxMovement() const { return _iMaxMovement; }
  void setMinThickness(int minThickness) { _iMinThickness = minThickness; }
  int getMinThickness() const { return _iMinThickness; }
  void setMaxThickness(int maxThickness) { _iMaxThickness = maxThickness; }
  int getMaxThickness() const { return _iMaxThickness; }
  void setThicknessFluctuation(double thicknessFluctuation) { _dThicknessFluctuation = thicknessFluctuation; }
  double getThicknessFluctuation() const { return _dThicknessFluctuation; }
  void setSmooth(bool smooth) { _bSmooth = smooth; }
  bool getSmooth() const { return _bSmooth; }

  void generateTexture(PiiMatrix<unsigned char>& buffer,
                       int row, int column,
                       int rows, int columns,
                       bool first = false);
private:
  class FiberBundle;

  void generateLine(unsigned char* line, int width);

  int _iBundleWidth;
  int _iBundleCount;
  double _dElasticity;
  int _iMaxMovement;
  double _dThicknessFluctuation;
  int _iMinThickness;
  int _iMaxThickness;
  bool _bSmooth;

  QList<FiberBundle*> _lstBundles;
};



#endif //_PIIFIBERBUNDLEGENERATOR_H
