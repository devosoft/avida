/*
 *  cGenotypeData.h
 *  Avida
 *
 *  Created by David Bryson on 1/10/09.
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

#ifndef cGenotypeData_h
#define cGenotypeData_h

class cGenotypeData
{
public:
  cGenotypeData() { ; }
  virtual ~cGenotypeData() = 0;
};

enum eGENOTYPE_DATA_IDS
{
  GD_MD_ID = 1,
  
  GD_DYNAMIC_ID_BASE
};

#endif
