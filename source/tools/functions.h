/*
 *  functions.h
 *  Avida
 *
 *  Called "functions.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef functions_h
#define functions_h

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef NULL
#define NULL 0
#endif

inline int Min(int in1, int in2);
inline int Max(int in1, int in2);
inline int Range(int value, int min, int max);
inline int Mod(int value, int base);
inline int Pow(int bas, int expon);
inline double Log(double value);


namespace nFunctions {

  inline void Swap(int & in1, int & in2)
  {
    const int tmp = in1;
    in1 = in2;
    in2 = tmp;
  }
  
  inline void Swap(double & in1, double & in2)
  {
    const double tmp = in1;
    in1 = in2;
    in2 = tmp;
  }
  
}

inline bool ToggleBool(bool & in_bool)
{
  if (in_bool == true) in_bool = false;
  else in_bool = true;
  return in_bool;
}

inline int Min(int in1, int in2)
{
  return (in1 > in2) ? in2 : in1;
}

inline int Max(int in1, int in2)
{
  return (in1 < in2) ? in2 : in1;
}

inline double Max(double in1, double in2)
{
  return (in1 < in2) ? in2 : in1;
}

inline int Range(int value, int min, int max)
{
  value = (value < min) ? min : value;
  return (value > max) ? max : value;
}

inline int Mod(int value, int base)
{
  value %= base;
  if (value < 0) value += base;
  return value;
}

// Positive integer exponentiation
// O(log2(p))
inline int Pow(int base, int p)
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

inline double Log(double value)
{
  assert(value > 0);
  return log(value);
}

/*
 *   Special array functions...
 */

inline int Min(const tArray<int> & test_array)
{
  assert(test_array.GetSize() > 0);

  int min_val = test_array[0];
  for (int i = 1; i < test_array.GetSize(); i++) {
    if (test_array[i] < min_val) min_val = test_array[i];
  }
  return min_val;
}

inline int Max(const tArray<int> & test_array)
{
  assert(test_array.GetSize() > 0);

  int max_val = test_array[0];
  for (int i = 1; i < test_array.GetSize(); i++) {
    if (test_array[i] > max_val) max_val = test_array[i];
  }
  return max_val;
}

inline int Sum(const tArray<int> & test_array)
{
  int sum = 0;
  for (int i = 0; i < test_array.GetSize(); i++) {
    sum += test_array[i];
  }
  return sum;
}

inline int GridNeighbor(int cell_id, int size_x, int size_y, int diff_x, 
                    int diff_y)
{
  // Routine to find a neighboring cell in an array that is
  // pretending to be a matrix

  int new_x = Mod(((cell_id % size_x) + diff_x), size_x);
  int new_y = Mod(((cell_id / size_x) + diff_y), size_y);
  return (new_y * size_x) + new_x;
}

// For QSORT in Divide_DoMutations ... MOVE THIS --@TCC
inline int IntCompareFunction(const void * a, const void * b)
{
  if( *((int*)a) > *((int*)b) ) return 1;
  if( *((int*)a) < *((int*)b) ) return -1;
  return 0;
}

#endif
