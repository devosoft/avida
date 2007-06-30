/*
 *  main.cc
 *  Avida
 *
 *  Created by David on 5/3/07
 *  Copyright 2007 Michigan State University. All rights reserved.
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
  str = strncpy(str, test_name, l);
  str = strncat(str, ": ", 2);
  
  cout << setw(74) << left << str;
  if (successful) {
    cout << "passed";
  } else {
    cout << "failed";
    m_failed++;
  }
  cout << endl;  
}
