/*
 *  cFixedBlock.cc
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cFixedBlock.h"

#ifdef ENABLE_UNIT_TESTS

/*
Unit tests
*/
#include "cXMLArchive.h"

#include <boost/detail/lightweight_test.hpp>

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

namespace nFixedBlock {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cFixedBlock_Archive", s);
  }

  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cFixedBlock_Archive", s);
  }


  namespace utFixedBlock_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utFixedBlock_archiving {
    void test(){
      std::string filename("./cFixedBlock_basic_serialization.xml");

      /*
      Create and archive a cFixedBlock object.
      */
      {
        cFixedBlock fb;
        BOOST_TEST(0 == fb.GetStart());
        fb.SetStart(2);
        BOOST_TEST(2 == fb.GetStart());
        save_stuff<>(fb, filename.c_str());
      }
      /*
      Reload archived cFixedBlock object.
      */
      {
        cFixedBlock fb;
        BOOST_TEST(0 == fb.GetStart());
        restore_stuff<>(fb, filename.c_str());
        BOOST_TEST(2 == fb.GetStart());
      }

      std::remove(filename.c_str());
    }
  } // utFixedBlock_archiving


  void UnitTests(bool full)
  {
    //if(full) {
    //  std::cout << "utFixedBlock_hello_world" << std::endl;
    //  utFixedBlock_hello_world::test();
    //}
    if(full) {
      std::cout << "utFixedBlock_archiving" << std::endl;
      utFixedBlock_archiving::test();
    }
  }
} // nFixedBlock

#endif // ENABLE_UNIT_TESTS

