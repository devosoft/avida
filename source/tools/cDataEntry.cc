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

#include "lightweight_test.h"

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

namespace nDataEntry {
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
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(true);
      TEST(false);
    }
  }

  namespace utDataEntry_archiving {
    void test(){
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./cDataEntry_basic_serialization.xml");

      // Open cDataEntry_data.txt for reading.
      cDataEntry d("name", "desc", "null_value", "html_table_flags");
      cDataEntry d1("n", "d", "nv", "htf");

      // Sanity checks...
      TEST(cString("name") == d.GetName());
      TEST(cString("desc") == d.GetDesc());
      TEST(cString("null_value") == d.GetNull());
      TEST(cString("html_table_flags") == d.GetHtmlCellFlags());

      TEST(cString("n") == d1.GetName());
      TEST(cString("d") == d1.GetDesc());
      TEST(cString("nv") == d1.GetNull());
      TEST(cString("htf") == d1.GetHtmlCellFlags());

      // Save cDataEntry state.
      save_stuff<>(d, filename.c_str());
      // Reload state into new cDataEntry.
      restore_stuff<>(d1, filename.c_str());

      // Check reloaded state.
      TEST(cString("name") == d1.GetName());
      TEST(cString("desc") == d1.GetDesc());
      TEST(cString("null_value") == d1.GetNull());
      TEST(cString("html_table_flags") == d1.GetHtmlCellFlags());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    } 
  } // utDataEntry_archiving



  void UnitTests(bool full)
  {
    //if(full) utDataEntry_hello_world::test();
    if(full) utDataEntry_archiving::test();
  }
} // nDataEntry

#endif // ENABLE_UNIT_TESTS
