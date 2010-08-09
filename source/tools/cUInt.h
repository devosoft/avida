/*
 *  cUInt.h
 *  Avida
 *
 *  Called "uint.hh" prior to 12/7/05.
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

#ifndef cUInt_h
#define cUInt_h

#include <cstdio>

// Should this just use a long?  For testing only...
#define LONG_C_UINT

#ifdef LONG_C_UINT
class cUInt
{
private:
  long value;
  int size;

public:
  inline cUInt() { size = 0; value = 0; }
  inline cUInt(int in_size) { size = in_size, value = 0; }
  inline cUInt(int in_size, int in_value) { size = in_size, value = in_value; }
  inline ~cUInt() { ; }

  inline void Print() const { printf ("%ld", value); }
  inline int AsInt() const { return (int) value; }
  inline long AsLong() const { return value; }

  inline void ShiftLeft(int num_bytes = 1) { value = value << (8 * num_bytes);}
  inline void ShiftRight(int num_bytes = 1){ value = value >> (8 * num_bytes);}

  inline cUInt& operator=(const unsigned char in_char) {
    value = (long) in_char;
    return *this;
  }
  inline cUInt& operator=(const int in_int) {
    value = (long) in_int;
    return *this;
  }
  inline cUInt&  operator=(const cUInt& in_cUInt) {
    value = in_cUInt.AsLong();
    return *this;
  }

  inline cUInt& operator*=(const unsigned char in_char){
    value *= (long) in_char;
    return *this;
  }

  inline cUInt& operator+=(const int in_int) {
    value += (long) in_int;
    return *this;
  }

  inline cUInt& operator-=(const int in_int) {
    value -= in_int;
    return *this;
  }

  inline cUInt& operator*=(const int in_int) {
    value *= in_int;
    return *this;
  }

  inline cUInt& operator/=(const int in_int) {
    value /= in_int;
    return *this;
  }

  inline cUInt& operator%=(const int in_int) {
    value %= in_int;
    return *this;
  }


  inline cUInt& operator+=(const cUInt& in_cUInt) {
    value += in_cUInt.AsLong();
    return *this;
  }

  inline cUInt& operator-=(const cUInt& in_cUInt) {
    value -= in_cUInt.AsLong();
    return *this;
  }

  inline cUInt& operator*=(const cUInt& in_cUInt) {
    value *= in_cUInt.AsLong();
    return *this;
  }

  inline cUInt& operator/=(const cUInt& in_cUInt) {
    value /= in_cUInt.AsLong();
    return *this;
  }

  inline cUInt& operator%=(const cUInt& in_cUInt) {
    value %= in_cUInt.AsLong();
    return *this;
  }


  inline cUInt& operator+(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp += in_int;
    return *temp;
  }
  inline cUInt& operator-(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp -= in_int;
    return *temp;
  }
  inline cUInt& operator*(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp *= in_int;
    return *temp;
  }
  inline cUInt& operator/(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp /= in_int;
    return *temp;
  }
  inline cUInt& operator%(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp %= in_int;
    return *temp;
  }

  inline cUInt& operator+(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp += in_cUInt;
    return *temp;
  }
  inline cUInt& operator-(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp -= in_cUInt;
    return *temp;
  }
  inline cUInt& operator*(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp *= in_cUInt;
    return *temp;
  }
  inline cUInt& operator/(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp /= in_cUInt;
    return *temp;
  }
  inline cUInt& operator%(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp %= in_cUInt;
    return *temp;
  }

  inline int operator==(const cUInt& in_cUInt) const {
    return (value == in_cUInt.AsLong());
  }

  inline int operator<(const cUInt& in_cUInt) const {
    return (value < in_cUInt.AsLong());
  }

  inline int operator<=(const cUInt& in_cUInt) const {
    return (value <= in_cUInt.AsLong());
  }

  inline int operator>(const cUInt& in_cUInt) const {
    return (value > in_cUInt.AsLong());
  }

  inline int operator>=(const cUInt& in_cUInt) const {
    return (value >= in_cUInt.AsLong());
  }
};


#else /* LONG_C_UINT */

/**
 * This class constructs an arbitrary (pre-set) large integer.  Not all of the
 * possible operators have been added yet; they will be as they are needed.
 **/

class cUInt {
private:
  unsigned char * value;
  int size;

public:
  inline cUInt();
  inline cUInt(int in_size);
  inline cUInt(int in_size, int in_value);
  inline ~cUInt();

  inline void Resize(int new_size);
  inline void PrintBytes() const;
  inline void Print(cUInt * input_thing = NULL) const;
  inline int AsInt() const;
  inline long AsLong() const; //Do not use!! Assumes 8 byte longs... for tests.

