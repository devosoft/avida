/*
 *  cOrgSeqMessage.h
 *  Avida
 *
 *  Created by David on 3/28/06.
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
