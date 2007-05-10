/*
 *  cMutationRates.h
 *  Avida
 *
 *  Called "mutation_rates.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

  // ...anytime during execution...
  struct sExecMuts {
    double point_mut_prob;
  };
  sExecMuts exec;

  // ...during an instruction copy...
  struct sCopyMuts {
    double mut_prob;
  };
  sCopyMuts copy;

  // ...at the divide...
  struct sDivideMuts {
    double ins_prob;        // Per site
    double del_prob;        // Per site
    double mut_prob;        // Per site
    double divide_mut_prob;     // Max one per divide
    double divide_ins_prob;     // Max one per divide
    double divide_del_prob;     // Max one per divide
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

  cMutationRates& operator=(const cMutationRates&); // @not_implemented

public:
  cMutationRates() { Clear(); }
  cMutationRates(const cMutationRates& in_muts) { Copy(in_muts); }
  ~cMutationRates() { ; }

  void Setup(cWorld* world);
  void Clear();
  void Copy(const cMutationRates& in_muts);

  bool TestPointMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(exec.point_mut_prob); }
  bool TestCopyMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(copy.mut_prob); }
  bool TestCopyIns(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.ins_prob); }
  bool TestCopyDel(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.del_prob); }
  bool TestDivideMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_mut_prob); }
  bool TestDivideIns(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_ins_prob); }
  bool TestDivideDel(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.divide_del_prob); }
  bool TestParentMut(cAvidaContext& ctx) const { return ctx.GetRandom().P(divide.parent_mut_prob); }
  double DoMetaCopyMut(cAvidaContext& ctx) {
    if (ctx.GetRandom().P(meta.copy_mut_prob) == false) return 1.0;
    const double exp = ctx.GetRandom().GetRandNormal() * meta.standard_dev;
    const double change = pow(2.0, exp);
    copy.mut_prob *= change;
    return change;
  }

  double GetPointMutProb() const     { return exec.point_mut_prob; }
  double GetCopyMutProb() const      { return copy.mut_prob; }
  double GetInsMutProb() const       { return divide.ins_prob; }
  double GetDelMutProb() const       { return divide.del_prob; }
  double GetDivMutProb() const       { return divide.mut_prob; }
  double GetDivideMutProb() const    { return divide.divide_mut_prob; }
  double GetDivideInsProb() const    { return divide.divide_ins_prob; }
  double GetDivideDelProb() const    { return divide.divide_del_prob; }
  double GetParentMutProb() const    { return divide.parent_mut_prob; }
  double GetInjectInsProb() const    { return inject.ins_prob; }
  double GetInjectDelProb() const    { return inject.del_prob; }
  double GetInjectMutProb() const    { return inject.mut_prob; }
  double GetMetaCopyMutProb() const  { return meta.copy_mut_prob; }
  double GetMetaStandardDev() const  { return meta.standard_dev; }
  
  void SetPointMutProb(double in_prob)  { exec.point_mut_prob    = in_prob; }
  void SetCopyMutProb(double in_prob)   { copy.mut_prob          = in_prob; }
  void SetInsMutProb(double in_prob)    { divide.ins_prob        = in_prob; }
  void SetDelMutProb(double in_prob)    { divide.del_prob        = in_prob; }
  void SetDivMutProb(double in_prob)    { divide.mut_prob        = in_prob; }
  void SetDivideMutProb(double in_prob) { divide.divide_mut_prob = in_prob; }
  void SetDivideInsProb(double in_prob) { divide.divide_ins_prob = in_prob; }
  void SetDivideDelProb(double in_prob) { divide.divide_del_prob = in_prob; }
  void SetParentMutProb(double in_prob) { divide.parent_mut_prob = in_prob; }
  void SetInjectInsProb(double in_prob) { inject.ins_prob        = in_prob; }
  void SetInjectDelProb(double in_prob) { inject.del_prob        = in_prob; }
  void SetInjectMutProb(double in_prob) { inject.mut_prob        = in_prob; }
  void SetMetaCopyMutProb(double in_prob) { meta.copy_mut_prob   = in_prob; }
  void SetMetaStandardDev(double in_dev) { meta.standard_dev     = in_dev; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nMutationRates {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
