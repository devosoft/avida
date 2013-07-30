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
#include "cOrgInterface.h"
#include "tBuffer.h"
#include "tList.h"

#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <map>

class cAvidaContext;
class cBioGroup;
class cContextPhenotype;
class cEnvironment;
class cHardwareBase;
class cInstSet;

struct sOrgDisplay;

using namespace Avida;


class cOrganism : public Systematics::Unit
{
private:
  cWorld* m_world;
  cHardwareBase* m_hardware;              // The actual machinery running this organism.
  cPhenotype m_phenotype;                 // Descriptive attributes of organism.
  Systematics::Source m_src;
  
  const Genome m_initial_genome;         // Initial genome; can never be changed!
  Apto::Array<Systematics::UnitPtr> m_parasites;   // List of all parasites associated with this organism.
  cMutationRates m_mut_rates;             // Rate of all possible mutations.
  cOrgInterface* m_interface;             // Interface back to the population.
  int m_id;                               // unique id for each org, is just the number it was born

  int m_org_list_index;
  
  sOrgDisplay* m_org_display;
  sOrgDisplay* m_queued_display_data;
  bool m_display;
  
  // Other stats
  Genome m_offspring_genome;              // Child genome, while under construction.

  // Input and Output with the environment
  int m_input_pointer;
  tBuffer<int> m_input_buf;
  tBuffer<int> m_output_buf;

  int m_max_executed;      // Max number of instruction executed before death.  
  bool m_is_running;       // Does this organism have the CPU?
  bool m_is_dead;          // Is this organism dead?

  cOrganism(); // @not_implemented
  cOrganism(const cOrganism&); // @not_implemented
  cOrganism& operator=(const cOrganism&); // @not_implemented

public:
  cOrganism(cWorld* world, cAvidaContext& ctx, const Genome& genome, int parent_generation, Systematics::Source src);
  ~cOrganism();
  
  static void Initialize();
  
  
  // --------  Systematics::Unit Methods  --------
  Systematics::Source UnitSource() const { return m_src; }
  const Genome& UnitGenome() const { return m_initial_genome; }
  
  const PropertyMap& Properties() const;
  

  // --------  Support Methods  --------
  inline double GetTestFitness(cAvidaContext& ctx) const;
  inline double GetTestMerit(cAvidaContext& ctx) const;
  inline double GetTestColonyFitness(cAvidaContext& ctx) const;
  double CalcMeritRatio();

  void HardwareReset(cAvidaContext& ctx);
  void NotifyDeath(cAvidaContext& ctx);

