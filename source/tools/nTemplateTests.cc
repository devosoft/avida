/*
 *  nTemplateTests.cc
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "nTemplateTests.h"


/*
Unit tests
*/
#include "cXMLArchive.h"
#include "tArray.h"
#include "tBuffer.h"
#include "tDictionary.h"
#include "tHashTable.h"
#include "tList.h"
#include "tManagedPointerArray.h"
#include "tMatrix.h"
#include "tMemTrack.h"
#include "tSmartArray.h"

#include "lightweight_test.h"

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream> 
#include <string>

namespace nTemplateTests {
  /*
  Test-helpers.
  */  
  class A {
    tMemTrack<A> mt;
  public:
    int m_i;
    int m_j;
  public:
    A():m_i(-1),m_j(-1){}
    A(const A &in):m_i(in.m_i),m_j(in.m_j){}
    A& operator=(const A& in){ 
      m_i = in.m_i;
      m_j = in.m_j;
      return *this;
    }
    template<class Archive>
    void serialize(Archive & a, const unsigned int version){
      a.ArkvObj("m_i", m_i);
      a.ArkvObj("m_j", m_j);
    } 
  };  
  // ostream output, needed for tHashTable::OK()
  std::ostream& operator << (std::ostream& out, const A & a){
    out << "(" << a.m_i << "," << a.m_j << ")";
    return out;
  }
      
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("TemplateTest", s);
  }   
      
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("TemplateTest", s);
  }   
    

  namespace utTemplateTests_hello_world {
    void test(){
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(true);
      TEST(false);
    }
  }
  

  namespace utMemTrack {
    void test() {
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(0 == tMemTrack<A>::Instances());
      {
        A a;
        TEST(1 == tMemTrack<A>::Instances());
      }
      TEST(0 == tMemTrack<A>::Instances());
      A *p;
      TEST(0 == tMemTrack<A>::Instances());
      p = new A();
      TEST(1 == tMemTrack<A>::Instances());
      delete p;
      TEST(0 == tMemTrack<A>::Instances());
      
    }
  }
  
  namespace utList_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(0 == tMemTrack<A>::Instances());
      A *a;
      tList<A> l;
      tListIterator<A> it(l);
      std::string filename("./tList_basic_serialization.xml");
      int i, listsize = 10;
      
      TEST(1 == tMemTrack<tList<A> >::Instances());
      TEST(1 == tMemTrack<tListNode<A> >::Instances());
      
      /*
      Construct basic list.
      */
      TEST(0 == tMemTrack<A>::Instances());
      for(i=0; i<listsize; i++){
        a = new A;
        a->m_i = i; 
        l.PushRear(a);
      }
      TEST(listsize == tMemTrack<A>::Instances());
      TEST(1 + listsize == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Verify basic list contents.
      */
      TEST(listsize == tMemTrack<A>::Instances());
      it.Reset();
      for(i=0; (a = it.Next()); i++){
        TEST(i == a->m_i);
      }
  
      /*
      Save basic list.
      */
      save_stuff<>(l, filename.c_str());
      it.Reset();
      for(i=0; (a = it.Next()); i++){
        delete a;
      }
      l.Clear();
      TEST(0 == tMemTrack<A>::Instances());
      TEST(1 == tMemTrack<tList<A> >::Instances());
      TEST(1 == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Reload basic list.
      */
      restore_stuff<>(l, filename.c_str());
      TEST(1 == tMemTrack<tList<A> >::Instances());
      TEST(listsize == tMemTrack<A>::Instances());
      TEST(1 + listsize == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Verify contents of reloaded basic list.
      */
      for(i=0; i<listsize; i++){
        a = l.Pop();
        TEST(i == a->m_i);
        delete a;
      }
  
      TEST(0 == tMemTrack<A>::Instances());
      TEST(1 == tMemTrack<tList<A> >::Instances());
      TEST(1 == tMemTrack<tListNode<A> >::Instances());
  
      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utList_basic_serialization
  
  namespace utList_archive_structure {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tList_archive_structure.xml");
      
      A *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10;
      
      tList<A> l;
      tListIterator<A> it(l);
      
      /*
      Construct list...
      */
      TEST(0 == tMemTrack<A>::Instances());
      a1 = new A;
      a2 = new A;
      TEST(2 == tMemTrack<A>::Instances());
      l.PushRear(a1); 
      l.PushRear(a2);
      l.PushRear(a1);
      l.PushRear(a2);
      TEST(2 == tMemTrack<A>::Instances());
      
      /*
      Verify list structure. It should now contain two copies each,
      interleaved, of two instances of A.
      */
      it.Reset();
      a3 = it.Next();
      a4 = it.Next();
      a5 = it.Next();
      a6 = it.Next();
      TEST(2 == tMemTrack<A>::Instances());
  
      a1->m_i = 3;
      a2->m_i = 5;
      TEST(3 == a3->m_i);
      TEST(5 == a4->m_i);
      TEST(3 == a5->m_i);
      TEST(5 == a6->m_i);
  
      /*
      Save to disk, clear, and reload.
      */
      save_stuff<>(l, filename.c_str());
      TEST(2 == tMemTrack<A>::Instances());
  
      l.Clear();
      TEST(2 == tMemTrack<A>::Instances());
  
      restore_stuff<>(l, filename.c_str());
  
      /*
      New list should contain two copies each, interleaved, of the two
      new instances of A.
      */
      TEST(4 == tMemTrack<A>::Instances());
  
      /*
      Verify new list structure.
      */
      it.Reset();
      a7 = it.Next();
      a8 = it.Next();
      a9 = it.Next();
      a10 = it.Next();
      TEST(4 == tMemTrack<A>::Instances());
  
      TEST(3 == a7->m_i);
      TEST(5 == a8->m_i);
      TEST(3 == a9->m_i);
      TEST(5 == a10->m_i);
  
      /*
      Verify that elements of old and new lists are independent.
      */
      a1->m_i = 7;
      a7->m_i = 11;
      a8->m_i = 13;
      TEST(7 == a3->m_i);
      TEST(5 == a4->m_i);
      TEST(7 == a5->m_i);
      TEST(5 == a6->m_i);
      TEST(11 == a7->m_i);
      TEST(13 == a8->m_i);
      TEST(11 == a9->m_i);
      TEST(13 == a10->m_i);
  
      l.Clear();
      /*
      Delete originals instances of A.
      */
      delete a1;
      delete a2;
      /*
      Delete reloaded copies of A.
      */
      delete a7;
      delete a8;
      TEST(0 == tMemTrack<A>::Instances());
  
      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utList_archive_structure

  namespace utArrays_of_pointers {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      const int num_instances = 3;
      tArray<A *> instance_array(num_instances);
      TEST(0 == tMemTrack<A>::Instances());
      for (int i = 0; i < num_instances; i++){
        instance_array[i] = new A();
      }
      TEST(num_instances == tMemTrack<A>::Instances());
      for (int i = 0; i < num_instances; i++){
        delete instance_array[i];
      }
      TEST(0 == tMemTrack<A>::Instances());
#   endif // ENABLE_SERIALIZATION
    }
  } // utArrays_of_pointers

  namespace utArray_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tArray_basic_serialization.xml");
      int i, arraysize = 10;

      TEST(0 == tMemTrack<A>::Instances());
      tArray<A> l(arraysize);
      TEST(arraysize == tMemTrack<A>::Instances());
       
      /*
      Construct basic array.
      */
      for(i=0; i<arraysize; i++){
        l[i].m_i = i;
      }
  
      /*
      Verify basic array contents.
      */ 
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utArray_basic_serialization
  
  namespace utHashTable_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tHashTable_basic_serialization.xml");

      TEST(0 == tMemTrack<A>::Instances());
      tHashTable<cString, A> d;
      char keystr[9] = "number 0";
      int i, dictsize = 10;
      TEST(0 == tMemTrack<A>::Instances());

      // Sanity checks.
      TEST(d.OK());
      TEST(0 == d.GetSize());
      TEST(!d.HasEntry("blah"));
      {
        A a;
        TEST(!d.Find("blah", a));
      }

      /*
      Construct basic dict.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        a.m_i = i;
        d.Add(cString(keystr), a);
      }
      TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Verify basic dict contents.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        TEST(d.Find(cString(keystr), a));
        TEST(i == a.m_i);
      }
      TEST(d.OK());

      /*
      Save basic table.
      */
      save_stuff<>(d, filename.c_str());
      TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Reload basic table.
      */
      {
        tHashTable<cString, A> d2;
        restore_stuff<>(d2, filename.c_str());
        TEST(2 * dictsize == tMemTrack<A>::Instances());
        TEST(d2.OK());

        /*
        Verify contents of reloaded basic dict.
        */
        for(i=0; i<dictsize; i++){
          A a;
          keystr[7] = '0' + i;
          TEST(d2.Find(cString(keystr), a));
          TEST(i == a.m_i);
        }
        TEST(2 * dictsize == tMemTrack<A>::Instances());
      }

      TEST(dictsize == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utHashTable_basic_serialization

  namespace utDictionary_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tDictionary_basic_serialization.xml");

      TEST(0 == tMemTrack<A>::Instances());
      tDictionary<A> d;
      char keystr[9] = "number 0";
      int i, dictsize = 10;
      TEST(0 == tMemTrack<A>::Instances());

      // Sanity checks.
      TEST(d.OK());
      TEST(0 == d.GetSize());
      TEST(!d.HasEntry("blah"));
      {
        A a;
        TEST(!d.Find("blah", a));
      }
      TEST(cString("") == d.NearMatch("blah"));

      /*
      Construct basic dict.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        a.m_i = i;
        d.Add(cString(keystr), a);
      }
      TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Verify basic dict contents.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        TEST(d.Find(cString(keystr), a));
        TEST(i == a.m_i);
      }
      TEST(d.OK());

      /*
      Save basic list.
      */
      save_stuff<>(d, filename.c_str());
      TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Reload basic list.
      */
      {
        tDictionary<A> d2;
        restore_stuff<>(d2, filename.c_str());
        TEST(2 * dictsize == tMemTrack<A>::Instances());
        TEST(d2.OK());

        /*
        Verify contents of reloaded basic dict.
        */
        for(i=0; i<dictsize; i++){
          A a;
          keystr[7] = '0' + i;
          TEST(d2.Find(cString(keystr), a));
          TEST(i == a.m_i);
        }
        TEST(2 * dictsize == tMemTrack<A>::Instances());
      }

      TEST(dictsize == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utDictionary_basic_serialization

  namespace utBuffer_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tBuffer_basic_serialization.xml");
      A a;
      int i, buffer_size = 10;

      TEST(1 == tMemTrack<A>::Instances());
      tBuffer<A> b(buffer_size);
      TEST(1 + buffer_size == tMemTrack<A>::Instances());
       
      /*
      Construct basic buffer.
      */
      for(i=0; i < buffer_size - 1; i++){
        a.m_i = i;
        b.Add(a);
      }
  
      /*
      Verify basic buffer contents.
      */ 
      for(i=0; i < buffer_size - 1; i++){
        TEST(buffer_size - 2 - i == b[i].m_i);
      }
      TEST(b.GetCapacity() == buffer_size);
      TEST(b.GetTotal() == buffer_size - 1);
      TEST(b.GetNumStored() == buffer_size - 1);
      TEST(b.GetNum() == buffer_size - 1);
      
      /* 
      Save and clear basic buffer.
      */
      save_stuff<>(b, filename.c_str());
      b.Clear();
      for(i=0; i < buffer_size - 1; i++){
        a.m_i = 0;
        b.Add(a);
      }
      for(i=0; i < buffer_size - 1; i++){
        TEST(0 == b[i].m_i);
      }
      b.Clear();
      TEST(b.GetCapacity() == buffer_size);
      TEST(b.GetTotal() == 0);
      TEST(b.GetNumStored() == 0);
      TEST(b.GetNum() == 0);

      /*
      Reload basic buffer.
      */
      restore_stuff<>(b, filename.c_str());
      TEST(1 + buffer_size == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic buffer.
      */
      for(i=0; i < buffer_size - 1; i++){
        TEST(buffer_size - 2 - i == b[i].m_i);
      }
      TEST(b.GetCapacity() == buffer_size);
      TEST(b.GetTotal() == buffer_size - 1);
      TEST(b.GetNumStored() == buffer_size - 1);
      TEST(b.GetNum() == buffer_size - 1);

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utBuffer_basic_serialization

  namespace utManagedPointerArray_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tManagedPointerArray_basic_serialization.xml");
      int i, arraysize = 10;

      TEST(0 == tMemTrack<A>::Instances());
      tManagedPointerArray<A> l(arraysize);
      TEST(arraysize == tMemTrack<A>::Instances());
       
      /*
      Construct basic array.
      */
      for(i=0; i<arraysize; i++){
        l[i].m_i = i;
      }
  
      /*
      Verify basic array contents.
      */ 
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utManagedPointerArray_basic_serialization

  namespace utSmartArray_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tSmartArray_basic_serialization.xml");
      int i, arraysize = 10;

      TEST(0 == tMemTrack<A>::Instances());
      tSmartArray<A> l(arraysize);
      TEST(arraysize == tMemTrack<A>::Instances());
       
      /*
      Construct basic array.
      */
      for(i=0; i<arraysize; i++){
        l[i].m_i = i;
      }
  
      /*
      Verify basic array contents.
      */ 
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utSmartArray_basic_serialization

  namespace utMatrix_basic_serialization {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./tMatrix_basic_serialization.xml");
      int i, j, rows = 3, cols = 5;
      
      TEST(0 == tMemTrack<A>::Instances());
      tMatrix<A> m(rows, cols);
      TEST(rows * cols == tMemTrack<A>::Instances());
      
      /*
      Construct basic matrix.
      */
      for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
          m[i][j].m_i = i;
          m[i][j].m_j = j;
        }
      }
  
      /*
      Verify basic matrix contents.
      */
      for(i=0; i<rows; i++){ 
        for(j=0; j<cols; j++){
          TEST(i == m[i][j].m_i); 
          TEST(j == m[i][j].m_j);
        }
      } 
        
      /*
      Save basic matrix. 
      */
      save_stuff<>(m, filename.c_str());
      m.ResizeClear(1,1);
      TEST(1 == tMemTrack<A>::Instances());
  
      /*
      Reload basic matrix.
      */
      restore_stuff<>(m, filename.c_str());
      TEST(rows * cols == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic matrix.
      */
      for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
          TEST(i == m[i][j].m_i);
          TEST(j == m[i][j].m_j);
        }
      }

      m.ResizeClear(1,1);
      TEST(1 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utMatrix_basic_serialization




  void UnitTests(bool full)
  {
    //if(full) utTemplateTests_hello_world::test();
    if(full) utMemTrack::test();
    if(full) utList_basic_serialization::test();
    if(full) utList_archive_structure::test();
    if(full) utArrays_of_pointers::test();
    if(full) utArray_basic_serialization::test();
    if(full) utHashTable_basic_serialization::test();
    if(full) utDictionary_basic_serialization::test();
    if(full) utBuffer_basic_serialization::test();
    if(full) utManagedPointerArray_basic_serialization::test();
    if(full) utSmartArray_basic_serialization::test();
    if(full) utMatrix_basic_serialization::test();
  }
} // nTemplateTests