  inline int GetSize() const { return size; }
  inline int GetSizeUsed() const;
  inline unsigned char GetByte(int byte_num) const { return value[byte_num]; }
  inline void SetByte(int byte_num, unsigned char in_char) {value[byte_num] = in_char;}

  inline void ShiftLeft(int num_bytes = 1);
  inline void ShiftRight(int num_bytes = 1);

  inline cUInt& operator=(const unsigned char in_char);
  inline cUInt& operator=(const int in_int);
  inline cUInt& operator=(const cUInt& in_cUInt);

  inline cUInt& operator*=(const unsigned char in_char);

  inline cUInt& operator+=(const int in_int);
  inline cUInt& operator-=(const int in_int);
  inline cUInt& operator*=(const int in_int);
  inline cUInt& operator/=(const int in_int);
  inline cUInt& operator%=(const int in_int);

  inline cUInt& operator+=(const cUInt& in_cUInt);
  inline cUInt& operator-=(const cUInt& in_cUInt);
  inline cUInt& operator*=(const cUInt& in_cUInt);
  inline cUInt& operator/=(const cUInt& in_cUInt);
  inline cUInt& operator%=(const cUInt& in_cUInt);

  inline int operator==(const cUInt& in_cUInt) const;
  inline int operator<( const cUInt& in_cUInt) const;
  inline int operator<=(const cUInt& in_cUInt) const;
  inline int operator>( const cUInt& in_cUInt) const;
  inline int operator>=(const cUInt& in_cUInt) const;

  inline cUInt& operator+(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp += in_int;
    return *temp;
  }
  inline cUInt& operator-(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp -= in_int;
    return *temp;
  }
  inline cUInt& operator*(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp *= in_int;
    return *temp;
  }
  inline cUInt& operator/(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp /= in_int;
    return *temp;
  }
  inline cUInt& operator%(const int in_int) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp %= in_int;
    return *temp;
  }

  inline cUInt& operator+(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp += in_cUInt;
    return *temp;
  }
  inline cUInt& operator-(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp -= in_cUInt;
    return *temp;
  }
  inline cUInt& operator*(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp *= in_cUInt;
    return *temp;
  }
  inline cUInt& operator/(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp /= in_cUInt;
    return *temp;
  }
  inline cUInt& operator%(const cUInt& in_cUInt) {
    cUInt * temp = new cUInt;
    *temp = *this;
    *temp %= in_cUInt;
    return *temp;
  }
};

inline cUInt::cUInt()
{
  int i;
  size = 10;
  value = new unsigned char[size];
  for (i = 0; i < size; i++) {
    value[i] = 0;
  }
}

inline cUInt::cUInt(int in_size)
{
  int i;

  size = in_size;
  value = new unsigned char[size];
  for (i = 0; i < size; i++) {
    value[i] = 0;
  }
}

inline cUInt::cUInt(int in_size, int in_value)
{
  int i;

  size = in_size;
  value = new unsigned char[size];
  for (i = 0; i < size && i < 4; i++) {
    value[i] = static_cast<unsigned char>(in_value >> (8 * i));
  }
  while (i < size) {
    value[i++] = 0;
  }
}

inline cUInt::~cUInt()
{
  delete value;
}

inline void cUInt::Resize(int new_size)
{
  unsigned char * new_value = new unsigned char[new_size];

  int i;
  for (i = 0; i < new_size && i < size; i++) {
    new_value[i] = value[i];
  }
  while (i < new_size) {
    new_value[i++] = 0;
  }

  delete value;
  size = new_size;
  value = new_value;
}

inline void cUInt::PrintBytes() const
{
  int i;

  for (i = 0; i < GetSizeUsed(); i++) {
    printf("[%d] ", (int) value[i]);
  }
}

// This is *very* poorly written currently!
inline void cUInt::Print(cUInt * input_thing) const
{
  if (!input_thing) {
    cUInt * input_thing2;
    input_thing2 = new cUInt(size);
    *input_thing2 = *this;
    Print(input_thing2);
  }
  else if (input_thing->GetSizeUsed()) {
    cUInt this_num(size);
    this_num = *input_thing;
    this_num %= 10;

    *input_thing /= 10;
    Print(input_thing);
    printf("%01d", this_num.AsInt());
  }
}

inline int cUInt::AsInt() const
{
  int i, ret_value = 0;

  for (i = 3; i >= 0; i--) {
    ret_value = ret_value << 8;
    ret_value += value[i];
  }

  return ret_value;
}

