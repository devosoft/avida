/*
 *  cBioUnit.h
 *  Avida
 *
 *  Created by David on 10/7/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#ifndef cBioUnit_h
#define cBioUnit_h

class cMetaGenome;

class cBioUnit
{
public:
  cBioUnit() { ; }
  virtual ~cBioUnit() = 0;
  
  virtual cMetaGenome& GetMetaGenome() = 0;
};

#endif
