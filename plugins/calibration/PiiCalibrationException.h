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

#ifndef _PIICALIBRATIONEXCEPTION_H
#define _PIICALIBRATIONEXCEPTION_H

#include <PiiException.h>
#include "PiiCalibrationGlobal.h"

/**
 * PiiCalibrationException is thrown when errors occur in camera
 * calibration.
 */
class PII_CALIBRATION_EXPORT PiiCalibrationException : public PiiException
{
public:
  /**
   * Construct a new PiiCalibrationException.
   */
  PiiCalibrationException(const QString& message = "", const QString& location = "") : PiiException(message, location) {}
};


#endif //_PIICALIBRATIONEXCEPTION_H
