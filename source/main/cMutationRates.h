/*
 *  cMutationRates.h
 *  Avida
 *
 *  Called "mutation_rates.hh" prior to 12/5/05.
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

#ifndef cMutationRates_h
#define cMutationRates_h

#ifndef cAvidaContext_h
#include "cAvidaContext.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif

class cWorld;

class cMutationRates
{
private:
  // Mutations are divided up by when they occur...

  // ...during an instruction copy...
  struct sCopyMuts {
    double mut_prob;
    double ins_prob;
    double del_prob;
    double uniform_prob;
    double slip_prob;
  };
  sCopyMuts copy;

  // ...at the divide...
  struct sDivideMuts {
    double ins_prob;                  // Per site
    double del_prob;                  // Per site
    double mut_prob;                  // Per site
    double uniform_prob;              // Per site
    double slip_prob;                 // Per site
    double divide_mut_prob;           // Max one per divide
    double divide_ins_prob;           // Max one per divide
    double divide_del_prob;           // Max one per divide
    double divide_poisson_mut_mean;   // Allows multiple with constant genomic rate
    double divide_poisson_ins_mean;   // Allows multiple with constant genomic rate
    double divide_poisson_del_mean;   // Allows multiple with constant genomic rate
    double divide_poisson_slip_mean;  // Allows multiple with constant genomic rate
    double divide_slip_prob;          // Max one per divide
    double divide_uniform_prob;
    double parent_mut_prob;
  };
  sDivideMuts divide;
  
  // ...at inject...
  struct sInjectMuts {
    double ins_prob;        // Per site
    double del_prob;        // Per site
    double mut_prob;        // Per site
  };
  sInjectMuts inject;
  
  // Mutations in mutation rates...
  struct sMetaMuts {
    double copy_mut_prob;  // Prob of copy mut changing.
    double standard_dev;   // Standard dev. on meta muts.
  };
  sMetaMuts meta;
  
  struct sUpdateMuts {
    double death_prob;    
  };
  sUpdateMuts update;

public:
  cMutationRates() { Clear(); }
  cMutationRates(const cMutationRates& in_muts) { Copy(in_muts); }
  cMutationRates& operator=(const cMutationRates& in_muts) { Copy(in_muts); return *this; }
  ~cMutationRates() { ; }

  void Setup(cWorld* world);
  void Clear();
  void Copy(const cMutationRates& in_muts);

  // Copy muts should always check if they are 0.0 before consulting the random number generator for performance
  bool TestCopyMut(cAvidaContext& ctx) const { return (copy.mut_prob == 0.0) ? false : ctx.GetRandom().P(copy.mut_prob); }
  bool TestCopyIns(cAvidaContext& ctx) const { return (copy.ins_prob == 0.0) ? false : ctx.GetRandom().P(copy.ins_prob); }
  bool TestCopyDel(cAvidaContext& ctx) const { return (copy.del_prob == 0.0) ? false : ctx.GetRandom().P(copy.del_prob); }
  bool TestCopySlip(cAvidaContext& ctx) const { return (copy.slip_prob == 0.0) ? false : ctx.GetRandom().P(copy.slip_prob); }
  bool TestCopyUniform(cAvidaContext& ctx) const
  {
    return (copy.uniform_prob == 0.0) ? false : ctx.GetRandom().P(copy.uniform_prob);
  }
  
  bool TestDivideMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_mut_prob); }
  bool TestDivideIns(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_ins_prob); }
  bool TestDivideDel(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_del_prob); }
  
  unsigned int NumDividePoissonMut(cAvidaContext& ctx) const 
    { return (divide.divide_poisson_mut_mean == 0.0) ? 0 : ctx.GetRandom().GetRandPoisson(divide.divide_poisson_mut_mean); }
  unsigned int NumDividePoissonIns(cAvidaContext& ctx) const 
    { return (divide.divide_poisson_ins_mean == 0.0) ? 0 : ctx.GetRandom().GetRandPoisson(divide.divide_poisson_ins_mean); }
  unsigned int NumDividePoissonDel(cAvidaContext& ctx) const 
    { return (divide.divide_poisson_del_mean == 0.0) ? 0 : ctx.GetRandom().GetRandPoisson(divide.divide_poisson_del_mean); }
  unsigned int NumDividePoissonSlip(cAvidaContext& ctx) const 
    { return (divide.divide_poisson_slip_mean == 0.0) ? 0 : ctx.GetRandom().GetRandPoisson(divide.divide_poisson_slip_mean); }

  bool TestDivideSlip(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_slip_prob); }
  bool TestDivideUniform(cAvidaContext& ctx) const
  {
    return (divide.divide_uniform_prob == 0.0) ? false : ctx.GetRandom().P(divide.divide_uniform_prob);
  }

  
  bool TestParentMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.parent_mut_prob); }  
  
  double DoMetaCopyMut(cAvidaContext& ctx) {
    if (meta.copy_mut_prob == 0.0 || !ctx.GetRandom().P(meta.copy_mut_prob)) return 1.0;
    const double exp = ctx.GetRandom().GetRandNormal() * meta.standard_dev;
    const double change = pow(2.0, exp);
    copy.mut_prob *= change;
    return change;
  }

  bool TestDeath(cAvidaContext& ctx) const { return (update.death_prob == 0.0) ? false : ctx.GetRandom().P(update.death_prob); }

  
  double GetCopyMutProb() const       { return copy.mut_prob; }
  double GetCopyInsProb() const       { return copy.ins_prob; }
  double GetCopyDelProb() const       { return copy.del_prob; }
  double GetCopyUniformProb() const   { return copy.uniform_prob; }
  double GetCopySlipProb() const      { return copy.slip_prob; }
  
  double GetDivInsProb() const        { return divide.ins_prob; }
  double GetDivDelProb() const        { return divide.del_prob; }
  double GetDivMutProb() const        { return divide.mut_prob; }
  double GetDivUniformProb() const    { return divide.uniform_prob; }
  double GetDivSlipProb() const       { return divide.slip_prob; }
  
  double GetDivideMutProb() const     { return divide.divide_mut_prob; }
  double GetDivideInsProb() const     { return divide.divide_ins_prob; }
  double GetDivideDelProb() const     { return divide.divide_del_prob; }
  double GetDivideUniformProb() const { return divide.divide_uniform_prob; }
  double GetDivideSlipProb() const    { return divide.divide_slip_prob; }
  
  double GetParentMutProb() const     { return divide.parent_mut_prob; }
  
  double GetInjectInsProb() const     { return inject.ins_prob; }
  double GetInjectDelProb() const     { return inject.del_prob; }
  double GetInjectMutProb() const     { return inject.mut_prob; }
  
  double GetMetaCopyMutProb() const   { return meta.copy_mut_prob; }
  double GetMetaStandardDev() const   { return meta.standard_dev; }
  
  double GetDeathProb() const         { return update.death_prob; }

  
  void SetCopyMutProb(double in_prob)       { copy.mut_prob = in_prob; }
  void SetCopyInsProb(double in_prob)       { copy.ins_prob = in_prob; }
  void SetCopyDelProb(double in_prob)       { copy.del_prob = in_prob; }
  void SetCopyUniformProb(double in_prob)   { copy.uniform_prob = in_prob; }
  void SetCopySlipProb(double in_prob)      { copy.slip_prob = in_prob; }
  
  void SetDivMutProb(double in_prob)        { divide.mut_prob = in_prob; }
  void SetDivInsProb(double in_prob)        { divide.ins_prob = in_prob; }
  void SetDivDelProb(double in_prob)        { divide.del_prob = in_prob; }
  void SetDivUniformProb(double in_prob)    { divide.uniform_prob = in_prob; }
  void SetDivSlipProb(double in_prob)       { divide.slip_prob = in_prob; }
  
  void SetDivideMutProb(double in_prob)     { divide.divide_mut_prob = in_prob; }
  void SetDivideInsProb(double in_prob)     { divide.divide_ins_prob = in_prob; }
  void SetDivideDelProb(double in_prob)     { divide.divide_del_prob = in_prob; }
  void SetDivideUniformProb(double in_prob) { divide.divide_del_prob = in_prob; }
  void SetDivideSlipProb(double in_prob)    { divide.divide_del_prob = in_prob; }
  
  void SetParentMutProb(double in_prob)     { divide.parent_mut_prob = in_prob; }
  
  void SetInjectInsProb(double in_prob)     { inject.ins_prob        = in_prob; }
  void SetInjectDelProb(double in_prob)     { inject.del_prob        = in_prob; }
  void SetInjectMutProb(double in_prob)     { inject.mut_prob        = in_prob; }
  
  void SetMetaCopyMutProb(double in_prob)   { meta.copy_mut_prob   = in_prob; }
  void SetMetaStandardDev(double in_dev)    { meta.standard_dev     = in_dev; }

  void SetDeathProb(double in_prob)         { update.death_prob      = in_prob; }
};

#endif
