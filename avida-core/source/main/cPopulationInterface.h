/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Called "pop_interface.hh" prior to 12/5/05.
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

#ifndef cPopulationInterface_h
#define cPopulationInterface_h

#include "avida/core/WorldDriver.h"
#include "avida/systematics/Unit.h"

#include "cOrgInterface.h"
#include "cWorld.h"
#include "cGenomeUtil.h"
#include "cPopulationCell.h"

class cAvidaContext;
class cDeme;
class cPopulation;
class cOrgMessage;
class cOrganism;

using namespace Avida;


class cPopulationInterface : public cOrgInterface
{
private:
  cWorld* m_world;
  int m_cell_id;
  int m_deme_id;
  
  int m_prevseen_cell_id;	// Previously-seen cell's ID
  int m_prev_task_cell;		// Cell ID of previous task
  int m_num_task_cells;		// Number of task cells seen

  cPopulationInterface(); // @not_implemented
  cPopulationInterface(const cPopulationInterface&); // @not_implemented
  cPopulationInterface& operator=(const cPopulationInterface&); // @not_implemented
  
public:
  cPopulationInterface(cWorld* world);
  virtual ~cPopulationInterface();

  const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const;
	//! Retrieve this organism.
	cOrganism* GetOrganism();
	//! Retrieve the ID of this cell.
  int GetCellID() { return m_cell_id; }
  //! Retrieve the cell in which this organism lives.
  cPopulationCell* GetCell();
  cPopulationCell* GetCell(int cell_id);
  //! Retrieve the cell currently faced by this organism.
  cPopulationCell* GetCellFaced();
  int GetDemeID() { return m_deme_id; }
  //! Retrieve the deme in which this organism lives.
  cDeme* GetDeme();
  void SetCellID(int in_id) { m_cell_id = in_id; }
  void SetDemeID(int in_id) { m_deme_id = in_id; }
  int GetCellXPosition();
  int GetCellYPosition();
  
  int GetCellData();
  int GetCellDataOrgID();
  int GetCellDataUpdate();
  int GetCellDataTerritory();
  int GetCellDataForagerType();
  void SetCellData(const int newData);
  int GetFacedCellData();
  int GetFacedCellDataOrgID();
  int GetFacedCellDataUpdate();
  int GetFacedCellDataTerritory();

  int GetPrevSeenCellID() { return m_prevseen_cell_id; }
  int GetPrevTaskCellID() { return m_prev_task_cell; }
  int GetNumTaskCellsReached() { return m_num_task_cells; }
  void AddReachedTaskCell() { m_num_task_cells++; }
  void SetPrevSeenCellID(int in_id) { m_prevseen_cell_id = in_id; }
  void SetPrevTaskCellID(int in_id) { m_prev_task_cell = in_id; }

  bool GetLGTFragment(cAvidaContext& ctx, int region, const Genome& dest_genome, InstructionSequence& seq);

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome);
  cOrganism* GetNeighbor();
  bool IsNeighborCellOccupied();
  int GetNumNeighbors();
  void GetNeighborhoodCellIDs(Apto::Array<int>& list);
  void GetAVNeighborhoodCellIDs(Apto::Array<int>& list, int av_num = 0);
  int GetFacing(); // Returns the facing of this organism.
  int GetFacedCellID();
  int GetFacedDir(); // Returns the human interpretable facing of this org.
  int GetNeighborCellContents();
  void Rotate(cAvidaContext& ctx, int direction = 1);
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const Apto::Array<int>& GetInputs() const;
  const Apto::Array<double>& GetResources(cAvidaContext& ctx); 
  double GetResourceVal(cAvidaContext& ctx, int res_id);
  const Apto::Array<double>& GetFacedCellResources(cAvidaContext& ctx);
  double GetFacedResourceVal(cAvidaContext& ctx, int res_id);
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx); 
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id);
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  const Apto::Array<double>& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  const Apto::Array< Apto::Array<int> >& GetCellIdLists();
  int GetCurrPeakX(cAvidaContext& ctx, int res_id); 
  int GetCurrPeakY(cAvidaContext& ctx, int res_id);
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id); 
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id);
  cResourceCount* GetResourceCount();
  void TriggerDoUpdates(cAvidaContext& ctx);
  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateDemeResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void Die(cAvidaContext& ctx); 
  void KillCellID(int target, cAvidaContext& ctx); 
  void Kaboom(int distance, cAvidaContext& ctx); 
  void SpawnDeme(cAvidaContext& ctx); 
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price, const int org_id);
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code);
  bool UpdateMerit(cAvidaContext& ctx, double new_merit);
  bool TestOnDivide();
  //! Send a message to the faced organism.
  bool SendMessage(cOrgMessage& msg);
  //! Send a message to the organism in the given cell.
  bool SendMessage(cOrgMessage& msg, cPopulationCell& rcell);
  //! Send a message to the cell with the given cell id.
  bool SendMessage(cOrgMessage& msg, int cellid);	
  //! Broadcast a message.
  bool BroadcastMessage(cOrgMessage& msg, int depth);
  bool BcastAlarm(int jump_label, int bcast_range);  
  void DivideOrgTestamentAmongDeme(double value);
  //! Send a flash to all neighboring organisms.
  void SendFlash();

  int GetStateGridID(cAvidaContext& ctx);
	
  bool Move(cAvidaContext& ctx, int src_id, int dest_id);

  // Reputation
  void RotateToGreatestReputation();
  void RotateToGreatestReputationWithDifferentTag(int tag);
  void RotateToGreatestReputationWithDifferentLineage(int line);

  // -------- Network creation support --------
