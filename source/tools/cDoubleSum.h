/*
 *  cDoubleSum.h
 *  Avida
 *
 *  Called "double_sum.hh" prior to 12/7/05.
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

#ifndef cDoubleSum_h
#define cDoubleSum_h

#include <cmath>
#include <climits>

class cDoubleSum {
private:
  double s1;  // Sum (x)
  double s2;  // Sum of squared x (x^2)
  double n;

public:
  cDoubleSum() { Clear(); }

  void Clear() { s1 = s2 = n = 0; }

  double Count()        const { return n; }
  double N()            const { return n; }
  double Sum()          const { return s1; }

  double Average() const { return (n > 0.0) ? (s1 / n) : 0.0; }
  double Variance() const { return (n > 1.0) ? (s2 - s1 * s1 / n) / (n - 1.0) : 0.0; }
  double StdDeviation() const { return sqrt(Variance()); }
  double StdError()  const { return (n > 1) ? sqrt(Variance() / n) : 0.0; }
  
  // Notation Shortcuts
  double Ave() const { return Average(); }
  double Var() const { return Variance(); }

  void Add(double value, double weight = 1.0)
  {
    double w_val = value * weight;
    n += weight;
    s1 += w_val;
    s2 += w_val * w_val;
  }

  void Subtract(double value, double weight = 1.0)
  {
    double w_val = value * weight;
    n -= weight;
    s1 -= w_val;
    s2 -= w_val * w_val;
  }
};

#endif
