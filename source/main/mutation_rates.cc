//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_RATES_HH
#include "mutation_rates.hh"
#endif

#ifndef TOOLS_HH
#include "tools.hh"
#endif

////////////////////
//  cMutationRates
////////////////////

cMutationRates::cMutationRates()
{
  Clear();
}

cMutationRates::cMutationRates(const cMutationRates & in_muts)
{
  Copy(in_muts);
}

cMutationRates::~cMutationRates()
{
}

void cMutationRates::Clear()
{
  exec.point_mut_prob = 0.0;
  copy.copy_mut_prob = 0.0;
  divide.ins_mut_prob = 0.0;
  divide.del_mut_prob = 0.0;
  divide.div_mut_prob = 0.0;
  divide.divide_mut_prob = 0.0;
  divide.divide_ins_prob = 0.0;
  divide.divide_del_prob = 0.0;
  divide.parent_mut_prob = 0.0;
  divide.crossover_prob = 0.0;
  divide.aligned_cross_prob = 0.0;
}

void cMutationRates::Copy(const cMutationRates & in_muts)
{
  exec.point_mut_prob = in_muts.exec.point_mut_prob;
  copy.copy_mut_prob = in_muts.copy.copy_mut_prob;
  divide.ins_mut_prob = in_muts.divide.ins_mut_prob;
  divide.del_mut_prob = in_muts.divide.del_mut_prob;
  divide.div_mut_prob = in_muts.divide.div_mut_prob;
  divide.divide_mut_prob = in_muts.divide.divide_mut_prob;
  divide.divide_ins_prob = in_muts.divide.divide_ins_prob;
  divide.divide_del_prob = in_muts.divide.divide_del_prob;
  divide.parent_mut_prob = in_muts.divide.parent_mut_prob;
  divide.crossover_prob = in_muts.divide.crossover_prob;
  divide.aligned_cross_prob = in_muts.divide.aligned_cross_prob;

  //  if (copy.copy_mut_prob != 0) cerr << "Copying non-zero copy mut rate!" << endl;
}

bool cMutationRates::TestPointMut() const
{
  return g_random.P(exec.point_mut_prob);
}

bool cMutationRates::TestCopyMut() const
{
  return g_random.P(copy.copy_mut_prob);
}

bool cMutationRates::TestDivideMut() const
{
  return g_random.P(divide.divide_mut_prob);
}

bool cMutationRates::TestDivideIns() const
{
  return g_random.P(divide.divide_ins_prob);
}

bool cMutationRates::TestDivideDel() const
{
  return g_random.P(divide.divide_del_prob);
}

bool cMutationRates::TestParentMut() const
{
  return g_random.P(divide.parent_mut_prob);
}

bool cMutationRates::TestCrossover() const
{
  return g_random.P(divide.crossover_prob);
}

bool cMutationRates::TestAlignedCrossover() const
{
  return g_random.P(divide.aligned_cross_prob);
}
