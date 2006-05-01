/*
 *  cStringList.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cStringList.h"

#include "functions.h"
#include "cStringIterator.h"

using namespace std;


cStringList::cStringList(const cString & _list, char seperator)
{
  Load(_list, seperator);
}

cStringList::cStringList(const cStringList & _list)
{
  tConstListIterator<cString> list_it(_list.string_list);
  while (list_it.Next() != NULL) {
    string_list.PushRear(new cString( *(list_it.Get()) ));
  }
}

cStringList::~cStringList()
{
  while (string_list.GetSize() > 0) delete string_list.Pop();
}


cStringList & cStringList::operator=(const cStringList & _list)
{
  // If this list already as anything in it, make sure to delete it.
  Clear();

  // Now add the new list into this one.
  tConstListIterator<cString> list_it(_list.string_list);
  while (list_it.Next() != NULL) {
    string_list.PushRear(new cString( *(list_it.Get()) ));
  }
  return *this;
}

bool cStringList::HasString(const cString & test_string) const
{
  tConstListIterator<cString> string_it(string_list);
  const cString * cur_string = NULL;
  while ((cur_string = string_it.Next()) != NULL) {
    if (test_string == *cur_string) {
      return true;
    }
  }
  return false;
}

cString cStringList::PopString(const cString & test_string)
{
  tListIterator<cString> string_it(string_list);
  const cString * cur_string = NULL;
  while ((cur_string = string_it.Next()) != NULL) {
    if (test_string == *cur_string) {
      return ReturnString(string_list.Remove(string_it));
    }
  }
  return "";
}


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

namespace nStringListTests {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cStringList_Archive", s);
  }
  
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cStringList_Archive", s);
  }
  

  namespace utStringList_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utStringList_archiving {
    void test(){
      char numstr[] = "number 0";
      cString s;
      cStringList sl;
      cStringIterator it(sl);
      std::string filename("./cStringList_basic_serialization.xml");
      int i, listsize = 10;
      
      /*
      Construct basic list.
      */
      BOOST_TEST(0 == sl.GetSize());
      for(i=0; i<listsize; i++){
        numstr[7] = '0' + i; 
        s = cString(numstr);
        sl.PushRear(s);
      }
      BOOST_TEST(listsize == sl.GetSize());
    
      /*
      Verify basic list contents.
      */
      it.Reset();
      for(i=0; !it.AtEnd(); i++){
        s = it.Next();
        numstr[7] = '0' + i;
        BOOST_TEST(cString(numstr) == s);
      }
      BOOST_TEST(i == listsize);

      /*
      Save basic list.
      */
      save_stuff<>(sl, filename.c_str());
      sl.Clear();
      BOOST_TEST(0 == sl.GetSize());

      /*
      Reload basic list.
      */
      restore_stuff<>(sl, filename.c_str());
      BOOST_TEST(listsize == sl.GetSize());

      /*
      Verify contents of reloaded basic list.
      */
      it.Reset();
      for(i=0; !it.AtEnd(); i++){
        s = it.Next();
        numstr[7] = '0' + i;
        BOOST_TEST(cString(numstr) == s);
      }
      BOOST_TEST(i == listsize);

      sl.Clear();
      BOOST_TEST(0 == sl.GetSize());

      std::remove(filename.c_str());
    }
  } // utStringList_archiving
} // nStringListTests

void cStringList::UnitTests(bool full)
{
  //if(full) {
  //  std::cout << "nStringListTests::utStringList_hello_world" << std::endl;
  //  nStringListTests::utStringList_hello_world::test();
  //}
  if(full) {
    std::cout << "nStringListTests::utStringList_archiving" << std::endl;
    nStringListTests::utStringList_archiving::test();
  }
}
