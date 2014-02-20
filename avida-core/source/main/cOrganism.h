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
  void PrintFinalStatus(std::ostream& fp, int time_used, int time_allocated) const;

  // --------  Accessor Methods  --------
  const Genome& GetGenome() const { return m_initial_genome; }
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
  cPhenotype& GetPhenotype() { return m_phenotype; }
  void SetPhenotype(cPhenotype& _in_phenotype) { m_phenotype = _in_phenotype; }

  const cMutationRates& MutationRates() const { return m_mut_rates; }
  cMutationRates& MutationRates() { return m_mut_rates; }

  
  int GetMaxExecuted() const { return m_max_executed; }


  inline void SetOrgIndex(int index) { m_org_list_index = index; }
  inline int GetOrgIndex() { return m_org_list_index; }

  
  // --------  cOrgInterface Methods  --------
  int GetID() { return m_id; }


  void Die(cAvidaContext& ctx) { m_interface->Die(ctx); m_is_dead = true; }
  void UpdateMerit(double new_merit) { m_interface->UpdateMerit(new_merit); }

  
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

