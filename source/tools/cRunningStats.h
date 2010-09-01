/*
 *  cRunningStats.h
 *  Avida
 *
 *  Created by David on 10/21/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#ifndef cRunningStats_h
#define cRunningStats_h

#include <cmath>


class cRunningStats
{
private:
  double m_n;  // count
  double m_m1; // mean
  double m_m2; // second moment
  double m_m3; // third moment
  double m_m4; // fourth moment
  
public:
  inline cRunningStats() : m_n(0.0), m_m1(0.0), m_m2(0.0), m_m3(0.0), m_m4(0.0) { ; }

  inline void Clear() { m_n = 0.0; m_m1 = 0.0; m_m2 = 0.0; m_m3 = 0.0; m_m4 = 0.0; }
  
  inline void Push(double x);

  inline double N() const { return m_n; }
  inline double Mean() const { return m_m1; }
  inline double StdDeviation() const { return sqrt(Variance()); }
  inline double StdError() const { return (m_n > 1.0) ? sqrt(Variance() / m_n) : 0.0; }
  inline double Variance() const { return (m_n > 1.0) ? (m_m2 / (m_n - 1.0)) : 0.0; }
  inline double Skewness() const { return sqrt(m_n) * m_m3 / pow(m_m2, 1.5); }
  inline double Kurtosis() const { return m_n * m_m4 / (m_m2 * m_m2); }
};


inline void cRunningStats::Push(double x)
{
  m_n++;
  double d = (x - m_m1);
  double d_n = d / m_n;
  double d_n2 = d_n * d_n;
  
  m_m4 += d * d_n2 * d_n * ((m_n - 1) * ((m_n * m_n) - 3 * m_n + 3)) + 6 * d_n2 * m_m2 - 4 * d_n * m_m3;
  m_m3 += d * d_n2 * ((m_n - 1) * (m_n - 2)) - 3 * d_n * m_m2;
  m_m2 += d * d_n * (m_n - 1);
  m_m1 += d_n;
}

#endif
