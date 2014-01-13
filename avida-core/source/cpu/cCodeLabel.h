/*
 *  cCodeLabel.h
 *  Avida
 *
 *  Called "code_label.hh" prior to 11/22/05.
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

#ifndef cCodeLabel_h
#define cCodeLabel_h

#include "avida/Avida.h"

#include "cString.h"
#include "nHardware.h"

/**
 * The cCodeLabel class is used to identify a label within the genotype of
 * a creature, and aid in its manipulation.
 **/

class cCodeLabel
{
public:
  static const int MAX_LENGTH;
  
private:
  Apto::Array<char, Apto::Smart> m_nops;

public:
  inline cCodeLabel() { m_nops.SetReserve(MAX_LENGTH); }
  inline cCodeLabel(const cCodeLabel& in_label) : m_nops(in_label.m_nops) { ; }
  ~cCodeLabel() { ; }

  inline bool operator==(const cCodeLabel& other_label) const;
  inline bool operator!=(const cCodeLabel& other_label) const { return !(operator==(other_label)); }
  inline char operator[](int position) const { return (int) m_nops[position]; }
  inline cCodeLabel& operator=(const cCodeLabel& in_lbl) { m_nops = in_lbl.m_nops; return *this; }

  void ReadString(const cString& label_str);
  
  int FindSublabel(const cCodeLabel& sub_label) const;
  inline bool Contains(const cCodeLabel& sub_label) const { return (FindSublabel(sub_label) >= 0); }

  inline void Clear() { m_nops.Resize(0); }
  inline void AddNop(int nop_num);
  inline void Rotate(const int rot, const int base);

  inline int GetSize() const { return m_nops.GetSize(); }
  
  inline cString AsString() const;
  
  int AsInt(const int base) const;
  int AsIntGreyCode(const int base) const;
  int AsIntDirect(const int base) const;
  int AsIntUnique(const int base) const;
  int AsIntAdditivePolynomial(const int base) const;
  int AsIntFib(const int base) const;
  int AsIntPolynomialCoefficent(const int base) const;
};


inline void cCodeLabel::AddNop(int nop_num)
{
  if (m_nops.GetSize() < MAX_LENGTH) {
    m_nops.Push((char)nop_num);
  }
}

inline void cCodeLabel::Rotate(const int rot, const int base)
{
  for (int i = 0; i < m_nops.GetSize(); i++) {
    m_nops[i] += rot;
    if (m_nops[i] >= base) m_nops[i] -= base;
  }
}


inline cString cCodeLabel::AsString() const
{
  cString out_string;
  for (int i = 0; i < m_nops.GetSize(); i++) {
    out_string += m_nops[i] + 'A';
  }

  return out_string;
}


inline bool cCodeLabel::operator==(const cCodeLabel & other_label) const
{
  if (m_nops.GetSize() != other_label.GetSize()) return false;
  
  for (int i = 0; i < m_nops.GetSize(); i++) if (m_nops[i] != other_label[i]) return false;
  
  return true;
}

#endif
