/*
 *  cIntSum.h
 *  Avida
 *
 *  Called "int_sum.hh" prior to 12/7/05.
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

#ifndef cIntSum_h
#define cIntSum_h

#include <cmath>
#include <climits>

class cIntSum
{
private:
  long s1;  // Sum (x)
  long s2;  // Sum of squares (x^2)
  long s3;  // Sum of cubes (x^3)
  long s4;  // Sum of x^4
  long n;

public:
  static const double INF_ERR;  // Value Returned by StdError if Infinate

  cIntSum() { Clear(); }

  void Clear() { s1 = s2 = s3 = s4 = n = 0; }

  long Count()        const { return n; }
  long N()            const { return n; }
  long Sum()          const { return s1; }
  long S1()           const { return s1; }
  long SumOfSquares() const { return s2; }
  long S2()           const { return s2; }
  long SumOfCubes()   const { return s3; }
  long S3()           const { return s3; }
  long S4()           const { return s4; }

  double Average() const { return (n > 1) ? ((double)s1/n) : 0; }

  double Variance() const { return (n > 1) ? (s2 - s1*s1/(double)n) / (double)(n-1) : INF_ERR; }
  double StdDeviation() const { return sqrt(Variance()); }
  double StdError()  const { return (n > 1) ? sqrt(s2 / (double)(n * (n-1))) : INF_ERR; }

  double Skewness() const { return (n > 2) ? (n*s3 - 3*s2*s1 + 2*s1*s1*s1/(double)n) / (double)((n-1)*(n-2)) : INF_ERR; }
  double Kurtosis() const
  {
    return (n > 3) ? (n+1)*(n*s4 - 4*s3*s1 + 6*s2*s1*s1/(double)n - 3*s1*s1*s1/(double)n*s1/(double)n) / (double)((n-1)*(n-2)*(n-3)) : INF_ERR;
  }

  // Notation Shortcuts
  double Ave() const { return Average(); }
  double Var() const { return Variance(); }
  double Kur() const { return Kurtosis(); }
  double Skw() const { return Skewness(); }


  void Add(long value, long weight = 1)
  {
    long w_val = value * weight;
    n += weight;
    s1 += w_val;
    s2 += w_val * w_val;
    s3 += w_val * w_val * w_val;
    s4 += w_val * w_val * w_val * w_val;
  }

  void Subtract(long value, long weight=1)
  {
    long w_val = value * weight;
    n -= weight;
    s1 -= w_val;
    s2 -= w_val * w_val;
    s3 -= w_val * w_val * w_val;
    s4 -= w_val * w_val * w_val * w_val;
  }
};

#endif
