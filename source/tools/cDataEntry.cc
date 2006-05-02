/*
 *  cDataEntry.cc
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cDataEntry.h"

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

namespace nDataEntryTests {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cDataEntry_Archive", s);
  }

  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cDataEntry_Archive", s);
  }


  namespace utDataEntry_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utDataEntry_archiving {
    void test(){
      std::string filename("./cDataEntry_basic_serialization.xml");

      // Open cDataEntry_data.txt for reading.
      cDataEntry d("name", "desc", "null_value", "html_table_flags");
      cDataEntry d1("n", "d", "nv", "htf");

      // Sanity checks...
      BOOST_TEST(cString("name") == d.GetName());
      BOOST_TEST(cString("desc") == d.GetDesc());
      BOOST_TEST(cString("null_value") == d.GetNull());
      BOOST_TEST(cString("html_table_flags") == d.GetHtmlCellFlags());

      BOOST_TEST(cString("n") == d1.GetName());
      BOOST_TEST(cString("d") == d1.GetDesc());
      BOOST_TEST(cString("nv") == d1.GetNull());
      BOOST_TEST(cString("htf") == d1.GetHtmlCellFlags());

      // Save cDataEntry state.
      save_stuff<>(d, filename.c_str());
      // Reload state into new cDataEntry.
      restore_stuff<>(d1, filename.c_str());

      // Check reloaded state.
      BOOST_TEST(cString("name") == d1.GetName());
      BOOST_TEST(cString("desc") == d1.GetDesc());
      BOOST_TEST(cString("null_value") == d1.GetNull());
      BOOST_TEST(cString("html_table_flags") == d1.GetHtmlCellFlags());

      std::remove(filename.c_str());
    } 
  } // utDataEntry_archiving
} // nDataEntryTests

void cDataEntry::UnitTests(bool full)
{
  //if(full) {
  //  std::cout << "nDataEntryTests::utDataEntry_hello_world" << std::endl;
  //  nDataEntryTests::utDataEntry_hello_world::test();
  //}
  if(full) {
    std::cout << "nDataEntryTests::utDataEntry_archiving" << std::endl;
    nDataEntryTests::utDataEntry_archiving::test();
  }
}

#endif // ENABLE_UNIT_TESTS
