/*
 *  cOrgSourceMessage.h
 *  Avida
 *
 *  Created by David on 3/26/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cOrgSourceMessage_h
#define cOrgSourceMessage_h

class cOrgSourceMessage
{
private:
  int m_value;
  int m_sent;
  int m_dropped;
  int m_corrupt;
  
public:
  cOrgSourceMessage() : m_value(0), m_sent(0), m_dropped(0), m_corrupt(0) { ; }
  cOrgSourceMessage(int value) : m_value(value), m_sent(1), m_dropped(0), m_corrupt(0) { ; }

  int GetValue() { return m_value; }
  
  int GetSent() { return m_sent; }
  void SetSent() { m_sent++; }  
  int GetDropped() { return m_dropped; }
  void SetDropped() { m_dropped++; }
  int GetCorrupted() { return m_corrupt; }
  void SetCorrupted() { m_corrupt++; }
};

#endif
