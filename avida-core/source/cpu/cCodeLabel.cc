/*
 *  cCodeLabel.cc
 *  Avida
 *
 *  Called "code_label.cc" prior to 11/22/05.
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

#include "cCodeLabel.h"


#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;


const int cCodeLabel::MAX_LENGTH = 10;


void cCodeLabel::ReadString(const cString& label_str)
{
  cString lbl(label_str);
  lbl.Trim();
  m_nops.Resize(lbl.GetSize());
  
  for (int i = 0; i < lbl.GetSize(); i++) {
    int nop = lbl[i] - 'A';
    m_nops[i] =  nop;
  }
}


// This function returns true if the sub_label can be found within
// the label affected.
int cCodeLabel::FindSublabel(const cCodeLabel& sub_label) const
{
  bool error = false;

  for (int offset = 0; offset <= m_nops.GetSize() - sub_label.GetSize(); offset++) {
    for (int i = 0; i < sub_label.GetSize(); i++) {
      if (m_nops[i + offset] != sub_label[i]) {
	error = true;
	break;
      }
    }
    if (!error) return offset;
    error = false;
  }

  return -1;
}

/* Translates a code label into an n-ary integer, reading the first nop as 0.
 * Example: nops A, B, C with base 3
 *   no nops = 0
 *   A       = 0
 *   B       = 1
 *   AA      = 0
 *   AC      = 2
 *   BB      = 4
 *   CA      = 6
 */
int cCodeLabel::AsInt(const int base) const
{
  int value = 0;

  for (int i = 0; i < m_nops.GetSize(); i++) {
    value *= base;
    value += m_nops[i];
  }

  return value;
}

int cCodeLabel::AsIntGreyCode(const int base) const
{
  int value = 0;
  int oddCount = 0;

  for (int i = 0; i < m_nops.GetSize(); i++) {
    value *= base;

    if(oddCount % 2 == 0) {
      value += m_nops[i];
    } else {
      value += (base - 1) - m_nops[i];
    }

    if(m_nops[i] % 2 == 1) {
      oddCount++;
    }
  }

  return value;
}

int cCodeLabel::AsIntDirect(const int base) const
{
  int value = 0;
  
  for (int i = 0; i < m_nops.GetSize(); i++) {
    value *= base;
    value += m_nops[i];
  }

  return value;
}

/* Translates a code label into a unique integer (given a base >= the number of nop types)
 * Example: nops A, B, C with base 3
 *   no nops = 0
 *   A       = 1
 *   B       = 2
 *   AA      = 4
 *   AC      = 6
 *   BB      = 8
 *   CA      = 12
 *
 * N.B.: Uniqueness will NOT be true if base < # of nop types
 */
int cCodeLabel::AsIntUnique(const int base) const
{
  int value = 0;
  
  for (int i = 0; i < m_nops.GetSize(); i++) {
    value *= base;
    value += m_nops[i] + 1;
  }
  
  return value;
}

int cCodeLabel::AsIntAdditivePolynomial(const int) const
{
  double value = 0.0;

  for (int i = 0; i < m_nops.GetSize(); i++) {
#if 1
    int n = (int)m_nops[i] + 1;
    double a = pow((double)n, 0.4 * (double)(m_nops.GetSize() - 1));
    double b = 0.3 * (double)i * (double)(m_nops.GetSize() - 1);
    double c = 0.45 * (double)i;
    value += a + b + c;
#else
    value += (pow(((double)m_nops[i] + 1.0), (0.4 * (double)(m_nops.GetSize() - 1))) +
	      (0.3 * (double)i * (double)(m_nops.GetSize() - 1)) +
	      (0.45 * (double)i));
#endif
  }

  return (int)(value + 0.5);
}

int cCodeLabel::AsIntFib(const int base) const
{
  int value = 0;
  if(base < 3) { return 0; }

  vector<int> fib;
  fib.resize(base, 0);
  fib[0] = 0;
  fib[1] = 1;
  fib[2] = 2;
  for(int i=3; i<base; i++) {
    fib[i] = fib[i-2] + fib[i-1];
  }

  for (int i = 0; i < m_nops.GetSize(); i++) {
    value += fib[(int)m_nops[i]];

    fib[2] = fib[base-2] + fib[base-1];
    fib[1] = fib[base-1];
    for(int j=3; j<base; j++) {
      fib[j] = fib[j-2] + fib[j-1];
    }
  }

  return value;
}

int cCodeLabel::AsIntPolynomialCoefficent(const int base) const
{
  int value = 0;

  int extra = m_nops.GetSize() % 2;
  int c = 1;

  for (int i = 0; i < m_nops.GetSize() - extra; i+=2, c++) {
    int b = m_nops[i];
    int a = m_nops[i+1];

    value += (int)pow((double)((a * base) + b), c);
  }

  if(extra) {
    value += (int)pow((double)m_nops[m_nops.GetSize() - 1], c);
  }

  return value;
}