  void PrintStatus(std::ostream& fp);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp);
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success);
  void PrintFinalStatus(std::ostream& fp, int time_used, int time_allocated) const;

  void NewTrial();

  // --------  Accessor Methods  --------
  const Genome& GetGenome() const { return m_initial_genome; }
  const cPhenotype& GetPhenotype() const { return m_phenotype; }
  cPhenotype& GetPhenotype() { return m_phenotype; }
  void SetPhenotype(cPhenotype& _in_phenotype) { m_phenotype = _in_phenotype; }

  const cMutationRates& MutationRates() const { return m_mut_rates; }
  cMutationRates& MutationRates() { return m_mut_rates; }

  const cOrgInterface& GetOrgInterface() const { assert(m_interface); return *m_interface; }
  cOrgInterface& GetOrgInterface() { assert(m_interface); return *m_interface; }
  void SetOrgInterface(cAvidaContext& ctx, cOrgInterface* org_interface);

  const Apto::Array<double>& GetRBins() const { return m_phenotype.GetCurRBinsAvail(); }
  double GetRBin(int index) { return m_phenotype.GetCurRBinAvail(index); }
  double GetRBinsTotal();
  void SetRBins(const Apto::Array<double>& rbins_in);
  void SetRBin(const int index, const double value);
  void AddToRBin(const int index, const double value);
  void IncCollectSpecCount(const int spec_id);

  int GetMaxExecuted() const { return m_max_executed; }

  Genome& OffspringGenome() { return m_offspring_genome; }
  const Genome& OffspringGenome() const { return m_offspring_genome; }

  void SetRunning(bool in_running) { m_is_running = in_running; }
  bool IsRunning() { return m_is_running; }

  bool IsDead() { return m_is_dead; }

  double GetVitality() const;

  inline void SetOrgIndex(int index) { m_org_list_index = index; }
  inline int GetOrgIndex() { return m_org_list_index; }
  
  // Org displaying
  inline void ActivateDisplay() { m_display = true; }
  inline void KillDisplay() { m_display = false; } 
  inline bool IsDisplaying() { return m_display; }
  inline void SetOrgDisplay(sOrgDisplay* org_display) { m_org_display = org_display; }
  inline void SetPotentialDisplay(sOrgDisplay* new_data) { m_queued_display_data = new_data; }
  bool UpdateOrgDisplay();
  inline sOrgDisplay* GetOrgDisplayData() { return m_org_display; }
  inline sOrgDisplay* GetPotentialDisplayData() { return m_queued_display_data; }
  void SetSimpDisplay(int display_part, int value);
  
  // --------  cOrgInterface Methods  --------
  cHardwareBase& GetHardware() { return *m_hardware; }
  const cHardwareBase& GetHardware() const { return *m_hardware; }
  int GetID() { return m_id; }

  int GetCellID() { return m_interface->GetCellID(); }
  int GetAVCellID() { return m_interface->GetAVCellID(); }
  
  cOrganism* GetNeighbor() { return m_interface->GetNeighbor(); }
  bool IsNeighborCellOccupied() { return m_interface->IsNeighborCellOccupied(); }
  int GetNeighborhoodSize() { return m_interface->GetNumNeighbors(); }
  int GetFacing() { assert(m_interface); return m_interface->GetFacing(); }  // Returns the facing of this organism.
  int GetFacedCellID() { assert(m_interface); return m_interface->GetFacedCellID(); }  // Returns the faced cell of this organism.
  int GetFacedDir() { assert(m_interface); return m_interface->GetFacedDir(); }  // Returns the human interpretable facing of this org.
  int GetNeighborCellContents() const { return m_interface->GetNeighborCellContents(); }
  void Rotate(cAvidaContext& ctx, int direction) { m_interface->Rotate(ctx, direction); }

  int GetInputAt(int i) { return m_interface->GetInputAt(i); }
  int GetNextInput() { return m_interface->GetInputAt(m_input_pointer); }
  int GetNextInput(int& in_input_pointer) { return m_interface->GetInputAt(in_input_pointer); }
  tBuffer<int>& GetInputBuf() { return m_input_buf; }
  tBuffer<int>& GetOutputBuf() { return m_output_buf; }
  void Die(cAvidaContext& ctx) { m_interface->Die(ctx); m_is_dead = true; } 
  void KillCellID(int target, cAvidaContext& ctx) { m_interface->KillCellID(target, ctx); } 
  void UpdateMerit(double new_merit) { m_interface->UpdateMerit(new_merit); }

  int GetPrevSeenCellID() const { return m_interface->GetPrevSeenCellID(); }
  int GetPrevTaskCellID() const { return m_interface->GetPrevTaskCellID(); }
  void SetPrevSeenCellID(int id) const { m_interface->SetPrevSeenCellID(id); }
  void SetPrevTaskCellID(int id) const { m_interface->SetPrevTaskCellID(id); }
  int GetNumTaskCellsReached() const { return m_interface->GetNumTaskCellsReached(); }
  void AddReachedTaskCell() { m_interface->AddReachedTaskCell(); }

  void AddLiveOrg() { m_interface->AddLiveOrg(); } 
  void RemoveLiveOrg() { m_interface->RemoveLiveOrg(); } 
  
  // --------  Input and Output Methods  --------
  void DoInput(const int value);
  void DoInput(tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);

  /* These different flavors of DoOutput are "frontends" to the main DoOutput
  that follows - One DoOutput to rule them all, etc., etc. */
  //! Check tasks based on the current state of this organism's IO & message buffers.
  // void DoOutput(cAvidaContext& ctx, const bool on_divide=false);
  void DoOutput(cAvidaContext& ctx, const bool on_divide=false, cContextPhenotype* context_phenotype = 0); 
  //! Add the passed-in value to this organism's output buffer, and check tasks (on_divide=false).
  void DoOutput(cAvidaContext& ctx, const int value);
  //! Check if we're calling this from a parasite.
  void DoOutput(cAvidaContext& ctx, const int value, bool is_parasite, cContextPhenotype* context_phenotype);
  //! Check tasks based on the passed-in IO buffers and value (on_divide=false).
  void DoOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const int value);    

  void ClearInput() { m_input_buf.Clear(); }
  void ResetInput() {m_input_pointer = 0; m_input_buf.Clear(); };
  void AddOutput(int val) { m_output_buf.Add(val); }

  // --------  Divide Methods  --------
  bool Divide_CheckViable(cAvidaContext& ctx);
  bool ActivateDivide(cAvidaContext& ctx, cContextPhenotype* context_phenotype = 0);

  // --------  Parasite Interactions  --------
  bool InjectParasite(Systematics::UnitPtr parent, const cString& label, const InstructionSequence& genome);
  bool ParasiteInfectHost(Systematics::UnitPtr parasite);
  int GetNumParasites() const { return m_parasites.GetSize(); }
  const Apto::Array<Systematics::UnitPtr>& GetParasites() const { return m_parasites; }
  void ClearParasites();

  // --------  Mutation Rate Convenience Methods  --------
  bool TestCopyMut(cAvidaContext& ctx) const { return m_mut_rates.TestCopyMut(ctx); }
  bool TestCopyIns(cAvidaContext& ctx) const { return m_mut_rates.TestCopyIns(ctx); }
  bool TestCopyDel(cAvidaContext& ctx) const { return m_mut_rates.TestCopyDel(ctx); }
  bool TestCopyUniform(cAvidaContext& ctx) const { return m_mut_rates.TestCopyUniform(ctx); }
  bool TestCopySlip(cAvidaContext& ctx) const { return m_mut_rates.TestCopySlip(ctx); }

  bool TestDivideMut(cAvidaContext& ctx) const { return m_mut_rates.TestDivideMut(ctx); }
  bool TestDivideIns(cAvidaContext& ctx) const { return m_mut_rates.TestDivideIns(ctx); }
  bool TestDivideDel(cAvidaContext& ctx) const { return m_mut_rates.TestDivideDel(ctx); }
  bool TestDivideUniform(cAvidaContext& ctx) const { return m_mut_rates.TestDivideUniform(ctx); }
  bool TestDivideSlip(cAvidaContext& ctx) const { return m_mut_rates.TestDivideSlip(ctx); }
  bool TestDivideTrans(cAvidaContext& ctx) const { return m_mut_rates.TestDivideTrans(ctx); }
  bool TestDivideLGT(cAvidaContext& ctx) const { return m_mut_rates.TestDivideLGT(ctx); }
  
  unsigned int NumDividePoissonMut(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonMut(ctx); }
  unsigned int NumDividePoissonIns(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonIns(ctx); }
  unsigned int NumDividePoissonDel(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonDel(ctx); }
  unsigned int NumDividePoissonSlip(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonSlip(ctx); }
  unsigned int NumDividePoissonTrans(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonTrans(ctx); }
  unsigned int NumDividePoissonLGT(cAvidaContext& ctx) const { return m_mut_rates.NumDividePoissonLGT(ctx); }

  bool TestDeath(cAvidaContext& ctx) const { return m_mut_rates.TestDeath(ctx); }

  double GetCopyMutProb() const { return m_mut_rates.GetCopyMutProb(); }
  double GetCopyInsProb() const { return m_mut_rates.GetCopyInsProb(); }
  double GetCopyDelProb() const { return m_mut_rates.GetCopyDelProb(); }
  double GetCopyUniformProb() const { return m_mut_rates.GetCopyUniformProb(); }
  double GetCopySlipProb() const { return m_mut_rates.GetCopySlipProb(); }

  void SetCopyMutProb(double _p) { return m_mut_rates.SetCopyMutProb(_p); }
  void SetDivMutProb(double _p) { return m_mut_rates.SetDivMutProb(_p); }

  double GetDivInsProb() const { return m_mut_rates.GetDivInsProb(); }
  double GetDivDelProb() const { return m_mut_rates.GetDivDelProb(); }
  double GetDivMutProb() const { return m_mut_rates.GetDivMutProb(); }
  double GetDivUniformProb() const { return m_mut_rates.GetDivUniformProb(); }
  double GetDivSlipProb() const { return m_mut_rates.GetDivSlipProb(); }
  double GetDivTransProb() const { return m_mut_rates.GetDivTransProb(); }
  double GetDivLGTProb() const { return m_mut_rates.GetDivLGTProb(); }
  
  double GetPointInsProb() const { return m_mut_rates.GetPointInsProb(); }
  double GetPointDelProb() const { return m_mut_rates.GetPointDelProb(); }
  double GetPointMutProb() const { return m_mut_rates.GetPointMutProb(); }

  double GetParentMutProb() const { return m_mut_rates.GetParentMutProb();}
  double GetParentInsProb() const { return m_mut_rates.GetParentInsProb();}
  double GetParentDelProb() const { return m_mut_rates.GetParentDelProb();}

  double GetDeathProb() const { return m_mut_rates.GetDeathProb();}

  double GetInjectInsProb() const { return m_mut_rates.GetInjectInsProb(); }
  double GetInjectDelProb() const { return m_mut_rates.GetInjectDelProb(); }
  double GetInjectMutProb() const { return m_mut_rates.GetInjectMutProb(); }

  // --------  Configuration Convenience Methods  --------
  bool GetTestOnDivide() const;
  int GetSterilizeUnstable() const;

  bool GetRevertFatal() const;
  bool GetRevertNeg() const;
  bool GetRevertNeut() const;
  bool GetRevertPos() const;
  bool GetRevertTaskLoss() const;
  bool GetRevertEquals() const;

  bool GetSterilizeFatal() const;
  bool GetSterilizeNeg() const;
  bool GetSterilizeNeut() const;
  bool GetSterilizePos() const;
  bool GetSterilizeTaskLoss() const;
  double GetNeutralMin() const;
  double GetNeutralMax() const;



  // -------- Movement ---------
