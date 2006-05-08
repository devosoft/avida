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

#include <boost/detail/lightweight_test.hpp>

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
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }
  

  namespace utMemTrack {
    void test(){
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      {
        A a;
        BOOST_TEST(1 == tMemTrack<A>::Instances());
      }
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      A *p;
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      p = new A();
      BOOST_TEST(1 == tMemTrack<A>::Instances());
      delete p;
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      
    }
  }
  
  namespace utList_basic_serialization {
    void test(){
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      A *a;
      tList<A> l;
      tListIterator<A> it(l);
      std::string filename("./tList_basic_serialization.xml");
      int i, listsize = 10;
      
      BOOST_TEST(1 == tMemTrack<tList<A> >::Instances());
      BOOST_TEST(1 == tMemTrack<tListNode<A> >::Instances());
      
      /*
      Construct basic list.
      */
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      for(i=0; i<listsize; i++){
        a = new A;
        a->m_i = i; 
        l.PushRear(a);
      }
      BOOST_TEST(listsize == tMemTrack<A>::Instances());
      BOOST_TEST(1 + listsize == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Verify basic list contents.
      */
      BOOST_TEST(listsize == tMemTrack<A>::Instances());
      it.Reset();
      for(i=0; (a = it.Next()); i++){
        BOOST_TEST(i == a->m_i);
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
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      BOOST_TEST(1 == tMemTrack<tList<A> >::Instances());
      BOOST_TEST(1 == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Reload basic list.
      */
      restore_stuff<>(l, filename.c_str());
      BOOST_TEST(1 == tMemTrack<tList<A> >::Instances());
      BOOST_TEST(listsize == tMemTrack<A>::Instances());
      BOOST_TEST(1 + listsize == tMemTrack<tListNode<A> >::Instances());
  
      /*
      Verify contents of reloaded basic list.
      */
      for(i=0; i<listsize; i++){
        a = l.Pop();
        BOOST_TEST(i == a->m_i);
        delete a;
      }
  
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      BOOST_TEST(1 == tMemTrack<tList<A> >::Instances());
      BOOST_TEST(1 == tMemTrack<tListNode<A> >::Instances());
  
      std::remove(filename.c_str());
    }
  } // utList_basic_serialization
  
  namespace utList_archive_structure {
    void test(){
      std::string filename("./tList_archive_structure.xml");
      
      A *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10;
      
      tList<A> l;
      tListIterator<A> it(l);
      
      /*
      Construct list...
      */
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      a1 = new A;
      a2 = new A;
      BOOST_TEST(2 == tMemTrack<A>::Instances());
      l.PushRear(a1); 
      l.PushRear(a2);
      l.PushRear(a1);
      l.PushRear(a2);
      BOOST_TEST(2 == tMemTrack<A>::Instances());
      
      /*
      Verify list structure. It should now contain two copies each,
      interleaved, of two instances of A.
      */
      it.Reset();
      a3 = it.Next();
      a4 = it.Next();
      a5 = it.Next();
      a6 = it.Next();
      BOOST_TEST(2 == tMemTrack<A>::Instances());
  
      a1->m_i = 3;
      a2->m_i = 5;
      BOOST_TEST(3 == a3->m_i);
      BOOST_TEST(5 == a4->m_i);
      BOOST_TEST(3 == a5->m_i);
      BOOST_TEST(5 == a6->m_i);
  
      /*
      Save to disk, clear, and reload.
      */
      save_stuff<>(l, filename.c_str());
      BOOST_TEST(2 == tMemTrack<A>::Instances());
  
      l.Clear();
      BOOST_TEST(2 == tMemTrack<A>::Instances());
  
      restore_stuff<>(l, filename.c_str());
  
      /*
      New list should contain two copies each, interleaved, of the two
      new instances of A.
      */
      BOOST_TEST(4 == tMemTrack<A>::Instances());
  
      /*
      Verify new list structure.
      */
      it.Reset();
      a7 = it.Next();
      a8 = it.Next();
      a9 = it.Next();
      a10 = it.Next();
      BOOST_TEST(4 == tMemTrack<A>::Instances());
  
      BOOST_TEST(3 == a7->m_i);
      BOOST_TEST(5 == a8->m_i);
      BOOST_TEST(3 == a9->m_i);
      BOOST_TEST(5 == a10->m_i);
  
      /*
      Verify that elements of old and new lists are independent.
      */
      a1->m_i = 7;
      a7->m_i = 11;
      a8->m_i = 13;
      BOOST_TEST(7 == a3->m_i);
      BOOST_TEST(5 == a4->m_i);
      BOOST_TEST(7 == a5->m_i);
      BOOST_TEST(5 == a6->m_i);
      BOOST_TEST(11 == a7->m_i);
      BOOST_TEST(13 == a8->m_i);
      BOOST_TEST(11 == a9->m_i);
      BOOST_TEST(13 == a10->m_i);
  
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
      BOOST_TEST(0 == tMemTrack<A>::Instances());
  
      std::remove(filename.c_str());
    }
  } // utList_archive_structure

  namespace utArray_basic_serialization {
    void test(){
      std::string filename("./tArray_basic_serialization.xml");
      int i, arraysize = 10;

      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tArray<A> l(arraysize);
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());
       
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
        BOOST_TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        BOOST_TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utArray_basic_serialization
  
  namespace utHashTable_basic_serialization {
    void test(){
      std::string filename("./tHashTable_basic_serialization.xml");

      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tHashTable<cString, A> d;
      char keystr[9] = "number 0";
      int i, dictsize = 10;
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      // Sanity checks.
      BOOST_TEST(d.OK());
      BOOST_TEST(0 == d.GetSize());
      BOOST_TEST(!d.HasEntry("blah"));
      {
        A a;
        BOOST_TEST(!d.Find("blah", a));
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
      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Verify basic dict contents.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        BOOST_TEST(d.Find(cString(keystr), a));
        BOOST_TEST(i == a.m_i);
      }
      BOOST_TEST(d.OK());

      /*
      Save basic table.
      */
      save_stuff<>(d, filename.c_str());
      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Reload basic table.
      */
      {
        tHashTable<cString, A> d2;
        restore_stuff<>(d2, filename.c_str());
        BOOST_TEST(2 * dictsize == tMemTrack<A>::Instances());
        BOOST_TEST(d2.OK());

        /*
        Verify contents of reloaded basic dict.
        */
        for(i=0; i<dictsize; i++){
          A a;
          keystr[7] = '0' + i;
          BOOST_TEST(d2.Find(cString(keystr), a));
          BOOST_TEST(i == a.m_i);
        }
        BOOST_TEST(2 * dictsize == tMemTrack<A>::Instances());
      }

      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utHashTable_basic_serialization

  namespace utDictionary_basic_serialization {
    void test(){
      std::string filename("./tDictionary_basic_serialization.xml");

      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tDictionary<A> d;
      char keystr[9] = "number 0";
      int i, dictsize = 10;
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      // Sanity checks.
      BOOST_TEST(d.OK());
      BOOST_TEST(0 == d.GetSize());
      BOOST_TEST(!d.HasEntry("blah"));
      {
        A a;
        BOOST_TEST(!d.Find("blah", a));
      }
      BOOST_TEST(cString("") == d.NearMatch("blah"));

      /*
      Construct basic dict.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        a.m_i = i;
        d.Add(cString(keystr), a);
      }
      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Verify basic dict contents.
      */
      for(i=0; i<dictsize; i++){
        A a;
        keystr[7] = '0' + i;
        BOOST_TEST(d.Find(cString(keystr), a));
        BOOST_TEST(i == a.m_i);
      }
      BOOST_TEST(d.OK());

      /*
      Save basic list.
      */
      save_stuff<>(d, filename.c_str());
      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      /*
      Reload basic list.
      */
      {
        tDictionary<A> d2;
        restore_stuff<>(d2, filename.c_str());
        BOOST_TEST(2 * dictsize == tMemTrack<A>::Instances());
        BOOST_TEST(d2.OK());

        /*
        Verify contents of reloaded basic dict.
        */
        for(i=0; i<dictsize; i++){
          A a;
          keystr[7] = '0' + i;
          BOOST_TEST(d2.Find(cString(keystr), a));
          BOOST_TEST(i == a.m_i);
        }
        BOOST_TEST(2 * dictsize == tMemTrack<A>::Instances());
      }

      BOOST_TEST(dictsize == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utDictionary_basic_serialization

  namespace utBuffer_basic_serialization {
    void test(){
      std::string filename("./tBuffer_basic_serialization.xml");
      A a;
      int i, buffer_size = 10;

      BOOST_TEST(1 == tMemTrack<A>::Instances());
      tBuffer<A> b(buffer_size);
      BOOST_TEST(1 + buffer_size == tMemTrack<A>::Instances());
       
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
        BOOST_TEST(buffer_size - 2 - i == b[i].m_i);
      }
      BOOST_TEST(b.GetCapacity() == buffer_size);
      BOOST_TEST(b.GetTotal() == buffer_size - 1);
      BOOST_TEST(b.GetNumStored() == buffer_size - 1);
      BOOST_TEST(b.GetNum() == buffer_size - 1);
      
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
        BOOST_TEST(0 == b[i].m_i);
      }
      b.Clear();
      BOOST_TEST(b.GetCapacity() == buffer_size);
      BOOST_TEST(b.GetTotal() == 0);
      BOOST_TEST(b.GetNumStored() == 0);
      BOOST_TEST(b.GetNum() == 0);

      /*
      Reload basic buffer.
      */
      restore_stuff<>(b, filename.c_str());
      BOOST_TEST(1 + buffer_size == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic buffer.
      */
      for(i=0; i < buffer_size - 1; i++){
        BOOST_TEST(buffer_size - 2 - i == b[i].m_i);
      }
      BOOST_TEST(b.GetCapacity() == buffer_size);
      BOOST_TEST(b.GetTotal() == buffer_size - 1);
      BOOST_TEST(b.GetNumStored() == buffer_size - 1);
      BOOST_TEST(b.GetNum() == buffer_size - 1);

      std::remove(filename.c_str());
    }
  } // utBuffer_basic_serialization

  namespace utManagedPointerArray_basic_serialization {
    void test(){
      std::string filename("./tManagedPointerArray_basic_serialization.xml");
      int i, arraysize = 10;

      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tManagedPointerArray<A> l(arraysize);
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());
       
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
        BOOST_TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        BOOST_TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utManagedPointerArray_basic_serialization

  namespace utSmartArray_basic_serialization {
    void test(){
      std::string filename("./tSmartArray_basic_serialization.xml");
      int i, arraysize = 10;

      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tSmartArray<A> l(arraysize);
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());
       
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
        BOOST_TEST(i == l[i].m_i);
      }
      
      /* 
      Save basic array.
      */
      save_stuff<>(l, filename.c_str());
      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      /*
      Reload basic array.
      */
      restore_stuff<>(l, filename.c_str());
      BOOST_TEST(arraysize == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic array.
      */
      for(i=0; i<arraysize; i++){
        BOOST_TEST(i == l[i].m_i);
      }

      l.ResizeClear(0);
      BOOST_TEST(0 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utSmartArray_basic_serialization

  namespace utMatrix_basic_serialization {
    void test(){
      std::string filename("./tMatrix_basic_serialization.xml");
      int i, j, rows = 3, cols = 5;
      
      BOOST_TEST(0 == tMemTrack<A>::Instances());
      tMatrix<A> m(rows, cols);
      BOOST_TEST(rows * cols == tMemTrack<A>::Instances());
      
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
          BOOST_TEST(i == m[i][j].m_i); 
          BOOST_TEST(j == m[i][j].m_j);
        }
      } 
        
      /*
      Save basic matrix. 
      */
      save_stuff<>(m, filename.c_str());
      m.ResizeClear(1,1);
      BOOST_TEST(1 == tMemTrack<A>::Instances());
  
      /*
      Reload basic matrix.
      */
      restore_stuff<>(m, filename.c_str());
      BOOST_TEST(rows * cols == tMemTrack<A>::Instances());

      /*
      Verify contents of reloaded basic matrix.
      */
      for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
          BOOST_TEST(i == m[i][j].m_i);
          BOOST_TEST(j == m[i][j].m_j);
        }
      }

      m.ResizeClear(1,1);
      BOOST_TEST(1 == tMemTrack<A>::Instances());

      std::remove(filename.c_str());
    }
  } // utMatrix_basic_serialization




  void UnitTests(bool full)
  {
    //if(full) {
    //  std::cout << "utTemplateTests_hello_world" << std::endl;
    //  utTemplateTests_hello_world::test();
    //}
    if(full) {
      std::cout << "utMemTrack" << std::endl;
      utMemTrack::test();
    }
    if(full) {
      std::cout << "utList_basic_serialization" << std::endl;
      utList_basic_serialization::test();
    }
    if(full) {
      std::cout << "utList_archive_structure" << std::endl;
      utList_archive_structure::test();
    }
    if(full) {
      std::cout << "utArray_basic_serialization" << std::endl;
      utArray_basic_serialization::test();
    }
    if(full) {
      std::cout << "utHashTable_basic_serialization" << std::endl;
      utHashTable_basic_serialization::test();
    }
    if(full) {
      std::cout << "utDictionary_basic_serialization" << std::endl;
      utDictionary_basic_serialization::test();
    }
    if(full) {
      std::cout << "utBuffer_basic_serialization" << std::endl;
      utBuffer_basic_serialization::test();
    }
    if(full) {
      std::cout << "utManagedPointerArray_basic_serialization" << std::endl;
      utManagedPointerArray_basic_serialization::test();
    }
    if(full) {
      std::cout << "utSmartArray_basic_serialization" << std::endl;
      utSmartArray_basic_serialization::test();
    }
    if(full) {
      std::cout << "utMatrix_basic_serialization" << std::endl;
      utMatrix_basic_serialization::test();
    }
  }
} // nTemplateTests
