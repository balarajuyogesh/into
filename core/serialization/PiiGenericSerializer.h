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

#ifndef _PIIGENERICSERIALIZER_H
#define _PIIGENERICSERIALIZER_H

/**
 * An serialization interface for serializing classes that are not
 * known to the application at compile time.
 */
class PiiGenericSerializer
{
public:
  virtual void serialize(PiiGenericOutputArchive& archive, void* value, const unsigned int version) = 0;
  virtual void serialize(PiiGenericInputArchive& archive, void* value, const unsigned int version) = 0;
};

template <class T> class PiiDefaultGenericSerializer
{
  virtual void serialize(PiiGenericOutputArchive& archive, void* value, const unsigned int version)
  {
    PiiSerialization::Accessor::save(archive, value, version);
  }

  virtual void serialize(PiiGenericInputArchive& archive, void*& value, const unsigned int version)
  {
    PiiSerialization::Accessor::load(archive, value, version);
  }
  
  static PiiDefaultGenericSerializer instance;
};

template <class T> class PiiDefaultGenericSerializer<T> PiiDefaultGenericSerializer<T>::instance;

#endif //_PIIGENERICSERIALIZER_H
