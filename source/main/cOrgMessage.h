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
  //! Default constructor.
  cOrgMessage();
	
  //! Constructor that takes a pointer to the sending organism.
  cOrgMessage(cOrganism* sender, int messageType = 0);
  
  cOrganism* GetSender() const { return m_pSender; }
  cOrganism* GetReceiver() const { return m_pReceiver; }
  void SetReceiver(cOrganism* recvr);
  
  unsigned int GetData() const { return m_data; }
  unsigned int GetLabel() const { return m_label; }
  
  void SetData(unsigned int data) { m_data = data; }
  void SetLabel(unsigned int label) { m_label = label; }

  int GetSenderCellID() const { return m_senderCellID; }
  int GetSenderOrgID() const { return m_senderOrgID; }

  int GetReceiverCellID() const { return m_receiverCellID; }
  int GetReceiverOrgID() const { return m_receiverOrgID; }
	
	int GetMessageType() const { return m_messageType; }

private:
  cOrganism* m_pSender;
  cOrganism* m_pReceiver;
	int m_messageType;
  unsigned int m_data;
  unsigned int m_label;

  //! ID of the organism that sent this message
  int m_senderOrgID;

  //! ID of the cell that the sending organism occupied when this message was sent
  int m_senderCellID;

  //! ID of the organism that received this message
  int m_receiverOrgID;

  //! ID of the cell that the receiving organism occupied when this message was sent
  int m_receiverCellID;
};


#endif
