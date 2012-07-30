/*
 *  cDoubleSum.h
 *  Avida
 *
 *  Called "double_sum.hh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cDoubleSum_h
#define cDoubleSum_h

#include <cmath>
#include <climits>
#include <limits>

class cDoubleSum {
private:
  double s1;  // Sum (x)
  double s2;  // Sum of squared x (x^2)
  double n;
  double max;

public:
  cDoubleSum() { Clear(); }

  void Clear() { s1 = s2 = n = 0; max = std::numeric_limits<double>::min();}

  double Count()        const { return n; }
  double N()            const { return n; }
  double Sum()          const { return s1; }
  double Max()          const { return max; }

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
    if (value > max) max = value;
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
