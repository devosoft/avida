/*
 *  cFile.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cFile.h"

#include <iostream>

extern "C" {
#include <stdlib.h>
#include <errno.h>   // needed for FOPEN error constants (MSVC)
}

using namespace std;


bool cFile::Open(cString _fname, ios::openmode flags)
{
  if( IsOpen() ) Close();    // If a file is already open, clost it first.
  fp.open(_fname, flags);  // Open the new file.

  // Test if there was an error, and if so, try again!
  int err_id = fp.fail();
  if( err_id ){
    fp.clear();
    fp.open(_fname, flags);
  }

  // If there is still an error, determine its type and report it.
  err_id = fp.fail();
  if (err_id){
    cString error_desc = "?? Unknown Error??";

    // See if we can determine a more exact error type.
    if (err_id == EACCES) error_desc = "Access denied";
    else if (err_id == EINVAL) error_desc = "Invalid open flag or access mode";
    else if (err_id == ENOENT) error_desc = "File or path not found";

    // Print the error.
    cerr << "Unable to open file '" << _fname << "' : " << error_desc << endl;
    return false;
  }

  m_openmode = flags;
  filename = _fname;
  is_open = true;

  // Return true only if there were no problems...
  return( fp.good() && !fp.fail() );
}

bool cFile::Close()
{
  if (is_open == true) {
    fp.close();
    is_open = false;
    return true;
  }
  return false;
}

bool cFile::ReadLine(cString & in_string)
{
  char cur_line[MAX_STRING_LENGTH];
  cur_line[0]='\0';
  fp.getline(cur_line, MAX_STRING_LENGTH);
  if( fp.bad() ){
    return false;
  }
  in_string = cur_line;
  return true;
}


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

namespace nFileTests {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cFile_Archive", s);
  }
  
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cFile_Archive", s);
  }
  

  namespace utFile_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utFile_archiving {
    void test(){
      std::string filename("./cFile_basic_serialization.xml");
      int linecount = 3;
      std::string data_file_name("./cFile_data.txt");
      {
        std::ofstream data_file(data_file_name.c_str());
        for(int i = 0; i < linecount; i++){
          data_file << 2 * i << std::endl;
        }
      }
      
      // Open cFile_data.txt for reading.
      cFile f(data_file_name.c_str());
      
      cFile f1, f2, f3;
      cString s1, s2, s3;
      cString l1, l2, l3;
      
      // Save initial cFile state.
      save_stuff<>(f, filename.c_str());
      // Reload initial state into new cFile.
      restore_stuff<>(f1, filename.c_str());
  
      // Save cFile state after reading first line.
      f.ReadLine(s1);
      save_stuff<>(f, filename.c_str());
      // Reload second state into new cFile.
      restore_stuff<>(f2, filename.c_str());
    
      // Save cFile state after reading second line.
      f.ReadLine(s2);
      save_stuff<>(f, filename.c_str());
      // Reload third state into new cFile.
      restore_stuff<>(f3, filename.c_str());
  
      f.ReadLine(s3);
  
      // Sanity checks...
      //BOOST_TEST(false);
      BOOST_TEST(cString("0") == s1);
      BOOST_TEST(cString("2") == s2);
      BOOST_TEST(cString("4") == s3);
  
      // Verify reading expected lines from various reloaded states.
      f3.ReadLine(l3);
      f2.ReadLine(l2);
      f1.ReadLine(l1);
      BOOST_TEST(l1 == s1);
      BOOST_TEST(l2 == s2);
      BOOST_TEST(l3 == s3);
  
      std::remove(filename.c_str());
      std::remove(data_file_name.c_str());
    }
  } // utFile_archiving

  namespace utFile_archiving_closed_file {
    void test(){
      std::string data_file_name("./cFile_data.txt");
      {
        std::ofstream data_file(data_file_name.c_str());
        for(int i = 0; i < 3; i++){
          data_file << 2 * i << std::endl;
        }
      }

      std::string filename("./cFile_serialize_closed_file.xml");

      // Open cFile_data.txt for reading.
      cFile f(data_file_name.c_str());
      // Close file.
      f.Close();
      BOOST_TEST(!f.IsOpen());

      cFile f1;

      // Save cFile state.
      save_stuff<>(f, filename.c_str());
      // Reload state into new cFile.
      restore_stuff<>(f1, filename.c_str());
      // Verify new cFile has matching filename.
      BOOST_TEST(f.GetFilename() == f1.GetFilename());
      // Verify new cFile is closed.
      BOOST_TEST(!f1.IsOpen());

      std::remove(filename.c_str());
      std::remove(data_file_name.c_str());
    }
  } // utFile_archiving_closed_file
} // nFileTests

void cFile::UnitTests(bool full)
{
  //if(full) {
  //  std::cout << "nFileTests::utFile_hello_world" << std::endl;
  //  nFileTests::utFile_hello_world::test();
  //}
  if(full) {
    std::cout << "nFileTests::utFile_archiving" << std::endl;
    nFileTests::utFile_archiving::test();
  }
  if(full) {
    std::cout << "nFileTests::utFile_archiving_closed_file" << std::endl;
    nFileTests::utFile_archiving_closed_file::test();
  }
}

#endif // ENABLE_UNIT_TESTS
