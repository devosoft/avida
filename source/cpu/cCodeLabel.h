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
  tArray<char> m_nops;
  int m_size;

public:
  cCodeLabel() : m_size(0) { ; }
  cCodeLabel(const cCodeLabel& in_label) : m_nops(in_label.m_nops), m_size(in_label.m_size) { ; }  
  ~cCodeLabel() { ; }

  bool OK();
  bool operator==(const cCodeLabel& other_label) const;
  bool operator!=(const cCodeLabel& other_label) const { return !(operator==(other_label)); }
  char operator[](int position) const { return (int) m_nops[position]; }
  cCodeLabel& operator=(const cCodeLabel& in_lbl)
  {
    m_nops = in_lbl.m_nops;
    m_size = in_lbl.m_size;
    return *this;
  }

  void ReadString(const cString& label_str);
  
  int FindSublabel(cCodeLabel& sub_label);

  void Clear() { m_size = 0; }
  inline void AddNop(int nop_num);
  inline void Rotate(const int rot, const int base);

  int GetSize() const { return m_size; }
  inline cString AsString() const;
  int AsInt(const int base) const;
  int AsIntGreyCode(const int base) const;
  int AsIntDirect(const int base) const;
  int AsIntAdditivePolynomial(const int base) const;
  int AsIntFib(const int base) const;
  int AsIntPolynomialCoefficent(const int base) const;
};


#ifdef ENABLE_UNIT_TESTS
namespace nCodeLabel {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

void cCodeLabel::AddNop(int nop_num) {
  assert (nop_num < nHardware::MAX_NOPS);

  if (m_size < nHardware::MAX_LABEL_SIZE) {
    if (m_size == m_nops.GetSize()) {
      m_nops.Resize(m_size + 1);
    }
    m_nops[m_size++] = (char) nop_num;
  }
}

void cCodeLabel::Rotate(const int rot, const int base)
{
  for (int i = 0; i < m_size; i++) {
    m_nops[i] += rot;
    if (m_nops[i] >= base) m_nops[i] -= base;
  }
}


cString cCodeLabel::AsString() const
{
  cString out_string;
  for (int i = 0; i < m_size; i++) {
    out_string += (char) m_nops[i] + 'A';
  }

  return out_string;
}

#endif
