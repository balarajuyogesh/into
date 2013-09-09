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

#include "PiiFrequencyLimiter.h"

PiiFrequencyLimiter::PiiFrequencyLimiter(double maxFrequency)
{
  setMaxFrequency(maxFrequency);
  _filterTime.start();
}

bool PiiFrequencyLimiter::check()
{
  bool bUpdateObject = true;
  if (_dFilterInterval > 0)
    {
      int elapsed = _filterTime.elapsed();
      _dMeanInterval = 0.05 * elapsed + 0.95 * _dMeanInterval;
      
      if (_dMeanInterval > _dMaxMeanInterval)
        _dMeanInterval = _dMaxMeanInterval;
      
      if ( (double)_dFilterInterval > _dMeanInterval )
        bUpdateObject = false;
    }

  if ( bUpdateObject )
    _filterTime.restart();

  return bUpdateObject;
}

void PiiFrequencyLimiter::setMaxFrequency(double frequency)
{
  _dMaxFrequency = frequency;
  if (frequency == 0)
    {
      _dFilterInterval = 0;
      _dMaxMeanInterval = 0;
    }
  else
    {
      _dFilterInterval = (int)(1.0/frequency*1000.0);
      _dMaxMeanInterval = _dFilterInterval + frequency*0.05;
      _dMeanInterval = _dMaxMeanInterval;
    }
}

