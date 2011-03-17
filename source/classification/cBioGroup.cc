/*
 *  cBioGroup.cc
 *  Avida
 *
 *  Created by David on 10/7/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cBioGroup.h"

#include <cassert>


cBioGroup::~cBioGroup()
{
  assert(m_a_refs == 0 && m_p_refs == 0);
  
  for (tArrayMap<cString, cBioGroupData*>::iterator t = m_data.begin(); t != m_data.end(); t++) {
    cBioGroupData* data = t->Value();
    delete data;
  }
}

cBioGroupData::~cBioGroupData() { ; }
