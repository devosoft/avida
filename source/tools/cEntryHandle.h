/*
 *  cEntryHandle.h
 *  Avida
 *
 *  Created by David on 9/27/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cEntryHandle_h
#define cEntryHandle_h

class cEntryHandle
{
public:
  virtual ~cEntryHandle() = 0;
  
  virtual bool IsValid() const = 0;
  virtual void Remove() = 0;
};

#endif
