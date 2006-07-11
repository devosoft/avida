/*
 *  cFixedCoords.cc
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cFixedCoords.h"

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

namespace nFixedCoords {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cFixedCoords_Archive", s);
  }

  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cFixedCoords_Archive", s);
  }


  namespace utFixedCoords_hello_world {
    void test(){
      std::cout << "utFixedCoords_hello_world" << std::endl;
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utFixedCoords_archiving {
    void test(){
#   ifdef ENABLE_SERIALIZATION
      std::cout << "utFixedCoords_archiving" << std::endl;
      std::string filename("./cFixedCoords_basic_serialization.xml");

      /*
      Create and archive a cFixedCoords object.
      */
      {
        cFixedCoords fc;
        BOOST_TEST(0 == fc.GetBlockNum());
        BOOST_TEST(0 == fc.GetOffset());
        fc(2,3);
        BOOST_TEST(2 == fc.GetBlockNum());
        BOOST_TEST(3 == fc.GetOffset());
        save_stuff<>(fc, filename.c_str());
      }
      /*
      Reload archived cFixedCoords object.
      */
      {
        cFixedCoords fc;
        BOOST_TEST(0 == fc.GetBlockNum());
        BOOST_TEST(0 == fc.GetOffset());
        restore_stuff<>(fc, filename.c_str());
        BOOST_TEST(2 == fc.GetBlockNum());
        BOOST_TEST(3 == fc.GetOffset());
      }

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utFixedCoords_archiving


  void UnitTests(bool full)
  {
    //if(full) utFixedCoords_hello_world::test();
    if(full) utFixedCoords_archiving::test();
  }
} // nFixedCoords

#endif // ENABLE_UNIT_TESTS


