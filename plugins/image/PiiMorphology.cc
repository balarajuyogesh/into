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

#include "PiiMorphology.h"
#include <cmath>

namespace PiiImage
{
  PiiMatrix<int> borderMasks[8][2] =
    {
      {
        // Border detector no 0
        PiiMatrix<int>(3,3,
                       0,0,0,
                       0,1,0,
                       1,1,1),
        // The corresponding significance mask
        PiiMatrix<int>(3,3,
                       1,1,1,
                       0,1,0,
                       1,1,1)
        
      },
      {
        // Corner detector no 0
        PiiMatrix<int>(3,3,
                       0,0,0,
                       1,1,0,
                       1,1,0),
        // The corresponding significance mask
        PiiMatrix<int>(3,3,
                       0,1,1,
                       1,1,1,
                       0,1,0)
      },
      {
        // Border detector no 1
        PiiMatrix<int>(3,3,
                       1,0,0,
                       1,1,0,
                       1,0,0),
        // The corresponding significance mask
        PiiMatrix<int>(3,3,
                       1,0,1,
                       1,1,1,
                       1,0,1)
        
      },
      {
        // Corner detector no 1
        PiiMatrix<int>(3,3,
                       1,1,0,
                       1,1,0,
                       0,0,0),
        // The corresponding significance mask
        PiiMatrix<int>(3,3,
                       0,1,0,
                       1,1,1,
                       0,1,1)
      },
      {
        // Border detector 2
        PiiMatrix<int>(3,3,
                       1,1,1,
                       0,1,0,
                       0,0,0),
        PiiMatrix<int>(3,3,
                       1,1,1,
                       0,1,0,
                       1,1,1)
        
      },
      {
        // Corner detector 2
        PiiMatrix<int>(3,3,
                       0,1,1,
                       0,1,1,
                       0,0,0),
        PiiMatrix<int>(3,3,
                       0,1,0,
                       1,1,1,
                       1,1,0)
      },
      {
        // Border detector 3
        PiiMatrix<int>(3,3,
                       0,0,1,
                       0,1,1,
                       0,0,1),
        PiiMatrix<int>(3,3,
                       1,0,1,
                       1,1,1,
                       1,0,1)
        
      },
      {
        // Corner detector 2
        PiiMatrix<int>(3,3,
                       0,0,0,
                       0,1,1,
                       0,1,1),
        PiiMatrix<int>(3,3,
                       1,1,0,
                       1,1,1,
                       0,1,0)
      }          
    };

  PiiMatrix<int> createMask(MaskType type, int rows, int columns)
  {
    return createMask<int>(type,rows,columns);
  }

  // Export an explicit instantiation.
  PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(PiiMatrix<unsigned char>, createMask<unsigned char>, (MaskType type, int rows, int columns));
}
