#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <assert.h>

#include <iostream>

#include "tArray.h"

using namespace std;

// Class: cBitArray and cBitMatrix
// Desc: These classes handle an arbitrarily large array or matrix of bits,
//       and optimizes the operations on those bits to be as fast as possible.
//

// Constructors:
//  cBitArray()                            -- Assume a size zero array.
//  cBitArray(int in_size)                 -- Create an uninitialized array.
//  cBitArray(const cBitArray & in_array)  -- Copy Constructor

// Assignment and equality test:
//  cBitArray & operator=(const cBitArray & in_array)
//  bool operator==(const cBitArray & in_array) const

// Sizing:
//  int GetSize() const
//  void Resize(const int new_size)
//  void ResizeClear(const int new_size)

// Accessors:
//  void Set(int index, bool value)
//  bool Get(int index) const
//  bool operator[](int index) const
//  cBitProxy operator[](int index)
//  void Clear()
//  void SetAll()

// Printing:
//  void Print(ostream & out=cout) const
//  void PrintOneIDs(ostream & out=cout) const

// Bit play:
//  int CountBits()   -- Count 1s -- fast for sparse arrays.
//  int CountBits2()  -- Count 1s -- fast for arbitary arrays.
//  int FindBit1(int start_bit)   -- Return pos of first 1 after start_bit 

// Boolean math functions:
//  cBitArray NOT() const
//  cBitArray AND(const cBitArray & array2) const
//  cBitArray OR(const cBitArray & array2) const
//  cBitArray NAND(const cBitArray & array2) const
//  cBitArray NOR(const cBitArray & array2) const
//  cBitArray XOR(const cBitArray & array2) const
//  cBitArray EQU(const cBitArray & array2) const
//  cBitArray SHIFT(const int shift_size) const   -- positive for left shift, negative for right shift

//  const cBitArray & NOTSELF()
//  const cBitArray & ANDSELF(const cBitArray & array2)
//  const cBitArray & ORSELF(const cBitArray & array2)
//  const cBitArray & NANDSELF(const cBitArray & array2)
//  const cBitArray & NORSELF(const cBitArray & array2)
//  const cBitArray & XORSELF(const cBitArray & array2)
//  const cBitArray & EQUSELF(const cBitArray & array2)
//  const cBitArray & SHIFTSELF(const int shift_size) const

// Arithmetic:
//  cBitArray INCREMENTSELF()

// Operator overloads:
//  cBitArray operator~() const
//  cBitArray operator&(const cBitArray & ar2) const
//  cBitArray operator|(const cBitArray & ar2) const
//  cBitArray operator^(const cBitArray & ar2) const
//  cBitArray operator>>(const int) const
//  cBitArray operator<<(const int) const
//  const cBitArray & operator&=(const cBitArray & ar2)
//  const cBitArray & operator|=(const cBitArray & ar2)
//  const cBitArray & operator^=(const cBitArray & ar2)
//  const cBitArray & operator>>=(const int)
//  const cBitArray & operator<<=(const int)
//  cBitArray & operator++()     // prefix ++
//  cBitArray & operator++(int)  // postfix ++




// The following is an internal class used by cBitArray (and will be used in
// cBitMatrix eventually....).  It does not keep track of size, so this value
// must be passed in.

class cRawBitArray {
private:
  unsigned int * bit_fields;
  
  // Disallow default copy constructor and operator=
  // (we need to know the number of bits we're working with!)
  cRawBitArray(const cRawBitArray & ) { assert(false); }
  const cRawBitArray & operator=(const cRawBitArray & in_array)
    { assert(false); return *this; }

  inline int GetNumFields(const int num_bits) const { return 1 + ((num_bits - 1) >> 5); }
  inline int GetField(const int index) const { return index >> 5; }
  inline int GetFieldPos(const int index) const { return index & 31; }
public:
  cRawBitArray() : bit_fields(NULL) { ; }
  ~cRawBitArray() {
    if (bit_fields != NULL) {
      delete [] bit_fields;
    }
  }

  void Zero(const int num_bits) {
    const int num_fields = GetNumFields(num_bits);
    for (int i = 0; i < num_fields; i++) {
      bit_fields[i] = 0;
    }    
  }

  void Ones(const int num_bits) {
    const int num_fields = GetNumFields(num_bits);
    for (int i = 0; i < num_fields; i++) {
      bit_fields[i] = ~0;
    }    
    const int last_bit = GetFieldPos(num_bits);
    if (last_bit > 0) {
      bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
    }
  }

  cRawBitArray(const int num_bits) {
    const int num_fields = GetNumFields(num_bits);
    bit_fields = new unsigned int[ num_fields ];
    Zero(num_bits);
  }

  // The Copy() method and the Copy Constructor must both be told how many
  // bits they are working with.
  void Copy(const cRawBitArray & in_array, const int num_bits);
  cRawBitArray(const cRawBitArray & in_array, const int num_bits)
    : bit_fields(NULL)
  {
    Copy(in_array, num_bits);
  }

