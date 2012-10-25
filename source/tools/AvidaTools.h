/*
 *  AvidaTools.h
 *  Avida
 *
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

#ifndef AvidaTools_h
#define AvidaTools_h

#include <cassert>
#include <cmath>


namespace AvidaTools
{  
  // Utility Functions
  template <typename T> inline T Range(T value, T min, T max);
  
  inline int Abs(int val) { return (val < 0) ? -val : val; }
  inline double Abs(double val) { return (val < 0.0) ? -val : val; }
  
  inline int Mod(int value, int base) { value %= base; return (value < 0) ? (value + base) : value; }
  inline int Pow(int bas, int expon);
  inline double Log(double value) { assert(value > 0.0); return log(value); }

  template <typename T> inline void Swap(T& op1, T& op2) { const T tmp = op1; op1 = op2; op2 = tmp; }


  inline int GridNeighbor(int cell_id, int size_x, int size_y, int diff_x, int diff_y);
};


template <typename T> inline T AvidaTools::Range(T value, T min, T max)
{
  value = (value < min) ? min : value;
  return (value > max) ? max : value;
}

// Positive integer exponentiation
// O(log2(p))
inline int AvidaTools::Pow(int base, int p)
{
  if (p == 0) return 1;
  if (p == 1) return base;
  if (p < 0) return 0;
  
  int r = 1;
  while (1) {
    if (p & 1) r *= base;
    if (!(p >>= 1)) return r;
    base *= base;
  }
}


inline int AvidaTools::GridNeighbor(int cell_id, int size_x, int size_y, int diff_x, int diff_y)
{
  // Routine to find a neighboring cell in an array that is
  // pretending to be a matrix
  
  int new_x = Mod(((cell_id % size_x) + diff_x), size_x);
  int new_y = Mod(((cell_id / size_x) + diff_y), size_y);
  return (new_y * size_x) + new_x;
}

#endif
