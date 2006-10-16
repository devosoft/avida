/*
 *  cMerit.h
 *  Avida
 *
 *  Called "merit.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cMerit_h
#define cMerit_h

#include <iostream>
#include <math.h>
#include <limits.h>
#include <assert.h>

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
  void operator+=(const cMerit & _m){ UpdateValue(value + _m.GetDouble()); }

  int  operator>(const cMerit & _m)  const { return value >  _m.GetDouble(); }
  int  operator<(const cMerit & _m)  const { return value <  _m.GetDouble(); }
  int  operator>=(const cMerit & _m) const { return value >= _m.GetDouble(); }
  int  operator<=(const cMerit & _m) const { return value <= _m.GetDouble(); }

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

  double CalcFitness(int gestation_time) const {
    return ( gestation_time != 0 ) ? value / ((double) gestation_time) : 0; }

  std::ostream& BinaryPrint(std::ostream& os = std::cout) const ;
};


#ifdef ENABLE_UNIT_TESTS
namespace nMerit {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

std::ostream& operator<<(std::ostream& os, const cMerit & merit);

#endif
