/*
 *  cSchedule.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cSchedule.h"

#include "cChangeList.h"


cSchedule::cSchedule(int _item_count)
: item_count(_item_count)
, m_change_list(0)
{
}

cSchedule::~cSchedule()
{
}

void cSchedule::SetChangeList(cChangeList *change_list) {
  m_change_list = change_list;
  if (m_change_list) m_change_list->Resize(item_count);
}
void cSchedule::SetSize(int _item_count) {
  item_count = _item_count;
  if (m_change_list) m_change_list->Resize(_item_count);
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

namespace nSchedule {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cSchedule_Archive", s);
  } 

  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename); 
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cSchedule_Archive", s);
  } 
  class testSchedule : public cSchedule {
#if USE_tMemTrack
    tMemTrack<testSchedule> mt;
#endif
  public:
    virtual int GetNextID(){ return 0; }
  };

  namespace utSchedule_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utSchedule_archiving {
    void test(){
      std::string filename("./cSchedule_basic_serialization.xml");
      BOOST_TEST(tMemTrack<cChangeList>::Instances() == 0);
      {
      }
      {
      }

      std::remove(filename.c_str());
    }
  } // utSchedule_archiving


  void UnitTests(bool full)
  {
    //if(full) {
    //  std::cout << "utSchedule_hello_world" << std::endl;
    //  utSchedule_hello_world::test();
    //}
    if(full) {
      std::cout << "utSchedule_archiving" << std::endl;
      utSchedule_archiving::test();
    }
  }
} // nSchedule

#endif // ENABLE_UNIT_TESTS
