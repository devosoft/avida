/*
 *  core/Avida.h
 *  avida-core
 *
 *  Created by David on 7/10/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#ifndef AvidaCoreAvida_h
#define AvidaCoreAvida_h

#include "avida/core/Types.h"


namespace Avida {
  LIB_EXPORT void Initialize();
  
  
  LIB_EXPORT inline double CalcFitness(double metabolic_rate, double gestation_time)
  {
    assert(metabolic_rate >= 0.0);
    assert(gestation_time >= 0.0);
    
    return (gestation_time != 0.0 ) ? (metabolic_rate / gestation_time) : 0.0;
  }
};


#endif
