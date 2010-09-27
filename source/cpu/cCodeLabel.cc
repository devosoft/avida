/*
 *  cCodeLabel.cc
 *  Avida
 *
 *  Called "code_label.cc" prior to 11/22/05.
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

#include "cCodeLabel.h"


#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;


void cCodeLabel::ReadString(const cString& label_str)
{
  cString lbl(label_str);
  lbl.Trim();
  m_size = lbl.GetSize();
  m_nops.Resize(lbl.GetSize());
  
  for (int i = 0; i < lbl.GetSize(); i++) {
    int nop = lbl[i] - 'A';
    if (nop < 0 || nop >= nHardware::MAX_NOPS) {
      // on invalid nop, terminate sequence
      m_size = i;
      m_nops.Resize(i);
      break;
    }
    m_nops[i] =  nop;
  }
}

bool cCodeLabel::OK()
{
  bool result = true;

  assert (m_size <= nHardware::MAX_LABEL_SIZE);
  assert (m_size <= m_nops.GetSize());
  for (int i = 0; i < m_size; i++) {
    assert (m_nops[i] < nHardware::MAX_NOPS);
  }

  return result;
}

bool cCodeLabel::operator==(const cCodeLabel & other_label) const
{
  if (m_size != other_label.GetSize()) {
    return false;
  }

  for (int i = 0; i < m_size; i++) {
    if (m_nops[i] != other_label[i]) {
      return false;
    }
  }

  return true;
}


// This function returns true if the sub_label can be found within
// the label affected.
int cCodeLabel::FindSublabel(cCodeLabel & sub_label)
{
  bool error = false;

  for (int offset = 0; offset <= m_size - sub_label.GetSize(); offset++) {
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

int cCodeLabel::AsInt(const int base) const
{
  int value = 0;

  for (int i = 0; i < m_size; i++) {
    value *= base;
    value += m_nops[i];
  }

  return value;
}

int cCodeLabel::AsIntGreyCode(const int base) const
{
  int value = 0;
  int oddCount = 0;

  for (int i = 0; i < m_size; i++) {
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
  
  for (int i = 0; i < m_size; i++) {
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
  
  for (int i = 0; i < m_size; i++) {
    value *= base;
    value += m_nops[i] + 1;
  }
  
  return value;
}

int cCodeLabel::AsIntAdditivePolynomial(const int base) const
{
  double value = 0.0;

  for (int i = 0; i < m_size; i++) {
#if 1
    int n = (int)m_nops[i] + 1;
    double a = pow((double)n, 0.4 * (double)(m_size-1));
    double b = 0.3 * (double)i * (double)(m_size-1);
    double c = 0.45 * (double)i;
    value += a + b + c;
#else
    value += (pow(((double)m_nops[i] + 1.0), (0.4 * (double)(m_size-1))) +
	      (0.3 * (double)i * (double)(m_size-1)) +
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

  for (int i = 0; i < m_size; i++) {
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

  int extra = m_size % 2;
  int c = 1;

  for (int i = 0; i < m_size - extra; i+=2, c++) {
    int b = m_nops[i];
    int a = m_nops[i+1];

    value += (int)pow((double)((a * base) + b), c);
  }

  if(extra) {
    value += (int)pow((double)m_nops[m_size-1], c);
  }

  return value;
}