public:
  //! Link this organism's cell to the cell it is currently facing.
  void CreateLinkByFacing(double weight=1.0);
  //! Link this organism's cell to the cell with coordinates (x,y).
  void CreateLinkByXY(int x, int y, double weight=1.0);
  //! Link this organism's cell to the cell with index idx.
  void CreateLinkByIndex(int idx, double weight=1.0);
  //! Broadcast a message to all organisms that are connected by this network.
  bool NetworkBroadcast(cOrgMessage& msg);
  //! Unicast a message to the current selected organism.
  bool NetworkUnicast(cOrgMessage& msg);
  //! Rotate to select a new network link.
  bool NetworkRotate(int x);
  //! Select a new network link.
  bool NetworkSelect(int x);

  int GetNextDemeInput(cAvidaContext& ctx);
  void DoDemeInput(int value);
  void DoDemeOutput(cAvidaContext& ctx, int value);

  // -------- HGT support --------
public:
  //! Container type for fragments used during HGT.
  typedef cPopulationCell::fragment_list_type fragment_list_type;
  //! Match record, used to indicate the region within a genome that should be mutated.
  typedef cGenomeUtil::substring_match substring_match;
  //! Called when this organism is the donor during conjugation.
  void DoHGTDonation(cAvidaContext& ctx);
  //! Called when this organism "requests" an HGT conjugation.
  void DoHGTConjugation(cAvidaContext& ctx);
  //! Perform an HGT mutation on this offspring.
  void DoHGTMutation(cAvidaContext& ctx, Genome& offspring);

protected:
	//! Place the fragment at the location of best match.
	void HGTMatchPlacement(cAvidaContext& ctx, const InstructionSequence& offspring,
												 fragment_list_type::iterator& selected,
												 substring_match& location);
	//! Place the fragment at the location of best match, with redundant instructions trimmed.
	void HGTTrimmedPlacement(cAvidaContext& ctx, const InstructionSequence& offspring,
													 fragment_list_type::iterator& selected,
													 substring_match& location);	
	//! Place the fragment at a random location.
	void HGTRandomPlacement(cAvidaContext& ctx, const InstructionSequence& offspring,
													fragment_list_type::iterator& selected,
													substring_match& location);
  //! Support for stateful HGT mutations.
  struct HGTSupport {
		fragment_list_type _pending; //!< HGT fragments that are awaiting an offspring.
	};
	HGTSupport* m_hgt_support; //!< Lazily-initialized pointer to HGT data.
	//! Initialize HGT support.
	inline void InitHGTSupport() { if(!m_hgt_support) { m_hgt_support = new HGTSupport(); } }
	//! Called when this organism is the receiver of an HGT donation.
	void ReceiveHGTDonation(const InstructionSequence& fragment);
  
