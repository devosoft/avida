//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_LIST_ITERATOR_HH
#define EVENT_LIST_ITERATOR_HH

#include <iostream>

#ifndef EVENT_LIST_ENTRY_HH
#include "cEventListEntry.h"
#endif

/**
 * sort of an iterator for cEventList;
 * doesn't allow access to its cEventListEntry, but will allow its entry
 * to be printed to an ostream.
 * added by Kaben.
 **/

class cEventListEntry; // access

class cEventListIterator {
private:
  cEventListEntry *m_node;
public:
  cEventListIterator() : m_node(0) {}
  cEventListIterator(cEventListEntry *node) : m_node(node) {}
  cEventListIterator(const cEventListIterator &it) : m_node(it.m_node) {}

  void PrintEvent(std::ostream& os = std::cout);

  bool operator==(const cEventListIterator &it) const {
    return m_node == it.m_node;
  }
  bool operator!=(const cEventListIterator &it) const {
    return m_node != it.m_node;
  }
  cEventListIterator& operator++() {
    m_node = m_node->GetNext();
    return *this;
  }
};

#endif
