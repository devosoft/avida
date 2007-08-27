/*
 *  cOrgMessagePredicate.h
 *  Avida
 *
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
#ifndef cOrgMessagePredicate_h
#define cOrgMessagePredicate_h

#include <iostream>
#include <functional>
#include <set>

#include "cOrgMessage.h"
#include "cOrganism.h"


/*! \brief An STL-compatible predicate on cOrgMessages.  The intent here is to
provide a straightforward way to track arbitrary messages *wherever* they appear
in the population.  The most utility can be had from message predicates if they're
installed into cStats (since every message goes through cStats). */
struct cOrgMessagePredicate : public std::unary_function<cOrgMessage, bool> 
{
  virtual ~cOrgMessagePredicate() { }
  virtual bool operator()(const cOrgMessage& msg) = 0;
  virtual void Print(std::ostream& out) { }
  virtual void Reset() { }
};


/*! A predicate that returns true and tracks the sending cell_id for messages
that contain the same data field as this predicate was constructed with.
*/
struct cOrgMessagePred_DataEQU : public cOrgMessagePredicate 
{
  cOrgMessagePred_DataEQU(unsigned int data) : m_data(data) { }
  
  virtual bool operator()(const cOrgMessage& msg) {
    if(m_data==msg.GetData()) {
      m_cell_ids.insert(msg.GetSender()->GetCellID());
    }
    return true;
  }
  
  virtual void Print(std::ostream& out) { 
    out << "data==" << m_data << ":{";
    for(std::set<int>::iterator i=m_cell_ids.begin(); i!=m_cell_ids.end(); ++i) {
      out << *i << ",";
    }
    out << "}";
  }
  
  virtual void Reset() { 
    m_cell_ids.clear();
  }
  
  unsigned int m_data;
  std::set<int> m_cell_ids;
};


/*! A predicate that returns true and tracks the label and data field for messages
that contain a sink as the receiver.
*/
struct cOrgMessagePred_SinkReceiverEQU : public cOrgMessagePredicate {
  cOrgMessagePred_SinkReceiverEQU(unsigned int data) : m_data(data) { }
  
  virtual bool operator()(const cOrgMessage& msg) {
    if(m_data==(unsigned int)msg.GetReceiver()->GetCellID()) {
      m_cell_ids.insert(msg.GetData());
//      cWorld* w = msg.GetSender()->getWorld();
      /*      int i = w->GetPopulation().GetCell(289).GetRandomCellID();
      if(msg.GetData() == i) {
        std::cout<<"289 = "<< i<<endl;
      }*/
    }
    return true;
  }
  
  virtual void print(std::ostream& out) { 
//    cPopulationCell::t_id_map& ids = cPopulationCell::GetRandomCellIDMap();
//    int badMSGs = 0;
//    
//    out << "data==" << m_data << ":{";
//    for(std::set<int>::iterator i=m_cell_ids.begin(); i!=m_cell_ids.end(); ++i) {
//      int x,y;
//      //check if # is ID and log cell location and count bad messages
//      if(cPopulationCell::IsRandomCellID(*i)) {
//        cPopulationCell* cell = ids.find(*i)->second;
//        cell->GetPosition(x, y);
//        out << x <<" "<< y << ",";
//        m_cell_ids_total_good.insert(cell->GetRandomCellID());
//      } else {
//        badMSGs++;
//        //        out << *i << ",";
//      }
//    }
//    //write # bad messages in last slot
//    out << badMSGs;
//    out << "} "<<m_cell_ids_total_good.size();
  }
  
  virtual void reset() { 
    m_cell_ids.clear();
  }
  
  unsigned int m_data;
  std::set<int> m_cell_ids;
  std::set<int> m_cell_ids_total_good;
};


#endif
