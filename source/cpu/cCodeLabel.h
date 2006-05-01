/*
 *  cCodeLabel.h
 *  Avida
 *
 *  Created by David on 11/22/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cCodeLabel_h
#define cCodeLabel_h

#ifndef nHardware_h
#include "nHardware.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cTools_h
#include "cTools.h"
#endif

/**
 * The cCodeLabel class is used to identify a label within the genotype of
 * a creature, and aid in its manipulation.
 **/

class cCodeLabel
{
private:
  tArray<char> nop_sequence;
  int size;

public:
  cCodeLabel() : size(0) { ; }
  cCodeLabel(const cCodeLabel& in_label) : nop_sequence(in_label.nop_sequence), size(in_label.size) { ; }  
  ~cCodeLabel() { ; }

  bool OK();
  bool operator==(const cCodeLabel& other_label) const;
  bool operator!=(const cCodeLabel& other_label) const { return !(operator==(other_label)); }
  char operator[](int position) const { return (int) nop_sequence[position]; }
  cCodeLabel& operator=(const cCodeLabel& in_lbl)
  {
    nop_sequence = in_lbl.nop_sequence;
    size = in_lbl.size;
    return *this;
  }
  
  int FindSublabel(cCodeLabel & sub_label);

  void Clear() { size = 0; }
  inline void AddNop(int nop_num);
  inline void Rotate(const int rot, const int base);

  int GetSize() const { return size; }
  //int GetBase() const { return base; }
  inline cString AsString() const;
  int AsInt(const int base) const;
  int AsIntGreyCode(const int base) const;
  int AsIntDirect(const int base) const;
  int AsIntAdditivePolynomial(const int base) const;
  int AsIntFib(const int base) const;
  int AsIntPolynomialCoefficent(const int base) const;

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

void cCodeLabel::AddNop(int nop_num) {
  assert (nop_num < nHardware::MAX_NOPS);

  if (size < nHardware::MAX_LABEL_SIZE) {
    if (size == nop_sequence.GetSize()) {
      nop_sequence.Resize(size+1);
    }
    nop_sequence[size++] = (char) nop_num;
  }
}

void cCodeLabel::Rotate(const int rot, const int base)
{
  for (int i = 0; i < size; i++) {
    nop_sequence[i] += rot;
    if (nop_sequence[i] >= base) nop_sequence[i] -= base;
  }
}


cString cCodeLabel::AsString() const
{
  cString out_string;
  for (int i = 0; i < size; i++) {
    out_string += (char) nop_sequence[i] + 'A';
  }

  return out_string;
}

#endif
