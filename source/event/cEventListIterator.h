/*
 *  cEventListIterator.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEventListIterator_h
#define cEventListIterator_h

#include <iostream>

#ifndef cEventListEntry_h
#include "cEventListEntry.h"
#endif

/**
 * sort of an iterator for cEventList;
 * doesn't allow access to its cEventListEntry, but will allow its entry
 * to be printed to an ostream.
 * added by Kaben.
 **/

class cEventListIterator {
private:
  cEventListEntry *m_node;
public:
  cEventListIterator() : m_node(0) {}
  cEventListIterator(cEventListEntry *node) : m_node(node) {}
  cEventListIterator(const cEventListIterator &it) : m_node(it.m_node) {}

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


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
