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

#ifndef _PIISTRINGCONVERTER_H
#define _PIISTRINGCONVERTER_H

#include <PiiDefaultOperation.h>

/**
 * Converts strings to numbers.
 *
 * @inputs
 *
 * @in input - a string to be converted (QString)
 *
 * @outputs
 *
 * @out output - converted value. Either a @p double or an @p int.
 *
 * @ingroup PiiBasePlugin
 */
class PiiStringConverter : public PiiDefaultOperation
{
  Q_OBJECT

  /**
   * Conversion mode. This property controls the type of the output.
   */
  Q_PROPERTY(ConversionMode conversionMode READ conversionMode WRITE setConversionMode);
  Q_ENUMS(ConversionMode);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  /**
   * Conversion modes.
   *
   * @lip ConvertToInt - the input string will be converted to an @p
   * int.
   * @lip ConvertToDouble - the input string will be converted to a @p
   * double.
   */
  enum ConversionMode
  {
    ConvertToInt,
    ConvertToDouble
  };
  
  PiiStringConverter();

  void setConversionMode(ConversionMode conversionMode);
  ConversionMode conversionMode() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    ConversionMode conversionMode;
  };
  PII_D_FUNC;
};

#endif //_PIISTRINGCONVERTER_H
