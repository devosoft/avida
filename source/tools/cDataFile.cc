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


void cDataFile::Init()
{
  m_descr_written = false;
  num_cols = 0;
}

cDataFile::cDataFile()
{
  Init();
}

cDataFile::cDataFile(cString name) : m_name(name)
{
  Init();
   
  //if (fopen(name,"r")) {
    // cout << "File " << name() << " exists and is being overwritten" << endl;
  //}
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
