/*
 *  core/Context.h
 *  avida-core
 *
 *  Created by David on 6/29/11.
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

#ifndef AvidaCoreContext_h
#define AvidaCoreContext_h

#include "avida/core/Types.h"

#include <cassert>


namespace Avida {
  
  // Context - current execution context
  // --------------------------------------------------------------------------------------------------------------
  
  class Context
  {
  private:
    WorldDriver* m_driver;
    Apto::Random* m_rng;
    
  public:
    inline Context(WorldDriver* driver, Apto::Random* rng) : m_driver(driver), m_rng(rng) { assert(driver); }
    
    LIB_EXPORT inline WorldDriver& Driver() { return *m_driver; }
    LIB_EXPORT inline Apto::Random& Random() { return *m_rng; }
    
    LIB_EXPORT inline void SetRandom(Apto::Random* rng) { m_rng = rng; }
  };
  
};

#endif
