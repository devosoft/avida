/*
 *  cRunningAverage.h
 *  Avida
 *
 *  Called "running_average.hh" prior to 12/7/05.
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

#ifndef cRunningAverage_h
#define cRunningAverage_h

#include <cmath>

class cRunningAverage
{
private:
  double* m_values;  // Array of actual values
  double m_s1;       // average
  double m_s2;       // sum of squares
  int m_window_size;        // Size of sliding window
  int m_pointer;
  int m_n;
  
  
  cRunningAverage(); // @not_implemented
  cRunningAverage(const cRunningAverage&); // @not_implemented
  cRunningAverage& operator=(const cRunningAverage&); // @not_implemented
  
public:
  cRunningAverage(int window_size);
  ~cRunningAverage();
  
  
  //manipulators
  void Add(double value);
  void Clear();
  
  
  //accessors
  double Sum()          const { return m_s1; }
  double S1()           const { return m_s1; }
  double SumOfSquares() const { return m_s2; }
  double S2()           const { return m_s2; }
  
  double Average() const { return ( m_n == m_window_size ) ? (m_s1 / m_n) : 0; }
  double Variance() const { return ( m_n == m_window_size ) ? (m_s2 - m_s1 * m_s1 / m_n) / (m_n - 1) : 0; }
    
  double StdDeviation() const { return sqrt(Variance()); }
  double StdError()  const
  {
    return ( m_n == m_window_size ) ? sqrt(m_s2 - m_s1 * m_s1 / m_n / (m_n * (m_n - 1))) : 0;
  }

  // Notation Shortcuts
  double Ave() const { return Average(); }
  double Var() const { return Variance(); }
};

#endif
