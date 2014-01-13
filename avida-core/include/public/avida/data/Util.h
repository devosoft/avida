/*
 *  data/Util.h
 *  avida-core
 *
 *  Created by David on 10/21/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaDataUtil_h
#define AvidaDataUtil_h

#include "apto/core.h"
#include "avida/core/Types.h"

namespace Avida {
  namespace Data {
    
    inline bool IsStandardID(const DataID& data_id)
    {
      return (data_id.GetSize() && (data_id.GetSize() < 3 || data_id[data_id.GetSize() - 1] != ']'));
    }
    
    inline bool IsArgumentedID(const DataID& data_id)
    {
      return (data_id.GetSize() > 2 && data_id[data_id.GetSize() - 1] == ']');
    }

  };
};

#endif
