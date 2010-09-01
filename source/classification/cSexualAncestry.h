/*
 *  cSexualAncestry.h
 *  Avida
 *
 *  Created by David on 7/7/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cSexualAncestry_h
#define cSexualAncestry_h

#include <cassert>

class cBioGroup;


class cSexualAncestry
{
private:
  int m_id;
  int m_ancestor_ids[6];
  
  
  cSexualAncestry(); // @not_implemented
  cSexualAncestry(const cSexualAncestry&); // @not_implemented
  cSexualAncestry& operator=(const cSexualAncestry&); // @not_implemented

  
public:
  cSexualAncestry(cBioGroup* bg);
  
  int GetID() const { return m_id; }
  int GetAncestorID(int idx) const { assert(idx < 6); return m_ancestor_ids[idx]; }
  
  int GetPhyloDistance(cBioGroup* bg) const;
};

#endif
