/*
 *  tIterator.h
 *  Avida
 *
 *  Created by David on 7/14/10.
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

#ifndef tIterator_h
#define tIterator_h

template <class T> class tIterator
{
public:
  tIterator() { ; }
  virtual ~tIterator() { ; }
  
  virtual T* Get() = 0;
  virtual T* Next() = 0;
};


#endif
