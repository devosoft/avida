//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MX_CODE_ARRAY_HH
#define MX_CODE_ARRAY_HH

#include <iostream>
#include <list>
#include <map>
#include <set>

#ifndef INSTRUCTION_HH
#include "instruction.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif

/**
 * This class stores the genome of an Avida organism only.
 * Cannot be run in a simulation.
 **/

class cGenome;
class MyCodeArrayLessThan;
class cMxCodeArray {
private:
  int size;
  int max_size;
  cInstruction * data;
  double m_merit;
  int m_gestation_time;
  static const int m_max_gestation_time;
  int m_num_instructions;
  mutable std::map<int, double, std::less<int> > m_trans_probs;


public:
  cMxCodeArray();
  explicit cMxCodeArray(int ninst, int in_size=0, int in_max_size=0);
  cMxCodeArray(const cMxCodeArray &in_code);
  cMxCodeArray(const cGenome & in_code, int in_max_size=0);
  virtual ~cMxCodeArray();

  void operator=(const cMxCodeArray &other_code);
  bool operator==(const cMxCodeArray &other_code) const;
  inline cInstruction & operator[](int index);
  bool operator<(const cMxCodeArray &other_code) const;

  void CopyDataTo(cGenome & other_code) const;
  void Resize(int new_size);
  void ResetSize(int new_size, int new_max=0);
  inline int GetSize() const { return size; }
  inline double GetFitness() const {
    if ( m_gestation_time == m_max_gestation_time )
      return 0;
    else return m_merit/m_gestation_time;}
  double GetMerit() const { return m_merit; }
  double GetGestationTime() const { return m_gestation_time; }
  void CalcFitness();

  inline const cInstruction & Get(int index) const {
    assert(index < size);
    return data[index];
  }

  int FindInstruction(const cInstruction & inst, int start_index=0);
  inline bool HasInstruction(const cInstruction & inst){
    return ( FindInstruction(inst) >= 0 ) ? true : false; }

  inline void Set(int index, const cInstruction & value);

  void Reset();
  void Clear();
  void Randomize();
  bool OK() const;
  cString AsString() const;

  cString DebugType() { return "cMxCodeArray"; }
  cString DebugInfo() { return cStringUtil::Stringf("size = %d", size); }
  int DebugSize() { return max_size; }



  /* For Depth Limited Search */
  void Get1Mutants(std::list<cMxCodeArray>& mutants) const;
  void SetNumInstructions(int ninst) {m_num_instructions = ninst;}


  /* For Transition Matrix */
  //int MakeTransitionList(const std::set<cMxCodeArray, MyCodeArrayLessThan >& m_data_set,
  /* dropped const for win32 -- kgn */
  int MakeTransitionList(std::set<cMxCodeArray, MyCodeArrayLessThan >& m_data_set,
                          int hamDistThresh, double errorRate) const;
  const std::map<int, double, std::less<int> >& GetTransitionList() const;
  int HammingDistance(const cMxCodeArray &other_gene) const;
  double TransitionProbability(const cMxCodeArray &other_gene, double errorRate) const;
  void PrintTransitionList(std::ostream &fp, int size) const;
};

cInstruction & cMxCodeArray::operator[](int index)
{
#ifdef DEBUG
  assert(index < size);
  assert(index >= 0);
#endif
  return data[index];
}

inline bool cMxCodeArray::operator==(const cMxCodeArray &other_code) const
{
  // Make sure the sizes are the same.
  if (size != other_code.size) return false;

  // Then go through line by line.
  for (int i = 0; i < size; i++)
    if (data[i].GetOp() != other_code.data[i].GetOp()) return false;

  return true;
}

inline void cMxCodeArray::Set(int index, const cInstruction & value)
{
  operator[](index) = value;
}

#endif
