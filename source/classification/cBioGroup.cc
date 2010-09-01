/*
 *  cBioGroup.cc
 *  Avida
 *
 *  Created by David on 10/7/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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