  // For fast bit operations, we're not going to setup operator[]; instead
  // we're going to have a GetBit and a SetBit commamd.  For this raw version
  // we're also going to assume that the index is within range w/o any special
  // checks.
  bool GetBit(const int index) const{
    const int field_id = GetField(index);
    const int pos_id = GetFieldPos(index);
    return (bit_fields[field_id] & (1 << pos_id)) != 0;
  }

  void SetBit(const int index, const bool value) {
    const int field_id = GetField(index);
    const int pos_id = GetFieldPos(index);
    const int pos_mask = 1 << pos_id;

    if (value == false) {
      bit_fields[field_id] &= ~pos_mask;
    } else {
      bit_fields[field_id] |= pos_mask;
    }
  }

  bool IsEqual(const cRawBitArray & in_array, int num_bits) const;

  void Resize(const int old_bits, const int new_bits);
  void ResizeSloppy(const int new_bits);
  void ResizeClear(const int new_bits);

  // Two different technique of bit counting...
  int CountBits(const int num_bits) const; // Better for sparse arrays
  int CountBits2(const int num_bits) const; // Better for dense arrays

  // Other bit-play
  int FindBit1(const int num_bits, const int start_pos) const;
  tArray<int> GetOnes(const int num_bits) const;
  void ShiftLeft(const int num_bits, const int shift_size); // Helper: call SHIFT with positive number instead
  void ShiftRight(const int num_bits, const int shift_size); // Helper: call SHIFT with negative number instead

  void Print(const int num_bits, ostream & out=cout) const {
    for (int i = 0; i < num_bits; i++) {
      out << GetBit(i);
    }
  }
  
  // prints in the accepted human readable low-to-hight = right-to-left format, taking bit 0 as low bit
  void PrintRightToLeft(const int num_bits, ostream & out=cout) const {
    for (int i = num_bits - 1; i >= 0; i--) {
      out << GetBit(i);
    }
  }

  void PrintOneIDs(const int num_bits, ostream & out=cout) const {
    for (int i = 0; i < num_bits; i++) {
      if (GetBit(i) == true) {
	out << i << " ";
      }
    }
  }

  // Fast bool operators where we uses this bit array as one of the 
  // inputs and the place to store the results.
  void NOT(const int num_bits);
  void AND(const cRawBitArray & array2, const int num_bits);
  void OR(const cRawBitArray & array2, const int num_bits);
  void NAND(const cRawBitArray & array2, const int num_bits);
  void NOR(const cRawBitArray & array2, const int num_bits);
  void XOR(const cRawBitArray & array2, const int num_bits);
  void EQU(const cRawBitArray & array2, const int num_bits);
  void SHIFT(const int num_bits, const int shift_size);  // positive numbers for left and negative for right (0 does nothing)
  void INCREMENT(const int num_bits);

  // Fast bool operators where we load all of the inputs and store the
  // results here.
  void NOT(const cRawBitArray & array1, const int num_bits);
  void AND(const cRawBitArray & array1, const cRawBitArray & array2,
	   const int num_bits);
  void OR(const cRawBitArray & array1, const cRawBitArray & array2,
	  const int num_bits);
  void NAND(const cRawBitArray & array1, const cRawBitArray & array2,
	    const int num_bits);
  void NOR(const cRawBitArray & array1, const cRawBitArray & array2,
	   const int num_bits);
  void XOR(const cRawBitArray & array1, const cRawBitArray & array2,
	   const int num_bits);
  void EQU(const cRawBitArray & array1, const cRawBitArray & array2,
	   const int num_bits);
  void SHIFT(const cRawBitArray & array1, const int num_bits, const int shift_size);
  void INCREMENT(const cRawBitArray & array1, const int num_bits);  // implemented for completeness, but unused by cBitArray
};

class cBitArray {
private:
  cRawBitArray bit_array;
  int array_size;

  // Setup a bit proxy so that we can use operator[] on bit arrays as a lvalue.
  class cBitProxy {
  private:
    cBitArray & array;
    int index;
  public:
    cBitProxy(cBitArray & _array, int _idx) : array(_array), index(_idx) {;}

    inline cBitProxy & operator=(bool b);    // lvalue handling...
    inline operator bool() const;            // rvalue handling...
  };
  friend class cBitProxy;
public:
  cBitArray() : array_size(0) { ; }
  cBitArray(int in_size) : bit_array(in_size), array_size(in_size) { ; }
  cBitArray(const cBitArray & in_array)
    : bit_array(in_array.bit_array, in_array.array_size)
    , array_size(in_array.array_size) { ; }
  cBitArray(const cRawBitArray & in_array, int in_size)
    : bit_array(in_array, in_size)
    , array_size(in_size) { ; }

  cBitArray & operator=(const cBitArray & in_array) {
    bit_array.Copy(in_array.bit_array, in_array.array_size);
    array_size = in_array.array_size;
    return *this;
  }

