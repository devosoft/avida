/*
 *  cMutationRates.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cMutationRates_h
#define cMutationRates_h

#ifndef cAvidaContext_h
#include "cAvidaContext.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif


class cMutationRates
{
private:
  // Mutations are divided up by when they occur...

  // ...anytime during execution...
  struct sExecMuts {
    double point_mut_prob;
  };
  sExecMuts exec;

  // ...during an instruction copy...
  struct sCopyMuts {
    double copy_mut_prob;
  };
  sCopyMuts copy;

  // ...at the divide...
  struct sDivideMuts {
    double ins_mut_prob;        // Per site
    double del_mut_prob;        // Per site
    double div_mut_prob;        // Per site
    double divide_mut_prob;     // Max one per divide
    double divide_ins_prob;     // Max one per divide
    double divide_del_prob;     // Max one per divide
    double crossover_prob;
    double aligned_cross_prob;
    double parent_mut_prob;
  };
  sDivideMuts divide;
  
  
  cMutationRates& operator=(const cMutationRates&); // @not_implemented

public:
  cMutationRates() { Clear(); }
  cMutationRates(const cMutationRates& in_muts) { Copy(in_muts); }
  ~cMutationRates() { ; }

  void Clear();
  void Copy(const cMutationRates & in_muts);

  bool TestPointMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(exec.point_mut_prob); }
  bool TestCopyMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(copy.copy_mut_prob); }
  bool TestDivideMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_mut_prob); }
  bool TestDivideIns(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_ins_prob); }
  bool TestDivideDel(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_del_prob); }
  bool TestParentMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.parent_mut_prob); }
  bool TestCrossover(cAvidaContext& ctx) const  { return ctx.GetRandom().P(divide.crossover_prob); }
  bool TestAlignedCrossover(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.aligned_cross_prob); }

  double GetPointMutProb() const     { return exec.point_mut_prob; }
  double GetCopyMutProb() const      { return copy.copy_mut_prob; }
  double GetInsMutProb() const       { return divide.ins_mut_prob; }
  double GetDelMutProb() const       { return divide.del_mut_prob; }
  double GetDivMutProb() const       { return divide.div_mut_prob; }
  double GetDivideMutProb() const    { return divide.divide_mut_prob; }
  double GetDivideInsProb() const    { return divide.divide_ins_prob; }
  double GetDivideDelProb() const    { return divide.divide_del_prob; }
  double GetParentMutProb() const    { return divide.parent_mut_prob; }
  double GetCrossoverProb() const    { return divide.crossover_prob; }
  double GetAlignedCrossProb() const { return divide.aligned_cross_prob; }

  void SetPointMutProb(double in_prob)  { exec.point_mut_prob  = in_prob; }
  void SetCopyMutProb(double in_prob)   { copy.copy_mut_prob   = in_prob; }
  void SetInsMutProb(double in_prob)    { divide.ins_mut_prob    = in_prob; }
  void SetDelMutProb(double in_prob)    { divide.del_mut_prob    = in_prob; }
  void SetDivMutProb(double in_prob)    { divide.div_mut_prob    = in_prob; }
  void SetDivideMutProb(double in_prob) { divide.divide_mut_prob = in_prob; }
  void SetDivideInsProb(double in_prob) { divide.divide_ins_prob = in_prob; }
  void SetDivideDelProb(double in_prob) { divide.divide_del_prob = in_prob; }
  void SetParentMutProb(double in_prob) { divide.parent_mut_prob = in_prob; }
  void SetCrossoverProb(double in_prob) { divide.crossover_prob  = in_prob; }
  void SetAlignedCrossProb(double in)   { divide.aligned_cross_prob = in; }


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
