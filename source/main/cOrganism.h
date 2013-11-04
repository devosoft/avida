/*
 *  cOrganism.h
 *  Avida
 *
 *  Called "organism.hh" prior to 12/5/05.
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

#ifndef cOrganism_h
#define cOrganism_h

#include "avida/core/Genome.h"
#include "avida/systematics/Unit.h"

#include "avida/private/systematics/GenomeTestMetrics.h"

#include "cMutationRates.h"
#include "cPhenotype.h"
#include "tBuffer.h"


class cAvidaContext;
class cEnvironment;
class cHardwareBase;

struct sOrgDisplay;

using namespace Avida;


class cOrganism : public Systematics::Unit
{
private:
  cWorld* m_world;
  cPhenotype m_phenotype;                 // Descriptive attributes of organism.
  
  cMutationRates m_mut_rates;             // Rate of all possible mutations.
  int m_id;                               // unique id for each org, is just the number it was born

  int m_org_list_index;
  
  // Input and Output with the environment
  int m_input_pointer;
  tBuffer<int> m_input_buf;
  tBuffer<int> m_output_buf;

  int m_max_executed;      // Max number of instruction executed before death.  

public:
  cOrganism(cWorld* world, cAvidaContext& ctx, const Genome& genome, int parent_generation, Systematics::Source src);
  ~cOrganism();
  

  // --------  Support Methods  --------
  inline double GetTestFitness(cAvidaContext& ctx) const;
  inline double GetTestMerit(cAvidaContext& ctx) const;
  inline double GetTestColonyFitness(cAvidaContext& ctx) const;
  double CalcMeritRatio();

  void NotifyDeath(cAvidaContext& ctx);

  void PrintStatus(std::ostream& fp);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp);
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success);
  void PrintFinalStatus(std::ostream& fp, int time_used, int time_allocated) const;

  // --------  Accessor Methods  --------
  const Genome& GetGenome() const { return m_initial_genome; }
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
  cPhenotype& GetPhenotype() { return m_phenotype; }
  void SetPhenotype(cPhenotype& _in_phenotype) { m_phenotype = _in_phenotype; }

  const cMutationRates& MutationRates() const { return m_mut_rates; }
  cMutationRates& MutationRates() { return m_mut_rates; }

  
  const Apto::Array<double>& GetRBins() const { return m_phenotype.GetCurRBinsAvail(); }
  double GetRBin(int index) { return m_phenotype.GetCurRBinAvail(index); }
  double GetRBinsTotal();
  void SetRBins(const Apto::Array<double>& rbins_in);
  void SetRBin(const int index, const double value);
  void AddToRBin(const int index, const double value);
  void IncCollectSpecCount(const int spec_id);

  int GetMaxExecuted() const { return m_max_executed; }


  inline void SetOrgIndex(int index) { m_org_list_index = index; }
  inline int GetOrgIndex() { return m_org_list_index; }

  
  // --------  cOrgInterface Methods  --------
  int GetID() { return m_id; }


  int GetInputAt(int i) { return m_interface->GetInputAt(i); }
  int GetNextInput() { return m_interface->GetInputAt(m_input_pointer); }
  int GetNextInput(int& in_input_pointer) { return m_interface->GetInputAt(in_input_pointer); }
  tBuffer<int>& GetInputBuf() { return m_input_buf; }
  tBuffer<int>& GetOutputBuf() { return m_output_buf; }
  void Die(cAvidaContext& ctx) { m_interface->Die(ctx); m_is_dead = true; } 
  void UpdateMerit(double new_merit) { m_interface->UpdateMerit(new_merit); }

  void AddLiveOrg() { m_interface->AddLiveOrg(); }
  void RemoveLiveOrg() { m_interface->RemoveLiveOrg(); } 
  
  
  // --------  Input and Output Methods  --------
  void DoInput(const int value);
  void DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);

  /* These different flavors of DoOutput are "frontends" to the main DoOutput
  that follows - One DoOutput to rule them all, etc., etc. */
  //! Check tasks based on the current state of this organism's IO & message buffers.
  void DoOutput(cAvidaContext& ctx, const bool on_divide=false);
  //! Add the passed-in value to this organism's output buffer, and check tasks (on_divide=false).
  void DoOutput(cAvidaContext& ctx, const int value);
  //! Check tasks based on the passed-in IO buffers and value (on_divide=false).
  void DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);    

  void ClearInput() { m_input_buf.Clear(); }
  void ResetInput() { m_input_pointer = 0; m_input_buf.Clear(); };
  void AddOutput(int val) { m_output_buf.Add(val); }

  // --------  Divide Methods  --------
  bool Divide_CheckViable(cAvidaContext& ctx);
  bool ActivateDivide(cAvidaContext& ctx);


  // -------- Movement ---------
public:
  bool Move(cAvidaContext& ctx);

  int GetForageTarget() const { return m_forage_target; }
  int GetShowForageTarget() const { return m_show_ft; }
  void SetForageTarget(cAvidaContext& ctx, int forage_target, bool inject = false);
  void SetPredFT(cAvidaContext& ctx) { SetForageTarget(ctx, -2); }
  void SetTopPredFT(cAvidaContext& ctx) { SetForageTarget(ctx, -3); }
  bool IsPreyFT() { return m_forage_target > -2; }
  bool IsPredFT() { return m_forage_target == -2; }
  bool IsTopPredFT() { return m_forage_target < -2; }
  bool IsMimicFT() { return m_forage_target == 1; }
  void SetShowForageTarget(cAvidaContext& ctx, int forage_target) { m_show_ft = forage_target; }
  bool HasSetFT() const { return m_has_set_ft; }
  void RecordFTSet() { m_has_set_ft = true; }
  bool IsTeacher() const { return m_teach; }
  void Teach(bool teach) { m_teach = teach; }
  bool HadParentTeacher() const { return m_parent_teacher; }
  void SetParentTeacher(bool had_teacher) { m_parent_teacher = had_teacher; }
  void SetParentFT(int parent_ft) { m_parent_ft = parent_ft; }
  int GetParentFT() const { return m_parent_ft; } 
  void CopyParentFT(cAvidaContext& ctx);
  void SetParentMerit(double parent_merit) { m_p_merit = parent_merit; }
  double GetParentMerit() { return m_p_merit; }
  void SetParentMultiThreaded(bool parent_is_mt) { m_p_mthread = parent_is_mt; }
  bool IsParentMThreaded() { return m_p_mthread; }
  
  
protected:

  int m_forage_target;
  int m_show_ft;
  bool m_has_set_ft;
  bool m_teach;
  bool m_parent_teacher;
  int m_parent_ft;
  double m_p_merit;
  bool m_p_mthread;
  

	// -------- Avatar support --------
public:
  bool MoveAV(cAvidaContext& ctx);
  
  
	// -------- Internal Support Methods --------
private:
  void initialize(cAvidaContext& ctx);
  
  
  
private:

  /*! The main DoOutput function.  The DoOutputs above all forward to this function. */
  void doOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const bool on_divide);
  // Need seperate doOutput function for avatars to avoid triggering reactions by true orgs
  void doAVOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const bool on_divide);
};


inline double cOrganism::GetTestFitness(cAvidaContext& ctx) const {
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, SystematicsGroup("genotype"))->GetFitness();
}

inline double cOrganism::GetTestMerit(cAvidaContext& ctx) const {
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, SystematicsGroup("genotype"))->GetMerit();
}

inline double cOrganism::GetTestColonyFitness(cAvidaContext& ctx) const {
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, SystematicsGroup("genotype"))->GetColonyFitness();
}



#endif

