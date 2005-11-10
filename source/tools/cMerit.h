//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MERIT_HH
#define MERIT_HH

#include <iostream>
#include <math.h>
#include <limits.h>
#include <assert.h>

#ifndef UINT
#define UINT unsigned int
#endif

class cMerit {
protected:
  int bits;
  UINT base;
  UINT offset;
  double value;

  void UpdateValue(double in_value);

public:
  cMerit() : bits(0), base(0), offset(0), value(0) {;}

  explicit cMerit(const int    in_value){ UpdateValue(in_value); }
  explicit cMerit(const UINT   in_value){ UpdateValue(in_value); }
  explicit cMerit(const double in_value){ UpdateValue(in_value); }

  bool OK() const ;

  void operator=(const cMerit & _merit){
    bits   = _merit.bits;
    base   = _merit.base;
    offset = _merit.offset;
    value  = _merit.value; }

  void operator=(double _merit){ UpdateValue(_merit); }
  void operator+=(const cMerit & _m){ UpdateValue(value + _m.GetDouble()); }

  int  operator>(const cMerit & _m)  const { return value >  _m.GetDouble(); }
  int  operator<(const cMerit & _m)  const { return value <  _m.GetDouble(); }
  int  operator>=(const cMerit & _m) const { return value >= _m.GetDouble(); }
  int  operator<=(const cMerit & _m) const { return value <= _m.GetDouble(); }

  int  operator==(const cMerit & _m) const { return value == _m.GetDouble(); }
  int  operator==(const double _m) const { return value == _m; }
  int  operator==(const UINT _m)   const { return (offset==0 && base==_m); }

  int  operator!=(const cMerit & _m) const { return value != _m.GetDouble(); }
  int  operator!=(const double _m) const { return value != _m; }
  int  operator!=(const UINT _m)   const { return (offset!=0 || base!=_m); }

  void Clear() { value = 0; base = 0; offset = 0; bits = 0; }

  // @TCC - This function fails for values > UINT_MAX...
  UINT GetUInt()   const {
    assert(value < UINT_MAX);  // Fails for merit values > UINT_MAX.
    return (UINT) value; }

  double GetDouble()      const { return value; }

  int GetBit(UINT bit_num)  const {
    return ( bit_num >= offset && bit_num < (UINT)bits ) ?
			( base >> (bit_num-offset) ) & 1 : 0; }

  int GetNumBits() const { return bits; }

  double CalcFitness(int gestation_time) const {
    return ( gestation_time != 0 ) ? value / ((double) gestation_time) : 0; }

  std::ostream& BinaryPrint(std::ostream& os = std::cout) const ;
};

std::ostream& operator<<(std::ostream& os, const cMerit & merit);

#endif
