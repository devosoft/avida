//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CODE_LABEL_HH
#define CODE_LABEL_HH

#ifndef CPU_DEFS_HH
#include "cpu_defs.hh"        // #defines only
#endif
#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TARRAY_HH
#include "tArray.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

/**
 * The cCodeLabel class is used to identify a label within the genotype of
 * a creature, and aid in its manipulation.
 **/

class cCodeLabel {
private:
  tArray<char> nop_sequence;
  int size;
  //const int base;
public:
  cCodeLabel() : size(0) { ; }
  cCodeLabel(const cCodeLabel& in_label) : nop_sequence(in_label.nop_sequence), size(in_label.size) { ; }  
  ~cCodeLabel() { ; }

  bool OK();
  bool operator==(const cCodeLabel & other_label) const;
  bool operator!=(const cCodeLabel & other_label) const
    { return !(operator==(other_label)); }
  char operator[](int position) const { return (int) nop_sequence[position]; }
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

  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
};

void cCodeLabel::AddNop(int nop_num) {
  assert (nop_num < MAX_NOPS);

  if (size < MAX_LABEL_SIZE) {
    if (size == nop_sequence.GetSize()) {
      nop_sequence.Resize(size+1);
    }
    nop_sequence[size++] = (char) nop_num;
  }
}

void cCodeLabel::Rotate(const int rot, const int base)
{
  //for (int i = 0; i < size; i++) {
  //  nop_sequence[i] += rot;
  //  if (nop_sequence[i] == 3) nop_sequence[i]++; //IGNORING NOP-D FOR NOW!
  //  if (nop_sequence[i] >= base) nop_sequence[i] -= base;
  //}
  for (int i = 0; i < size; i++) {
    nop_sequence[i] += rot;
    //if (nop_sequence[i] == 3) nop_sequence[i]++; //IGNORING NOP-D FOR NOW!
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
