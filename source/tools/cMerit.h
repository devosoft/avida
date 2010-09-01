/*
 *  cMerit.h
 *  Avida
 *
 *  Called "merit.hh" prior to 12/7/05.
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

#ifndef cMerit_h
#define cMerit_h

#include <iostream>
#include <cmath>
#include <climits>
#include <cassert>

class cWorld;

class cMerit
{
protected:
  int bits;
  unsigned int base;
  int offset;
  double value;

  void UpdateValue(double in_value);

public:
  cMerit() : bits(0), base(0), offset(0), value(0) { ; }

  explicit cMerit(const int    in_value){ UpdateValue(in_value); }
  explicit cMerit(const unsigned int   in_value){ UpdateValue(in_value); }
  explicit cMerit(const double in_value){ UpdateValue(in_value); }
  cMerit(const cMerit& merit) { *this = merit; }

  bool OK() const;

  void operator=(const cMerit & _merit)
  {
    bits   = _merit.bits;
    base   = _merit.base;
    offset = _merit.offset;
    value  = _merit.value;
  }

  void operator=(double _merit) { UpdateValue(_merit); }
  
  void operator+=(const cMerit & _m) { UpdateValue(value + _m.GetDouble()); }
  void operator+=(double _merit) { UpdateValue(value + _merit); }
  
  cMerit operator*(const cMerit& _m) const { return cMerit(value * _m.GetDouble()); }
  void operator*=(const cMerit& _m) { UpdateValue(value * _m.GetDouble()); }

  int  operator>(const cMerit& _m)  const { return value >  _m.GetDouble(); }
  int  operator<(const cMerit& _m)  const { return value <  _m.GetDouble(); }
  int  operator>=(const cMerit& _m) const { return value >= _m.GetDouble(); }
  int  operator<=(const cMerit& _m) const { return value <= _m.GetDouble(); }

  int  operator==(const cMerit & _m) const { return value == _m.GetDouble(); }
  int  operator==(const double _m) const { return value == _m; }
  int  operator==(const unsigned int _m)   const { return (offset==0 && base==_m); }

  int  operator!=(const cMerit & _m) const { return value != _m.GetDouble(); }
  int  operator!=(const double _m) const { return value != _m; }
  int  operator!=(const unsigned int _m)   const { return (offset!=0 || base!=_m); }

  void Clear() { value = 0; base = 0; offset = 0; bits = 0; }

  // @TCC - This function fails for values > UINT_MAX...
  unsigned int GetUInt()   const {
    assert(value < UINT_MAX);  // Fails for merit values > UINT_MAX.
    return (unsigned int) value; }

  double GetDouble()      const { return value; }

  int GetBit(int bit_num)  const {
    assert(bit_num >= 0);
    return ( bit_num >= offset && bit_num < bits ) ?
      ( base >> (bit_num-offset) ) & 1 : 0; }

  int GetNumBits() const { return bits; }

  double CalcFitness(int gestation_time) const { return ( gestation_time != 0 ) ? value / ((double) gestation_time) : 0; }

  std::ostream& BinaryPrint(std::ostream& os = std::cout) const;
};


std::ostream& operator<<(std::ostream& os, const cMerit & merit);

#endif
