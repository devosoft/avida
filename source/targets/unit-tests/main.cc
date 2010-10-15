/*
 *  main.cc
 *  Avida
 *
 *  Created by David on 5/3/07
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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

#include <iostream>
#include <iomanip>

using namespace std;


class cUnitTest
{
private:
  int m_total;
  int m_failed;

protected:
  virtual void RunTests() = 0;
  
  void ReportTestResult(const char* test_name, bool successful);

public:
  cUnitTest() : m_total(0), m_failed(0) { ; }
  virtual ~cUnitTest() { ; }

  virtual const char* GetUnitName() = 0;
  
  void Execute();
  
  inline int GetNumTests() { return m_total; }
  inline int GetSuccessful() { return m_total - m_failed; }
  inline int GetFailed() { return m_failed; }
};


#include "tArray.h"
class tArrayTests : public cUnitTest
{
public:
  const char* GetUnitName() { return "tArray"; }
protected:
  void RunTests()
  {
    bool result = false;
    const int SIZE = 10;
    
    tArray<int> test1;
    ReportTestResult("Constructor with no args", test1.GetSize() == 0);
    
    test1.Resize(SIZE);
    ReportTestResult("Resize Empty", test1.GetSize() == SIZE);
    
    for (int i = 0; i < SIZE; i++) test1[i] = i;
    ReportTestResult("Get/Set Values", test1[5] == 5);
    
    tArray<int> test2(SIZE);
    ReportTestResult("Constructor with default size", test2.GetSize() == SIZE);
    
    result = true;
    test2.SetAll(5);
    for (int i = 0; i < SIZE; i++) if (test2[i] != 5) result = false;
    ReportTestResult("SetAll", result);
    
    test2 = test1;
    ReportTestResult("Assignment", test2[6] == 6);
    
    test2.Resize(SIZE + 1, -1);
    ReportTestResult("Resize with default value", test2[SIZE] == -1 && test2[SIZE - 1] == (SIZE - 1));
    
    test1.Push(-1);
    ReportTestResult("Push value", test1[SIZE] == -1 && test1[SIZE - 1] == (SIZE - 1));
    
    result = true;
    for (tArray<int>::iterator it = test1.begin(); it != test1.end(); it++) (*it) = -2;
    for (int i = 0; i < test1.GetSize(); i++) if (test1[i] != -2) result = false;
    ReportTestResult("STL-style iterator", result);
    
    result = true;
    tArray<int> test3(10);
    test3[0] = 3;
    test3[1] = 9;
    test3[2] = 0;
    test3[3] = -1;
    test3[4] = 2;
    test3[5] = 3;
    test3[6] = 7;
    test3[7] = -3;
    test3[8] = 0;
    test3[9] = 4;
    test3.MergeSort(IntCompareFunction);
    for(int i = 0; i < test3.GetSize() - 1; i++) if (test3[i] > test3[i+1]) result = false;
    ReportTestResult("MergeSort", result);
  }
  
  static int IntCompareFunction(const void * a, const void * b)
  {
    if( *((int*)a) > *((int*)b) ) return 1;
    if( *((int*)a) < *((int*)b) ) return -1;
    return 0;
  }
  
};



#include "cBitArray.h"
class cRawBitArrayTests : public cUnitTest
{
public:
  const char* GetUnitName() { return "cRawBitArray"; }
protected:
  void RunTests()
  {
    int result = true;
    
    cRawBitArray bit_array1;
    bit_array1.ResizeClear(10);
    for (int i = 0; i < 10; i++) {
      if (bit_array1.GetBit(i) != false) {
        result = false;
        break;
      }
    }
    ReportTestResult("Default Constructor - ResizeClear", result);
    

    result = true;
    
    bit_array1.SetBit(1, true);
    bit_array1.SetBit(3, true);
    bit_array1.SetBit(5, true);
    bit_array1.SetBit(7, true);
    bit_array1.SetBit(9, true);
    
    for (int i = 0; i < 10; i++) {
      bool bit_value = !(2*(i/2) == i);
      if (bit_array1.GetBit(i) != bit_value) {
        result = false;
        break;
      }
    }
    ReportTestResult("SetBit (round 1)", result);

    
    result = true;
    
    bit_array1.SetBit(0, true);
    bit_array1.SetBit(1, false);
    bit_array1.SetBit(5, false);
    bit_array1.SetBit(6, true);
    bit_array1.SetBit(7, false);
    
    for (int i = 0; i < 10; i++) {
      bool bit_value = (3*(i/3) == i);
      if (bit_array1.GetBit(i) != bit_value) {
        result = false;
        break;
      }
    }
    ReportTestResult("SetBit (round 2)", result);
    
    // Test constructor with initial size < 32.
    cRawBitArray bit_array2(26);
    for (int i = 0; i < 26; i++) {
      if (bit_array2.GetBit(i) != false) {
        result = false;
        break;
      }
    }
    ReportTestResult("Constructor (size < 32)", result);
    
    
    result = true;
    
    bit_array2.SetBit(8, true);
    bit_array2.Copy(bit_array1, 10);
    
    for (int i = 0; i < 10; i++) {
      bool bit_value = (3*(i/3) == i);
      if (bit_array2.GetBit(i) != bit_value) {
        result = false;
        break;
      }
    }
    ReportTestResult("Copy", result);
    
    
    result = true;
    
    // Test constructor with initial size > 32.
    const int high_bit_count = 1000;
    cRawBitArray bit_array3(high_bit_count);
    int bit_pos = 2;
    while (bit_pos < high_bit_count) {
      bit_array3.SetBit(bit_pos, true);
      bit_pos = bit_pos * 3 / 2;
    }
    
    
    // Test faux copy constructor.
    cRawBitArray bit_array4(bit_array3, high_bit_count);
    bit_array4.SetBit(22, true);
    bit_array4.SetBit(24, true);
    int count1 =  bit_array3.CountBits(high_bit_count);
    int count2 =  bit_array3.CountBits2(high_bit_count);
    int count3 =  bit_array4.CountBits(high_bit_count);
    int count4 =  bit_array4.CountBits2(high_bit_count);
    ReportTestResult("CountBits vs. CountBits2", (count1 == count2 && count3 == count4));
    ReportTestResult("CountBits - Post Copy", (count1 == (count3 - 2)));
    
    
    int diff_count = 0;
    for (int i = 0; i < high_bit_count; i++) {
      if (bit_array3.GetBit(i) != bit_array4.GetBit(i)) diff_count++;
    }
    ReportTestResult("Copy Constructor", (diff_count == 2));
    
    
    // LOGICAL OPERATORS
    
    bit_array4.Resize(1000, 70);
    int count5 = bit_array4.CountBits(70);
    bit_array4.NOT(70);
    int count6 = bit_array4.CountBits(70);
    bit_array4.NOT(70);
    ReportTestResult("NOT Operation", (count5 + count6 == 70));
    
    
    cRawBitArray bit_array5(70);
    int pos = 1;
    int step = 1;
    while (pos <= 70) {
      bit_array5.SetBit(70 - pos, true);
      pos += step++;
    }
    
    cRawBitArray bit_array6(70);
    bit_array6.AND(bit_array4, bit_array5, 70);
    int count_and = bit_array6.CountBits(70);
    ReportTestResult("AND Operation", (count_and == 3));
    
    
    bit_array6.OR(bit_array4, bit_array5, 70);
    int count_or = bit_array6.CountBits(70);
    ReportTestResult("OR Operation", (count_or == 21));
    
    
    bit_array6.NAND(bit_array4, bit_array5, 70);
    int count_nand = bit_array6.CountBits(70);
    ReportTestResult("NAND Operation", (count_nand == 67));
    
    
    bit_array6.NOR(bit_array4, bit_array5, 70);
    int count_nor = bit_array6.CountBits(70);
    ReportTestResult("NOR Operation", (count_nor == 49));
    
    
    bit_array6.XOR(bit_array4, bit_array5, 70);
    int count_xor = bit_array6.CountBits(70);
    ReportTestResult("XOR Operation", (count_xor == 18));
    
    
    bit_array6.EQU(bit_array4, bit_array5, 70);
    int count_equ = bit_array6.CountBits(70);
    ReportTestResult("EQU Operation", (count_equ == 52));
    
    
    // LEFT AND RIGHT SHIFT
    
    cRawBitArray bit_array7(32);
    bit_array7.SetBit(0, true);
    
    bit_array7.SHIFT(32, 0);
    ReportTestResult("Shift 0", (bit_array7.GetBit(0) && bit_array7.CountBits(32) == 1));
    
    
    bit_array7.SHIFT(32, 31);
    ReportTestResult("Shift Left", (bit_array7.GetBit(31) && bit_array7.CountBits(32) == 1));
    
    
    bit_array7.SHIFT(32, -31);
    ReportTestResult("Shift Right (sign bit)", (bit_array7.GetBit(0) || bit_array7.CountBits(32) == 1));
    
    
    bit_array7.SHIFT(32, 30);
    bit_array7.SHIFT(32, -30);
    ReportTestResult("Shift Right (no sign bit)", (bit_array7.GetBit(0) && bit_array7.CountBits(32) == 1));
    
    
    bit_array7.SHIFT(32, 32);
    ReportTestResult("Shift Left Overflow", (bit_array7.CountBits(32) == 0));
    
    
    bit_array7.SetBit(31, true);
    bit_array7.SHIFT(32, -32);
    ReportTestResult("Shift Right Overflow", (bit_array7.CountBits(32) == 0));
    
    
    cRawBitArray bit_array8(34);
    bit_array8.SetBit(0, true);
    
    bit_array8.SHIFT(34, 33);
    ReportTestResult("Shift Left (across bit fields)", (bit_array8.GetBit(33) && bit_array8.CountBits(34) == 1));
    
    
    bit_array8.SHIFT(34, -33);
    ReportTestResult("Shift Right (across bit fields)", (bit_array8.GetBit(0) && bit_array8.CountBits(34) == 1));
    
    
    cRawBitArray bit_array9(66);
    bit_array9.SetBit(0, true);
    bit_array9.SetBit(32, true);
    
    bit_array9.SHIFT(66, 65);
    ReportTestResult("Shift Left (multiple bit fields)", (bit_array9.GetBit(65) && bit_array9.CountBits(66) == 1));
    
    
    bit_array9.SHIFT(66, -65);
    ReportTestResult("Shift Right (multiple bit fields)", (bit_array9.GetBit(0) && bit_array9.CountBits(66) == 1));
    
    // INCREMENT
    
    cRawBitArray bit_array10(1);
    
    bit_array10.INCREMENT(1);
    ReportTestResult("Increment", (bit_array10.GetBit(0) && bit_array10.CountBits(1) == 1));
    
    bit_array10.INCREMENT(1);
    ReportTestResult("Increment Overflow", (bit_array10.GetBit(0) == false && bit_array10.CountBits(1) == 0));
    
    cRawBitArray bit_array11(33);
    for (int i = 0; i < 32; i++) { bit_array11.SetBit(i, true); }
    bit_array11.INCREMENT(33);
    ReportTestResult("Increment (multiple bit fields)", (bit_array11.GetBit(32) == 1 && bit_array11.CountBits(33) == 1));
    
  }
};

class cBitArrayTests : public cUnitTest
{
public:
  const char* GetUnitName() { return "cBitArray"; }
protected:
  void RunTests()
  {
    cBitArray ba(74);
    for (int i = 0; i < 74; i++) if (i % 5 == 3) ba[i] = true;
    
    cBitArray ba2(74);
    for (int i = 0; i < 74; i++) {
      if ((i % 2 == 0 || i % 3 == 0) && i % 6 != 0) ba2[i] = true;
    }
    
    ReportTestResult("operator&", ((ba & ba2).CountBits() == 8));
    ReportTestResult("operator|", ((ba | ba2).CountBits() == 43));
    ReportTestResult("operator^", ((ba ^ ba2).CountBits() == 35));
    ReportTestResult("operator~", ((~ba).CountBits() == 59));
    ReportTestResult("operator<<", ((ba << 65).CountBits() == 2));
    ReportTestResult("operator>>", ((ba >> 65).CountBits() == 2));
    ReportTestResult("Chained Bitwise Operations", ((~ba & ~ba2).CountBits() == 31));
    ReportTestResult("++operator", ((++(~ba & ~ba2)).CountBits() == 30));
    ReportTestResult("operator++", (((~ba & ~ba2)++).CountBits() == 31));
  }
};




#define TEST(CLASS) \
tester = new CLASS ## Tests(); \
tester->Execute(); \
total += tester->GetNumTests(); \
failed += tester->GetFailed(); \
delete tester;

int main(int argc, const char* argv[])
{
  int total = 0;
  int failed = 0;
  
  cUnitTest* tester = NULL;
  
  cout << "Avida Tools Unit Tests" << endl;
  cout << endl;
  
  TEST(tArray);
  TEST(cRawBitArray);
  TEST(cBitArray);
  
  if (failed == 0)
    cout << "All unit tests passed." << endl;
  else
    cout << failed << " of " << total << " unit tests failed." << endl;

  return failed;
}


void cUnitTest::Execute()
{
  cout << "Testing: " << GetUnitName() << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
  RunTests();
  cout << "--------------------------------------------------------------------------------" << endl;
  if (GetFailed() == 0)
    cout << "All " << GetUnitName() << " tests passed." << endl;
  else
    cout << GetFailed() << " of " << GetNumTests() << " tests failed." << endl;
  
  cout << endl;
}

void cUnitTest::ReportTestResult(const char* test_name, bool successful)
{
  m_total++;
  
  size_t l = strlen(test_name);
  char* str = new char[l + 3];
  str = strncpy(str, test_name, l + 1);
  str = strncat(str, ": ", 2);
  
  cout << setw(74) << left << str;
  if (successful) {
    cout << "passed";
  } else {
    cout << "failed";
    m_failed++;
  }
  cout << endl;
  
  delete str;
}
