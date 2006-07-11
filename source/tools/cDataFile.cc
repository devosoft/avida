/*
 *  cDataFile.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cDataFile.h"

#include "cStringUtil.h"

extern "C" {
#include <stdio.h>
#include <time.h>
}

using namespace std;


cDataFile::cDataFile(cString& name) : m_name(name), num_cols(0), m_descr_written(false)
{
  m_fp.open(name);
  assert(m_fp.good());
  m_descr_written = false;
}


void cDataFile::Write( double x, const char * descr )
{
  if ( !m_descr_written ){
    m_data += cStringUtil::Stringf( "%f ", x );
    WriteColumnDesc(descr);
  }
  else m_fp << x << " ";
}


void cDataFile::Write( int i, const char * descr )
{
  if ( !m_descr_written ){
    m_data += cStringUtil::Stringf( "%i ", i );
    WriteColumnDesc(descr);
  }
  else m_fp << i << " ";
}


void cDataFile::Write( const char * data_str, const char * descr )
{
  if ( !m_descr_written ) {
    m_data += cStringUtil::Stringf( "%s ", data_str );
    WriteColumnDesc(descr);
  }
  else m_fp << data_str << " ";
}


void cDataFile::WriteBlockElement(double x, int element, int x_size)
{
  m_fp << x << " ";
  if (((element + 1) % x_size) == 0) {
    m_fp << "\n";
  }
}

void cDataFile::WriteBlockElement(int i, int element, int x_size)
{
  m_fp << i << " ";
  if (((element + 1) % x_size) == 0) {
    m_fp << "\n";
  }
}

void cDataFile::WriteColumnDesc( const char * descr )
{
  if ( !m_descr_written ){
    num_cols++;
    m_descr += cStringUtil::Stringf( "# %2d: %s\n", num_cols, descr );
  }
}

void cDataFile::WriteComment( const char * descr )
{
  if ( !m_descr_written ) m_descr += cStringUtil::Stringf( "# %s\n", descr );
}


void cDataFile::WriteRawComment( const char * descr )
{
  if ( !m_descr_written ) m_descr += cStringUtil::Stringf( "%s\n", descr );
}

void cDataFile::WriteRaw( const char * descr )
{
  m_fp << cStringUtil::Stringf( "%s\n", descr );
}




void cDataFile::WriteTimeStamp()
{
  if ( !m_descr_written ){
    time_t time_p = time( 0 );
    m_descr += cStringUtil::Stringf( "# %s", ctime( &time_p ) );
  }
}

void cDataFile::FlushComments()
{
  if ( !m_descr_written ){
    m_fp << m_descr;
    m_descr_written = true;
  }
}


void cDataFile::Endl()
{
  if ( !m_descr_written ){
    m_fp << m_descr << endl;
    m_fp << m_data << endl;
    m_descr_written = true;
  }
  else m_fp << endl;
}


#ifdef ENABLE_UNIT_TESTS

/*
Unit tests
*/
#include "cFile.h"
#include "cXMLArchive.h"

#include "lightweight_test.h"

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream> 
#include <string>

namespace nDataFile {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cDataFile_Archive", s);
  }
  
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cDataFile_Archive", s);
  }
  

  namespace utDataFile_hello_world {
    void test(){
      utDataFile_hello_world::test();
      TEST(true);
      TEST(false);
    }
  }

  namespace utDataFile_archiving {
    void test(){
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      std::string filename("./cDataFile_basic_serialization.xml");
      int linecount = 3;
      std::string data_file_name("./cDataFile_data.txt");
      cString dfn(data_file_name.c_str());

      /*
      Create a data file object, write two lines of data, archive the
      data file object ... 
      */
      {
        cDataFile df(dfn);
        df.WriteComment("Comment!");
        df.Write(1, "descr1");
        df.Write(1.0, "descr2");
        df.Write("blah", "descr3");
        df.Endl();
        df.Write(2, "descr1");
        df.Write(2.0, "descr2");
        df.Write("blahblah", "descr3");
        df.Endl();
        save_stuff<>(df, filename.c_str());
      }
      /*
      Sanity-check contents of file on disk.
      */
      {
        cFile f(dfn);
        TEST(f.Good());

        cString l[7];
        for(int i = 0; i<7; i++){
          f.ReadLine(l[i]);
          //std::cout << "\"" << l[i] << "\"" << std::endl;
        }
        TEST(cString("# Comment!") == l[0]);
        TEST(cString("#  1: descr1") == l[1]);
        TEST(cString("#  2: descr2") == l[2]);
        TEST(cString("#  3: descr3") == l[3]);
        TEST(cString("") == l[4]);
        TEST(cString("1 1.000000 blah ") == l[5]);
        TEST(cString("2 2 blahblah ") == l[6]);
      }
      /*
      Reload the data file object, write another line of data ...
      */
      {
        cDataFile df;
        restore_stuff<>(df, filename.c_str());
        df.Write(3, "descr1");
        df.Write(3.0, "descr2");
        df.Write("blahblahblah", "descr3");
      }
      /*
      Verify that a new line was appended to the file on disk.
      */
      {
        cFile f(dfn);
        TEST(f.Good());

        cString l[8];
        for(int i = 0; i<8; i++){
          f.ReadLine(l[i]);
          //std::cout << "\"" << l[i] << "\"" << std::endl;
        }
        TEST(cString("# Comment!") == l[0]);
        TEST(cString("#  1: descr1") == l[1]);
        TEST(cString("#  2: descr2") == l[2]);
        TEST(cString("#  3: descr3") == l[3]);
        TEST(cString("") == l[4]);
        TEST(cString("1 1.000000 blah ") == l[5]);
        TEST(cString("2 2 blahblah ") == l[6]);
        TEST(cString("3 3 blahblahblah ") == l[7]);
      }

      /*
      Create a new data file object with the same name. This should
      overwrite the original file on disk.
      */
      {
        cDataFile df(dfn);
        df.WriteComment("This should be a new file.");
        df.Write(1, "newdescr");
        df.Endl();
      }
      /*
      Verify that original file on disk has been overwritten.
      */
      {
        cFile f(dfn);
        TEST(f.Good());

        cString l[4];
        for(int i = 0; i<4; i++){
          f.ReadLine(l[i]);
          //std::cout << "\"" << l[i] << "\"" << std::endl;
        }
        TEST(cString("# This should be a new file.") == l[0]);
        TEST(cString("#  1: newdescr") == l[1]);
        TEST(cString("") == l[2]);
        TEST(cString("1 ") == l[3]);
      }

      std::remove(filename.c_str());
      std::remove(data_file_name.c_str());
#   endif // ENABLE_SERIALIZATION
    }
  } // utDataFile_archiving


  void UnitTests(bool full)
  {
    //if(full) utDataFile_hello_world::test();
    if(full) utDataFile_archiving::test();
  }
} // nDataFile

#endif // ENABLE_UNIT_TESTS
