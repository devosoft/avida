/*
 *  cRunningAverage.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cRunningAverage_h
#define cRunningAverage_h

#include <math.h>

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


#ifdef ENABLE_UNIT_TESTS
namespace nRunningAverage {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
