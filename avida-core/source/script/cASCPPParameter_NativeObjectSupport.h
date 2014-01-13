/*
 *  cASCPPParameter_NativeObjectSupport.h
 *  Avida
 *
 *  Created by David on 10/13/08.
 *  Copyright 2008-2011 Michigan State University. All rights reserved.
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
 */

#ifndef cASCPPParameter_NativeObjectSupport_h
#define cASCPPParameter_NativeObjectSupport_h

#include "cASCPPParameter.h"
#include "cASNativeObject.h"


template<typename T> inline T cASCPPParameter::Get() const
{
  if (strcmp(m_nobj->GetType(), typeid(T).name()) != 0)
    exit(AS_EXIT_INTERNAL_ERROR); // @AS_TODO - report type mismatch error
  return (T)m_nobj->GetObject();
}

#endif
