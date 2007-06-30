/*
 *  cSaleItem.h
 *  Avida
 *
 *  Called "event_list.hh" prior to 12/2/05.
 *  Copyright 2006-2007 Michigan State University. All rights reserved.
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

#ifndef cSaleItem_h
#define cSaleItem_h

#ifndef cOrganism_h
#include "cOrganism.h"
#endif

class cSaleItem
{
private:
  int m_data;
  int m_label;
  int m_price;
  int m_org_id;
  int m_cell_id;
  tListNode<tListNode<cSaleItem> >* m_nodeptr;

public:
  cSaleItem() : m_data(0), m_label(0), m_price(0), m_org_id(0), m_cell_id(0)  { ; }
  cSaleItem(const int in_data, const int in_label, const int in_price, 
    const int in_org_id, const int in_cell_id) : 
    m_data(in_data),
    m_label(in_label),
    m_price(in_price),
    m_org_id(in_org_id),
    m_cell_id(in_cell_id)
  {}

  ~cSaleItem() { ; }
  int GetData() { return m_data; }
  void SetData(const int in_data) { m_data = in_data; }
  int GetLabel() { return m_label; }
  void SetLabel(const int in_label) { m_label = in_label; }
  int GetPrice() { return m_price; }
  void SetPrice(const int in_price) { m_price = in_price; }
  int GetOrgID() { return m_org_id; }
  int GetCellID() { return m_cell_id; }
  void SetNodePtr(tListNode<tListNode<cSaleItem> >* in_nodeptr) { m_nodeptr = in_nodeptr; }
  tListNode<tListNode<cSaleItem> >* GetNodePtr() { return m_nodeptr; }
  bool operator == (cSaleItem &rhs)
  {
    return ( (m_data == rhs.GetData()) && (m_label == rhs.GetLabel()) && 
      (m_org_id == rhs.GetOrgID()) && (m_price == rhs.GetPrice()) &&
      (m_cell_id == rhs.GetCellID())); 
  }
};

#endif
