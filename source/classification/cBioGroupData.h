/*
 *  cBioGroupData.h
 *  Avida
 *
 *  Created by David on 5/26/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef cBioGroupData_h
#define cBioGroupData_h

class cBioGroupData
{
public:
  virtual ~cBioGroupData() = 0;
};

template<typename T> class tBioGroupData : public cBioGroupData
{
private:
  T* m_data;
  
  tBioGroupData(); // @not_implemented
  tBioGroupData(const tBioGroupData&); // @not_implemented
  tBioGroupData& operator=(const tBioGroupData&); // @not_implemented
  
public:
  tBioGroupData(T* data) : m_data(data) { ; }
  ~tBioGroupData() { delete m_data; }
  
  T* GetData() { return m_data; }
};


#endif
