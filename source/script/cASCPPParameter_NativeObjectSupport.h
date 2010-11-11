/*
 *  cASCPPParameter_NativeObjectSupport.h
 *  Avida
 *
 *  Created by David on 10/13/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
