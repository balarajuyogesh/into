/* This file is part of Into.
 * Copyright (C) Intopii
 * All rights reserved.
 */

#ifndef _PIIPYLONDRIVER_H
#define _PIIPYLONDRIVER_H

#include <PiiPylonDriverGlobal.h>
#include <PiiGenicamDriver.h>

class PII_PYLONDRIVER_EXPORT PiiPylonDriver : public PiiGenicamDriver
{
  Q_OBJECT

  PII_PROPERTY_SERIALIZATION_FUNCTION(PiiGenicamDriver)
public:
  PiiPylonDriver();
};

#endif //_PIIPYLONDRIVER_H
