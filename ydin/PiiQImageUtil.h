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

#ifndef _PIIQIMAGEUTIL_H
#define _PIIQIMAGEUTIL_H

#include <PiiColor.h>
#include <PiiQImage.h>
#include "PiiVariant.h"

namespace PiiYdin
{
  template <class T> QImage* createColorQImage(const PiiVariant& obj)
  {
    return PiiColorQImage::create(obj.valueAs<PiiMatrix<T> >());
  }
  
  template <class T> QImage* createGrayQImage(const PiiVariant& obj)
  {
    return PiiGrayQImage::create(obj.valueAs<PiiMatrix<T> >());
  }

  template <class T> inline QImage* createQImage(const PiiVariant& obj)
  {
    return Pii::createQImage(obj.valueAs<PiiMatrix<T> >());
  }
}

#endif //_PIIQIMAGEUTIL_H
