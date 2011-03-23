/*
 *  cOrgSinkMessage.h
 *  Avida
 *
 *  Created by David on 3/26/06.
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

#ifndef cOrgSinkMessage_h
#define cOrgSinkMessage_h

class cOrgSinkMessage
{
private:
  int m_source_id;
  int m_orig_val;
  int m_actual_val;
  bool m_validated;
  
public:
  cOrgSinkMessage() : m_source_id(-1), m_orig_val(0), m_actual_val(0), m_validated(false) { ; }
  cOrgSinkMessage(int source, int original, int actual)
    : m_source_id(source), m_orig_val(original), m_actual_val(actual), m_validated(false) { ; }
  
  int GetSourceID() { return m_source_id; }
  int GetOriginalValue() { return m_orig_val; }
  int GetActualValue() { return m_actual_val; }
  
  void SetValidated() { m_validated = true; }
  bool GetValidated() { return m_validated; }
};

#endif
