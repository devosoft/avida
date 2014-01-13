/*
 *  cAnalyzeJob.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeJob_h
#define cAnalyzeJob_h

class cAvidaContext;

class cAnalyzeJob
{
private:
  int m_id;
  
public:
  cAnalyzeJob() : m_id(0) { ; }
  virtual ~cAnalyzeJob() { ; }
  
  void SetID(int newid) { m_id = newid; }
  int GetID() { return m_id; }
  
  virtual void Run(cAvidaContext& ctx) = 0;
};

#endif