inline long cUInt::AsLong() const
{
  int i;
  long ret_value = 0;

  for (i = 7; i >= 0; i--) {
    ret_value = ret_value << 8;
    ret_value += value[i];
  }

  return ret_value;
}

inline int cUInt::GetSizeUsed() const
{
  int size_used = size, i;
  for (i = size - 1; i >= 0 && !value[i]; i--) size_used--;
  return size_used;
}

inline void cUInt::ShiftLeft(int num_bytes)
{
  int i;

  // Starting at the end, copy down all the applicable bytes.

  for (i = size - 1; i >= num_bytes; i--) {
    value[i] = value[i - num_bytes];
  }

  // Now fill in all of the 'new' bytes with 0's.

  for (i = num_bytes - 1; i >= 0; i--) {
    value[i] = 0;
  }
}

inline void cUInt::ShiftRight(int num_bytes)
{
  int i;

  // Shift over the numbers...

  for (i = 0; i < size - num_bytes; i++) {
    value[i] = value[i + num_bytes];
  }

  // And fill in 0's where applicable.

  for (i = size - num_bytes; i < size; i++) {
    value[i] = 0;
  }
}

inline cUInt& cUInt::operator=(const unsigned char in_char)
{
  int i;
  value[0] = in_char;
  for (i = 1; i < size; i++) {
    value[i] = 0;
  }

  return *this;
}

inline cUInt& cUInt::operator=(const int in_int)
{
  int i;
  for (i = 0; i < 4 && i < size; i++) {
    value[i] = static_cast<unsigned char>(in_int >> (8*i));
  }
  for (i = 4; i < size; i++) {
    value[i] = 0;
  }

  return *this;
}

inline cUInt& cUInt::operator=(const cUInt& in_cUInt)
{
  int i;
  int size2 = in_cUInt.GetSize();

  for (i = 0; i < size && i < size2; i++) {
    value[i] = in_cUInt.GetByte(i);
  }
  for (i = size2; i < size; i++) {
    value[i] = 0;
  }

  return *this;
}

inline cUInt& cUInt::operator*=(const unsigned char in_char)
{
  int this_byte = 0, carry = 0;
  int i;

  for (i = 0; i < size; i++) {
    this_byte = (int) value[i] * (int) in_char + carry;
    value[i] = this_byte & 255;
    carry = this_byte >> 8;
  }

  return *this;
}

inline cUInt& cUInt::operator+=(const int in_int)
{
  cUInt add_int(4, in_int);
  operator+=(add_int);

  return *this;
}

inline cUInt& cUInt::operator-=(const int in_int)
{
  cUInt sub_int(4, in_int);
  operator-=(sub_int);

  return *this;
}

inline cUInt& cUInt::operator*=(const int in_int)
{
  cUInt mult_int(4, in_int);
  operator*=(mult_int);

  return *this;
}

inline cUInt& cUInt::operator/=(const int in_int)
{
  cUInt div_int(4, in_int);
  operator/=(div_int);

  return *this;
}

inline cUInt& cUInt::operator%=(const int in_int)
{
  cUInt mod_int(4, in_int);
  operator%=(mod_int);

  return *this;
}

inline cUInt& cUInt::operator+=(const cUInt& in_cUInt)
{
  int i;
  int cur_sum = 0;

  // Add up sums byte by byte, carrying any remainders.

  for (i = 0; i < size && i < in_cUInt.GetSize(); i++) {
    cur_sum += value[i];
    cur_sum += in_cUInt.GetByte(i);
    value[i] = static_cast<unsigned char>(cur_sum & 255);
    cur_sum = cur_sum >> 8;
  }

  // If there is still a remainder, and room for it, continue.

  while (cur_sum && i < size) {
    value[i++] = static_cast<unsigned char>(cur_sum & 255);
  }

  return *this;
}

inline cUInt& cUInt::operator-=(const cUInt& in_cUInt)
{
  int i;
  int next_byte = 0, byte1, byte2;

  for (i = 0; i < size && i < in_cUInt.GetSize(); i++) {
    byte1 = (int) value[i];
    byte2 = (int) in_cUInt.GetByte(i);
    if (byte1 + next_byte >= byte2) {
      value[i] = static_cast<unsigned char>(byte1 + next_byte - byte2);
      next_byte = 0;
    }
    else {
      value[i] = static_cast<unsigned char>(256 + byte1 + next_byte - byte2);
      next_byte = -1;
    }
  }

  // Check to see if there are any more effects of the carry...

  for (i = in_cUInt.GetSize(); i < size && next_byte < 0; i++) {
    if (value[i] + next_byte >= 0) {
      value[i] += next_byte;
      next_byte = 0;
    }
    else {
      value[i] = static_cast<unsigned char>(256 + next_byte + (int) value[i]);
      next_byte = -1;
    }
  }

  return *this;
}

