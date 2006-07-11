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

namespace nStringList {
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
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(true);
      TEST(false);
    }
  }

  namespace utStringList_archiving {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      char numstr[] = "number 0";
      cString s;
      cStringList sl;
      cStringIterator it(sl);
      std::string filename("./cStringList_basic_serialization.xml");
      int i, listsize = 10;
      
      /*
      Construct basic list.
      */
      TEST(0 == sl.GetSize());
      for(i=0; i<listsize; i++){
        numstr[7] = '0' + i; 
        s = cString(numstr);
        sl.PushRear(s);
      }
      TEST(listsize == sl.GetSize());
    
      /*
      Verify basic list contents.
      */
      it.Reset();
      for(i=0; !it.AtEnd(); i++){
        s = it.Next();
        numstr[7] = '0' + i;
        TEST(cString(numstr) == s);
      }
      TEST(i == listsize);

      /*
      Save basic list.
      */
      save_stuff<>(sl, filename.c_str());
      sl.Clear();
      TEST(0 == sl.GetSize());

      /*
      Reload basic list.
      */
      restore_stuff<>(sl, filename.c_str());
      TEST(listsize == sl.GetSize());

      /*
      Verify contents of reloaded basic list.
      */
      it.Reset();
      for(i=0; !it.AtEnd(); i++){
        s = it.Next();
        numstr[7] = '0' + i;
        TEST(cString(numstr) == s);
      }
      TEST(i == listsize);

      sl.Clear();
      TEST(0 == sl.GetSize());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utStringList_archiving



  void UnitTests(bool full)
  {
    //if(full) utStringList_hello_world::test();
    if(full) utStringList_archiving::test();
  }
} // nStringList

#endif // ENABLE_UNIT_TESTS
