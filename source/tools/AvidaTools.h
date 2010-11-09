/*
 *  AvidaTools.h
 *  Avida
 *
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

#ifndef AvidaTools_h
#define AvidaTools_h

#include <cassert>
#include <cmath>
#include "cString.h"


namespace AvidaTools
{
  namespace FileSystem {
    bool MkDir(const cString& dirname, bool verbose = false);
    cString GetCWD();
    cString GetAbsolutePath(const cString& path, const cString& working_dir = GetCWD());
    inline cString PathAppend(const cString& path, const cString& path_add);
  };
  
  // Utility Functions
  template <typename T> inline T Min(T op1, T op2) { return (op1 > op2) ? op2 : op1; }
  template <typename T> inline T Max(T op1, T op2) { return (op1 > op2) ? op1 : op2; }
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

inline cString AvidaTools::FileSystem::PathAppend(const cString& path, const cString& path_add)
{
  return cString(path) + "/" + path_add;
}

#endif
