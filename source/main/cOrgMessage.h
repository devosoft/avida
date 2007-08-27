/*
 *  cOrgMessage.h
 *  Avida
 *
 *  Called "org_message.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
#ifndef cOrgMessage_h
#define cOrgMessage_h

class cOrganism;


/*! This class encapsulates two unsigned integers that are sent as a "message"
between connected organisms within an Avida population.  The label and data fields
are these two integers, while the sending and receiving organisms are represented by
pointers.

\todo Extend to support a varying number of bytes.
*/
class cOrgMessage
{
public:
  //! Constructor that takes a pointer to the sending organism.
  cOrgMessage(cOrganism* sender) : m_pSender(sender), m_pReceiver(0), m_data(0), m_label(0) 
  {
    assert(m_pSender);
  }
  
  cOrganism* GetSender() const { return m_pSender; }
  cOrganism* GetReceiver() const { return m_pReceiver; }
  void SetReceiver(cOrganism* recvr) { m_pReceiver = recvr; }
  
  unsigned int GetData() const { return m_data; }
  unsigned int GetLabel() const { return m_label; }
  
  void SetData(unsigned int data) { m_data = data; }
  void SetLabel(unsigned int label) { m_label = label; }

private:
  //! Default constructor is only used internally, to support message predicates.
  cOrgMessage() : m_pSender(0), m_pReceiver(0), m_data(0), m_label(0)
  {
  }
  
  cOrganism* m_pSender;
  cOrganism* m_pReceiver;
  unsigned int m_data;
  unsigned int m_label;
};


#endif
