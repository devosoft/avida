/*
 *  cCodeLabel.cc
 *  Avida
 *
 *  Created by David on 11/22/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cCodeLabel.h"

#include "cTools.h"
#include "nHardwareCPU.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

bool cCodeLabel::OK()
{
  bool result = true;

  assert (size <= nHardware::MAX_LABEL_SIZE);
  assert (size <= nop_sequence.GetSize());
  for (int i = 0; i < size; i++) {
    assert (nop_sequence[i] < nHardware::MAX_NOPS);
  }

  return result;
}

bool cCodeLabel::operator==(const cCodeLabel & other_label) const
{
  if (size != other_label.GetSize()) {
    return false;
  }

  for (int i = 0; i < size; i++) {
    if (nop_sequence[i] != other_label[i]) {
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

  for (int offset = 0; offset <= size - sub_label.GetSize(); offset++) {
    for (int i = 0; i < sub_label.GetSize(); i++) {
      if (nop_sequence[i + offset] != sub_label[i]) {
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

  for (int i = 0; i < size; i++) {
    value *= base;
    value += nop_sequence[i];
  }

  return value;
}

int cCodeLabel::AsIntGreyCode(const int base) const
{
  int value = 0;
  int oddCount = 0;

  for (int i = 0; i < size; i++) {
    value *= base;

    if(oddCount % 2 == 0) {
      value += nop_sequence[i];
    } else {
      // @DMB - this should not be relying on hardware specific NUM_NOPS
      value += (nHardwareCPU::NUM_NOPS - 1) - nop_sequence[i];
    }

    if(nop_sequence[i] % 2 == 1) {
      oddCount++;
    }
  }

  return value;
}

int cCodeLabel::AsIntDirect(const int base) const
{
  int value = 0;

  for (int i = 0; i < size; i++) {
    value *= base;
    value += nop_sequence[i];
  }

  return value;
}

int cCodeLabel::AsIntAdditivePolynomial(const int base) const
{
  double value = 0.0;

  for (int i = 0; i < size; i++) {
#if 1
    int n = (int)nop_sequence[i] + 1;
    double a = pow((double)n, 0.4 * (double)(size-1));
    double b = 0.3 * (double)i * (double)(size-1);
    double c = 0.45 * (double)i;
    value += a + b + c;
#else
    value += (pow(((double)nop_sequence[i] + 1.0), (0.4 * (double)(size-1))) +
	      (0.3 * (double)i * (double)(size-1)) +
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

  for (int i = 0; i < size; i++) {
    value += fib[(int)nop_sequence[i]];

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

  int extra = size % 2;
  int c = 1;

  for (int i = 0; i < size - extra; i+=2, c++) {
    int b = nop_sequence[i];
    int a = nop_sequence[i+1];

    value += (int)pow((double)((a * base) + b), c);
  }

  if(extra) {
    value += (int)pow((double)nop_sequence[size-1], c);
  }

  return value;
}
