/*
 *  cOrgSeqMessage.h
 *  Avida
 *
 *  Created by David on 3/28/06.
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

#ifndef cOrgSeqMessage_h
#define cOrgSeqMessage_h

class cOrgSeqMessage
{
private:
  int m_value;
  bool m_recvd;
  
public:
  cOrgSeqMessage() : m_value(0), m_recvd(false) { ; }

  void SetValue(int value) { m_value = value; }
  int GetValue() { return m_value; }

  void SetReceived() { m_recvd = true; }
  bool GetReceived() { return m_recvd; }
};

#endif
