//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef RUNNING_AVERAGE_HH
#include "running_average.hh"
#endif

#include <assert.h>

// cRunningAverage ////////////////////////////////////////////////////////////

cRunningAverage::cRunningAverage( int window_size ) : 
  m_values(0), m_s1(0), m_s2(0), m_window_size( window_size ),
  m_pointer(0), m_n(0) 
{
  assert( m_window_size > 1 );
  m_values = new double[ m_window_size ];
}


cRunningAverage::~cRunningAverage() {
  delete [] m_values;
}


void
cRunningAverage::Add( double value ) {
  m_s1 += value;
  m_s2 += value*value;
  if ( m_n < m_window_size ) {
    m_values[ m_n ] = value;
    m_n += 1;
  }
  else{
    double out_v = m_values[ m_pointer ];
    m_s1 -= out_v;
    m_s2 -= out_v * out_v;
    m_values[ m_pointer++ ] = value;
    if ( m_pointer == m_window_size ) m_pointer = 0;
  }
}


void
cRunningAverage::Clear() {
  m_s1 = 0;
  m_s2 = 0;
  m_pointer = 0;
  m_n = 0;
}