public:
  bool Move(cAvidaContext& ctx);




  // get the organism's relative position (from birth place)
  int GetNortherly() { return m_northerly; }
  int GetEasterly() { return m_easterly; } 
  void ClearEasterly() { m_easterly = 0; }
  void ClearNortherly() { m_northerly = 0; }
  
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
  //total number of steps taken to north (minus S steps) since birth
  int m_northerly;
  //total number of steps taken to east (minus W steps) since birth  
  int m_easterly;

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
  inline void SetAVInIndex(int index) { m_av_in_index = index; }
  inline int GetAVInIndex() { return m_av_in_index; }
  inline void SetAVOutIndex(int index) { m_av_out_index = index; }
  inline int GetAVOutIndex() { return m_av_out_index; }
    
	// -------- Internal Support Methods --------
private:
  int m_av_in_index;
  int m_av_out_index;
  
  void initialize(cAvidaContext& ctx);
  
  
  friend class OrgPropRetrievalContainer;
  template <class T> friend class OrgPropOfType;
  
  class OrgPropertyMap : public PropertyMap
  {
    friend class cOrganism;
  private:
    class OrgIntProp : public IntProperty
    {
    public:
      inline OrgIntProp(const PropertyDescriptionMap& desc_map) : IntProperty("", desc_map, 0) { ; }
      inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
    };
    
    class OrgDoubleProp : public DoubleProperty
    {
    public:
      inline OrgDoubleProp(const PropertyDescriptionMap& desc_map) : DoubleProperty("", desc_map, 0) { ; }
      inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
    };
    
    class OrgStringProp : public StringProperty
    {
    public:
      inline OrgStringProp(const PropertyDescriptionMap& desc_map) : StringProperty("", desc_map, Apto::String()) { ; }
      inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
    };
    
  private:
    cOrganism* m_organism;
    mutable OrgIntProp m_prop_int;
    mutable OrgDoubleProp m_prop_double;
    mutable OrgStringProp m_prop_string;
    
  public:
    LIB_LOCAL OrgPropertyMap(cOrganism* organism);
    LIB_LOCAL ~OrgPropertyMap();
    
    LIB_LOCAL int GetSize() const;
    
    LIB_LOCAL bool operator==(const PropertyMap& p) const;
    
    LIB_LOCAL bool Has(const PropertyID& p_id) const;
    
    LIB_LOCAL const Property& Get(const PropertyID& p_id) const;
    
    LIB_LOCAL bool SetValue(const PropertyID& p_id, const Apto::String& prop_value);
    LIB_LOCAL bool SetValue(const PropertyID& p_id, const int prop_value);
    LIB_LOCAL bool SetValue(const PropertyID& p_id, const double prop_value);
    
    
    LIB_LOCAL void Define(PropertyPtr p);
    LIB_LOCAL bool Remove(const PropertyID& p_id);
    
    LIB_LOCAL ConstPropertyIDSetPtr PropertyIDs() const;
    
    LIB_LOCAL bool Serialize(ArchivePtr ar) const;
    
    inline const Property& SetTempProp(const PropertyID& prop_id, int value) const
    {
      m_prop_int.SetPropertyID(prop_id); m_prop_int.SetValue(value); return m_prop_int;
    }
    inline const Property& SetTempProp(const PropertyID& prop_id, double value) const
    {
      m_prop_double.SetPropertyID(prop_id); m_prop_double.SetValue(value); return m_prop_double;
    }
    inline const Property& SetTempProp(const PropertyID& prop_id, const Apto::String& value) const
    {
      m_prop_string.SetPropertyID(prop_id); m_prop_string.SetValue(value); return m_prop_string;
    }
  };
  
  Apto::String getGenomeString();
  int getSrcTransmissionType();
  int getAge();
  int getGeneration();
  int getLastCopied();
  int getLastExecuted();
  int getLastGestation();
  double getLastMetabolicRate();
  double getLastFitness();
  
private:
  OrgPropertyMap m_prop_map;

  /*! The main DoOutput function.  The DoOutputs above all forward to this function. */
  void doOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const bool on_divide, bool is_parasite=false, cContextPhenotype* context_phenotype = 0);
  // Need seperate doOutput function for avatars to avoid triggering reactions by true orgs
  void doAVOutput(cAvidaContext& ctx, tBuffer<int>& input_buffer, tBuffer<int>& output_buffer, const bool on_divide, bool is_parasite=false, cContextPhenotype* context_phenotype = 0);
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

