/*
 *  cChangeList.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology.
 *
 */

#include "cChangeList.h"


void cChangeList::ResizeClear(int capacity)
{
  m_change_list.ResizeClear(capacity);
  m_change_tracking.ResizeClear(capacity);
  m_change_list.SetAll(0);
  m_change_tracking.SetAll(false);
  m_change_count = 0;
}

// Note that decreasing size invalidates stored changes.
void cChangeList::Resize(int capacity)
{
  if (capacity < m_change_list.GetSize()) {
    ResizeClear(capacity);
  } else {
    m_change_list.Resize(capacity);
    m_change_tracking.Resize(capacity, false);
  }
}

// Unsafe version : assumes changed_index is within capacity.
void cChangeList::MarkChange(int changed_index)
{
  if (!m_change_tracking[changed_index]) {
    m_change_tracking[changed_index] = true;
    m_change_list[m_change_count] = changed_index;
    m_change_count++;
  }
}

// Safe version : will resize to accommodate changed_index greater
// than capacity.
void cChangeList::PushChange(int changed_index)
{
  if (m_change_list.GetSize() <= changed_index) {
    Resize(changed_index + 1);
  }
  MarkChange(changed_index);
}

void cChangeList::Reset()
{
  for (int i = 0; i < m_change_count; i++) {
    m_change_tracking[m_change_list[i]] = false;
  }
  m_change_count = 0;
}


#ifdef ENABLE_UNIT_TESTS

/*
Unit tests
*/
#include "cFile.h"
#include "cXMLArchive.h"

#include <boost/detail/lightweight_test.hpp>

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

namespace nChangeList {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cChangeList_Archive", s);
  }

  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cChangeList_Archive", s);
  }


  namespace utChangeList_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utChangeList_archiving {
    void test(){
      std::string filename("./cChangeList_basic_serialization.xml");
      int changelist_size = 10;
      int change_count = 5;
      tArray<int> recorded_changes(change_count);

      {
        cChangeList cl(changelist_size);
        BOOST_TEST(cl.GetSize() == changelist_size);

        BOOST_TEST(cl.GetChangeCount() == 0);
        cl.PushChange(1);
        cl.PushChange(7);
        cl.PushChange(3);
        cl.PushChange(5);
        cl.PushChange(2);
        BOOST_TEST(cl.GetChangeCount() == change_count);

        for(int i = 0; i < change_count; i++) {
          recorded_changes[i] = cl.CheckChangeAt(i);
        }
        save_stuff<>(cl, filename.c_str());
      }

      {
        cChangeList cl;
        BOOST_TEST(cl.GetSize() == 0);
        BOOST_TEST(cl.GetChangeCount() == 0);

        restore_stuff<>(cl, filename.c_str());
        BOOST_TEST(cl.GetChangeCount() == change_count);
        for(int i = 0; i < change_count; i++) {
          BOOST_TEST(recorded_changes[i] == cl.CheckChangeAt(i));
        }
      }

      //std::remove(filename.c_str());
    }
  } // utChangeList_archiving



  void UnitTests(bool full)
  { 
    //if(full) {
    //  std::cout << "utChangeList_hello_world" << std::endl;
    //  utChangeList_hello_world::test();
    //}
    if(full) {
      std::cout << "utChangeList_archiving" << std::endl;
      utChangeList_archiving::test();
    }
  }
} // nChangeList

#endif // ENABLE_UNIT_TESTS

