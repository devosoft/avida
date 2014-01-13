/*
 *  core/Feedback.h
 *  avida-core
 *
 *  Created by David on 3/23/11.
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

#ifndef AvidaCoreFeedback_h
#define AvidaCoreFeedback_h

#include "apto/platform.h"


namespace Avida {

  // Feedback - protocol used to provide configuration and execution feedback to the user
  // --------------------------------------------------------------------------------------------------------------
  
  class Feedback
  {
  public:
    LIB_EXPORT virtual ~Feedback() { ; }
    
    LIB_EXPORT virtual void Error(const char* fmt, ...) = 0;
    LIB_EXPORT virtual void Warning(const char* fmt, ...) = 0;
    LIB_EXPORT virtual void Notify(const char* fmt, ...) = 0;
  };
};

#endif