public:
  void AddLiveOrg(); 
  void RemoveLiveOrg();

  bool HasOpinion(cOrganism* in_organism);
  void SetOpinion(int opinion, cOrganism* in_organism);
  void ClearOpinion(cOrganism* in_organism);
  
  void JoinGroup(int group_id);
  void MakeGroup();
  void LeaveGroup(int group_id);
  int NumberOfOrganismsInGroup(int group_id);
  int NumberGroupFemales(int group_id);
  int NumberGroupMales(int group_id);
  int NumberGroupJuvs(int group_id);
  void ChangeGroupMatingTypes(cOrganism* org, int group_id, int old_type, int new_type); 
  
  // ----- Tolerance/Group support ------
  int IncTolerance(const int tolerance_type, cAvidaContext& ctx);
  int DecTolerance(const int tolerance_type, cAvidaContext& ctx);
  int CalcGroupToleranceImmigrants(int prop_group_id, int mating_type = -1);
  int CalcGroupToleranceOffspring(cOrganism* parent_organism);
  double CalcGroupOddsImmigrants(int group_id, int mating_type);
  double CalcGroupOddsOffspring(cOrganism* parent);
  double CalcGroupOddsOffspring(int group_id);
  bool AttemptImmigrateGroup(cAvidaContext& ctx, int group_id, cOrganism* org);
  void PushToleranceInstExe(int tol_inst, cAvidaContext& ctx);
  int& GetGroupIntolerances(int group_id, int tol_num, int mating_type);
  
  void TryWriteGroupAttackBits(unsigned char raw_bits);
  void TryWriteGroupAttackString(cString& string);
  void DecNumPreyOrganisms();
  void DecNumPredOrganisms();
  void DecNumTopPredOrganisms();
  void IncNumPreyOrganisms();
  void IncNumPredOrganisms();
  void IncNumTopPredOrganisms();
  void AttackFacedOrg(cAvidaContext& ctx, int loser);
  
  void TryWriteBirthLocData(int org_idx);
  void InjectPreyClone(cAvidaContext& ctx, int gen_id);
  void KillRandPred(cAvidaContext& ctx, cOrganism* org);
  void KillRandPrey(cAvidaContext& ctx, cOrganism* org);
  void TryWriteLookData(cString& string);
  void TryWriteLookOutput(cString& string);
  void TryWriteLookEXOutput(cString& string);
  Apto::Array<int> GetFormedGroupArray();
  
// -------- Avatar support -------- @JJB
private:
  struct sIO_avatar
  {
    int av_cell_id;
    int av_facing;
    int av_faced_cell;
    bool av_input;
    bool av_output;
    sIO_avatar() : av_cell_id(-1), av_facing(0), av_faced_cell(-1), av_input(false), av_output(false) { ; }
    sIO_avatar(int in_av_cell_id, int in_av_facing, int in_av_faced_cell, bool input, bool output) : av_cell_id(in_av_cell_id), av_facing(in_av_facing), av_faced_cell(in_av_faced_cell), av_input(input), av_output(output) { ; }
  };

  Apto::Array<sIO_avatar, Apto::Smart> m_avatars;
  inline int GetNumAV() { return m_avatars.GetSize(); }
public:
  bool HasOutputAV(int av_num = 0);
  bool FacedHasOutputAV(int av_num = 0);
  bool FacedHasAV(int av_num = 0);
  bool FacedHasPredAV(int av_num = 0);
  bool FacedHasPreyAV(int av_num = 0);
  void AddIOAV(cAvidaContext& ctx, int av_cell_id, int av_facing, bool input, bool output);
  void AddPredPreyAV(cAvidaContext& ctx, int av_cell_id);
  void SwitchPredPrey(cAvidaContext& ctx, int av_num = 0);
  void RemoveAllAV();
  int GetAVFacing(int av_num = 0);
  int GetAVCellID(int av_num = 0);
  int GetAVFacedCellID(int av_num = 0);
  int GetAVNumNeighbors(int av_num = 0);
  int GetAVFacedData(int av_num = 0);
  int GetAVFacedDataOrgID(int av_num = 0);
  int GetAVFacedDataUpdate(int av_num = 0);
  int GetAVFacedDataTerritory(int av_num = 0);
  int GetAVData(int av_num = 0);
  int GetAVDataOrgID(int av_num = 0);
  int GetAVDataUpdate(int av_num = 0);
  int GetAVDataTerritory(int av_num = 0);
  int FindAV(bool input, bool output, int av_num = 0);
  void SetAVFacing(cAvidaContext& ctx, int av_facing, int av_num = 0);
  bool SetAVCellID(cAvidaContext& ctx, int av_cell_id, int av_num = 0);
  void SetAVFacedCellID(cAvidaContext& ctx, int av_num = 0);
  void SetAVCellData(const int newData, const int org_id, int av_num = 0);
  bool MoveAV(cAvidaContext& ctx, int av_num = 0);
  bool RotateAV(cAvidaContext& ctx, int increment, int av_num = 0);
  cOrganism* GetRandFacedAV(cAvidaContext& ctx, int av_num = 0);
  cOrganism* GetRandFacedPredAV(int av_num = 0);
  cOrganism* GetRandFacedPreyAV(int av_num = 0);
  Apto::Array<cOrganism*> GetFacedAVs(int av_num = 0);
  Apto::Array<cOrganism*> GetCellAVs(int cell_id, int av_num = 0);
  Apto::Array<cOrganism*> GetFacedPreyAVs(int av_num = 0);
  const Apto::Array<double>& GetAVResources(cAvidaContext& ctx, int av_num = 0);
  double GetAVResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  const Apto::Array<double>& GetAVFacedResources(cAvidaContext& ctx, int av_num = 0);
  double GetAVFacedResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  void UpdateAVResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, int av_num = 0);
  
  void BeginSleep();
  void EndSleep();
};

#endif