inline cUInt& cUInt::operator*=(const cUInt& in_cUInt)
{
  cUInt sum_cUInt(size), temp_cUInt(size);
  int i;

  for (i = in_cUInt.GetSize() - 1; i >= 0; i--) {
    temp_cUInt = *this;
    temp_cUInt *= in_cUInt.GetByte(i);
    sum_cUInt.ShiftLeft();
    sum_cUInt += temp_cUInt;
  }

  operator=(sum_cUInt);

  return *this;
}

inline cUInt& cUInt::operator/=(const cUInt& in_cUInt)
{
  // Guess the approximate size of the result, and start from there.

  cUInt result(size + 2);  // @CAO FIX THIS

  // Now, loops through filling in one byte of the result at a time.

  int cur_byte, cur_bit;
  for (cur_byte = GetSizeUsed() - in_cUInt.GetSizeUsed(); cur_byte >= 0; cur_byte--) {
    for (cur_bit = 7; cur_bit >= 0; cur_bit--) {
      cUInt cur_test(result.GetSize(), 1 << cur_bit);
      cur_test.ShiftLeft(cur_byte);
      cur_test += result;

      if (*this >= (cur_test * in_cUInt)) result = cur_test;
    }
  }

  operator=(result);

  return *this;
}

inline cUInt& cUInt::operator%=(const cUInt& in_cUInt)
{
  cUInt multiple(size);

  multiple = *this;
  multiple /= in_cUInt;
  multiple *= in_cUInt;
  *this -= multiple;

  return *this;
}

/* int operator==(int in_int)
{
  cUInt temp_int(4, in_int);
  return operator==(temp_int);
}

int operator<(int in_int)
{
  cUInt temp_int(4, in_int);
  return operator<(temp_int);
}

int operator<=(int in_int)
{
  cUInt temp_int(4, in_int);
  return operator<=(temp_int);
}

int operator>(int in_int)
{
  cUInt temp_int(4, in_int);
  return operator>(temp_int);
}

int operator>=(int in_int)
{
  cUInt temp_int(4, in_int);
  return operator>=(temp_int);
}
*/

inline bool cUInt::operator==(const cUInt& in_cUInt) const
{
  int i;

  // Make sure all of the overlaped bytes are identical.

  for (i = 0; i < size && i < in_cUInt.GetSize(); i++) {
    if (value[i] != in_cUInt.GetByte(i)) return false;
  }

  // Make sure all of the bytes one contains beyond the other are 0.

  for (i = in_cUInt.GetSize(); i < size; i++) {
    if (value[i]) return false;
  }
  for (i = size; i < in_cUInt.GetSize(); i++) {
    if (in_cUInt.GetByte(i)) return false;
  }

  return true;
}

inline bool cUInt::operator<(const cUInt& in_cUInt) const
{
  // First check the extra digits to see if they effect things...

  for (int i = size; i < in_cUInt.GetSize(); i++) {
    if (in_cUInt.GetByte(i)) return true;
  }
  for (int i = in_cUInt.GetSize(); i < size; i++) {
    if (value[i]) return false;
  }

  // Now loop through the matching digits until there is a difference

  int match_size = size;
  if (size > in_cUInt.GetSize()) match_size = in_cUInt.GetSize();

  for (int i = match_size - 1; i >= 0; i--) {
    if (value[i] > in_cUInt.GetByte(i)) return false;
    if (value[i] < in_cUInt.GetByte(i)) return true;
  }

  // And if they are equal...

  return false;
}

inline bool cUInt::operator<=(const cUInt& in_cUInt) const
{
  // First check the extra digits to see if they effect things...

  for (int i = size; i < in_cUInt.GetSize(); i++) {
    if (in_cUInt.GetByte(i)) return true;
  }
  for (int i = in_cUInt.GetSize(); i < size; i++) {
    if (value[i]) return false;
  }

  // Now loop through the matching digits until there is a difference

  int match_size = size;
  if (size > in_cUInt.GetSize()) match_size = in_cUInt.GetSize();

  for (int i = match_size - 1; i >= 0; i--) {
    if (value[i] > in_cUInt.GetByte(i)) return false;
    if (value[i] < in_cUInt.GetByte(i)) return true;
  }

  // And if they are equal...

  return true;
}

inline bool cUInt::operator>(const cUInt& in_cUInt) const
{
  return !operator<=(in_cUInt);
}

inline cUInt::operator>=(const cUInt& in_cUInt) const
{
  return !operator<(in_cUInt);
}

#endif

#endif
