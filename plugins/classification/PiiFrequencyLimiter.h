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

#ifndef _PIIFREQUENCYLIMITER_H
#define _PIIFREQUENCYLIMITER_H

#include <QTime>

/**
 * This is the class for the limit input frequency to the wanted
 * frequency. Notice that the wanted frequency is the max frequency,
 * so the real output frequency is a little bit smaller than wanted max
 * frequency.
 *
 * @ingroup PiiSmartClassifierOperation.
 */

class PiiFrequencyLimiter
{
  
public:
  PiiFrequencyLimiter(double maxFrequency = 10);

  /**
   * This function calculate mean frequence and check if it is smaller
   * than max frequency.
   *
   */
  bool check();

  /**
   * Set and get the max frequency.
   */
  void setMaxFrequency(double maxFrequency);
  double maxFrequency() { return _dMaxFrequency; }
  
private:
  double _dMaxFrequency,_dMaxMeanInterval,_dFilterInterval,_dMeanInterval;
  QTime _filterTime;

};

#endif //_PIIFREQUENCYLIMITER_H
