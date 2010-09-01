/*
 *  cRunningAverage.cc
 *  Avida
 *
 *  Called "running_average.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cRunningAverage.h"

#include <cassert>


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


void cRunningAverage::Add( double value ) {
  m_s1 += value;
  m_s2 += value*value;
  if ( m_n < m_window_size ) {
    m_values[ m_n ] = value;
    m_n += 1;
  } else {
    double out_v = m_values[ m_pointer ];
    m_s1 -= out_v;
    m_s2 -= out_v * out_v;
    m_values[ m_pointer++ ] = value;
    if ( m_pointer == m_window_size ) m_pointer = 0;
  }
}


void cRunningAverage::Clear() {
  m_s1 = 0;
  m_s2 = 0;
  m_pointer = 0;
  m_n = 0;
}
