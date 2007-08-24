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

#include <iostream>
#include <functional>
#include <set>

class cOrganism;


/*!
*/
class cOrgMessage
{
private:
  cOrganism* m_pSender;
  cOrganism* m_pReceiver;
  unsigned int m_data;
  unsigned int m_label;
  
  cOrgMessage() 
    : m_pSender(NULL), m_pReceiver(NULL), m_data(0), m_label(0) {
    }
  
public:
    cOrgMessage(cOrganism* sender)
    : m_pSender(sender), m_pReceiver(NULL), m_data(0), m_label(0) {
    }
  
  explicit cOrgMessage(int data, int label)
    : m_pSender(NULL), m_pReceiver(NULL), m_data(data), m_label(label) {
    }
  
  static cOrgMessage EmptyMessage() { return cOrgMessage(); }
  
  cOrganism* GetSender() { return m_pSender; }
  cOrganism* GetReceiver() { return m_pReceiver; }
  void SetReceiver(cOrganism* recvr) { m_pReceiver = recvr; }
  
  unsigned int GetData() const { return m_data; }
  unsigned int GetLabel() const { return m_label; }
  
  void SetData(unsigned int data) { m_data = data; }
  void SetLabel(unsigned int label) { m_label = label; }
};


///*! \brief An STL-compatible predicate on cOrgMessages.  The intent here is to
//provide a straightforward way to track arbitrary messages *wherever* they appear
//in the population.  The most utility can be had from message predicates if they're
//installed into cStats (since every message goes through cStats). */
//struct cOrgMessage_Predicate : public std::unary_function<cOrgMessage, bool> 
//{
//  virtual ~cOrgMessage_Predicate() { }
//  virtual bool operator()(cOrgMessage& msg) = 0;
//  virtual void Print(std::ostream& out) { }
//  virtual void Reset() { }
//};
//
//
///*! A predicate that returns true and tracks the sending cell_id for messages
//that contain the same data field as this predicate was constructed with.
//*/
//struct cOrgMessage_PredDataEQU : public cOrgMessage_Predicate 
//{
//  cOrgMessage_PredDataEQU(unsigned int data) : m_data(data) { }
//  
//  virtual bool operator()(cOrgMessage& msg) {
//    if(m_data==msg.GetData()) {
//      m_cell_ids.insert(msg.GetSender()->GetCellID());
//    }
//    return true;
//  }
//  
//  virtual void Print(std::ostream& out) { 
//    out << "data==" << m_data << ":{";
//    for(std::set<int>::iterator i=m_cell_ids.begin(); i!=m_cell_ids.end(); ++i) {
//      out << *i << ",";
//    }
//    out << "}";
//  }
//  
//  virtual void Reset() { 
//    m_cell_ids.clear();
//  }
//  
//  unsigned int m_data;
//  std::set<int> m_cell_ids;
//};


#endif