  bool operator==(const cBitArray & in_array) const {
    if (array_size != in_array.array_size) return false;
    return bit_array.IsEqual(in_array.bit_array, array_size);
  }

  int GetSize() const { return array_size; }

  void Set(int index, bool value) {
    assert(index < array_size);
    bit_array.SetBit(index, value);
  }

  bool Get(int index) const {
    assert(index < array_size);
    return bit_array.GetBit(index);
  }

  bool operator[](int index) const { return Get(index); }
  cBitProxy operator[](int index) { return cBitProxy(*this, index); }

  void Clear() { bit_array.Zero(array_size); }
  void SetAll() { bit_array.Ones(array_size); }
  

  void Print(ostream & out=cout) const { bit_array.Print(array_size, out); }
  void PrintRightToLeft(ostream & out=cout) const { bit_array.PrintRightToLeft(array_size, out); }
  void PrintOneIDs(ostream & out=cout) const
    { bit_array.PrintOneIDs(array_size, out); }
  void Resize(const int new_size) {
    bit_array.Resize(array_size, new_size);
    array_size = new_size;
  }
  void ResizeClear(const int new_size) {
    bit_array.ResizeClear(new_size);
    array_size = new_size;
  }
  int CountBits() const { return bit_array.CountBits(array_size); }
  int CountBits2() const { return bit_array.CountBits2(array_size); }

  int FindBit1(int start_bit=0) const
    { return bit_array.FindBit1(array_size, start_bit); }
  tArray<int> GetOnes() const { return bit_array.GetOnes(array_size); }

  // Boolean math functions...
  cBitArray NOT() const {
    cBitArray out_array;
    out_array.bit_array.NOT(bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray AND(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.AND(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray OR(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.OR(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray NAND(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.NAND(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray NOR(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.NOR(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray XOR(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.XOR(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }

  cBitArray EQU(const cBitArray & array2) const {
    assert(array_size == array2.array_size);
    cBitArray out_array;
    out_array.bit_array.EQU(bit_array, array2.bit_array, array_size);
    out_array.array_size = array_size;
    return out_array;
  }
  
  cBitArray SHIFT(const int shift_size) const {
    cBitArray out_array;
    out_array.bit_array.SHIFT(bit_array, array_size, shift_size);
    out_array.array_size = array_size;
    return out_array;
  }

  const cBitArray & NOTSELF() {
    bit_array.NOT(array_size);
    return *this;
  }

  const cBitArray & ANDSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.AND(array2.bit_array, array_size);
    return *this;
  }

  const cBitArray & ORSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.OR(array2.bit_array, array_size);
    return *this;
  }

  const cBitArray & NANDSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.NAND(array2.bit_array, array_size);
    return *this;
  }

  const cBitArray & NORSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.NOR(array2.bit_array, array_size);
    return *this;
  }

  const cBitArray & XORSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.XOR(array2.bit_array, array_size);
    return *this;
  }

  const cBitArray & EQUSELF(const cBitArray & array2) {
    assert(array_size == array2.array_size);
    bit_array.EQU(array2.bit_array, array_size);
    return *this;
  }
  
  const cBitArray & SHIFTSELF(const int shift_size) {
    bit_array.SHIFT(array_size, shift_size);
    return *this;
  }
  
  cBitArray & INCREMENTSELF() {
    bit_array.INCREMENT(array_size);
    return *this;
  }
  

  // Operator overloads...
  cBitArray operator~() const { return NOT(); }
  cBitArray operator&(const cBitArray & ar2) const { return AND(ar2); }
  cBitArray operator|(const cBitArray & ar2) const { return OR(ar2); }
  cBitArray operator^(const cBitArray & ar2) const { return XOR(ar2); }
  cBitArray operator<<(const int shift_size) const { return SHIFT(shift_size); }
  cBitArray operator>>(const int shift_size) const { return SHIFT(-shift_size); }
  const cBitArray & operator&=(const cBitArray & ar2) { return ANDSELF(ar2); }
  const cBitArray & operator|=(const cBitArray & ar2) { return ORSELF(ar2); }
  const cBitArray & operator^=(const cBitArray & ar2) { return XORSELF(ar2); }
  const cBitArray & operator<<=(const int shift_size) { return SHIFTSELF(shift_size); }
  const cBitArray & operator>>=(const int shift_size) { return SHIFTSELF(-shift_size); }
  cBitArray & operator++() { return INCREMENTSELF(); }  // prefix ++
  cBitArray operator++(int) { cBitArray ans = *this; operator++(); return ans;}  // postfix ++

};

std::ostream & operator << (std::ostream & out, const cBitArray & bit_array);

cBitArray::cBitProxy & cBitArray::cBitProxy::operator=(bool b)
{
  array.Set(index, b);
  return *this;
}


cBitArray::cBitProxy::operator bool() const
{
  return array.Get(index);
}

#endif
