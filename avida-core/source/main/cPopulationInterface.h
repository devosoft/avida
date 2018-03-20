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
  
  const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const override;
  //! Retrieve this organism.
  cOrganism* GetOrganism();
  //! Retrieve the ID of this cell.
  int GetCellID()  override { return m_cell_id; }
  //! Retrieve the cell in which this organism lives.
  cPopulationCell* GetCell() override;
  cPopulationCell* GetCell(int cell_id) override;
  //! Retrieve the cell currently faced by this organism.
  cPopulationCell* GetCellFaced();
  int GetDemeID()  override { return m_deme_id; }
  //! Retrieve the deme in which this organism lives.
  cDeme* GetDeme()  override;
  void SetCellID(int in_id)  override { m_cell_id = in_id; }
  void SetDemeID(int in_id)  override { m_deme_id = in_id; }
  int GetCellXPosition() override;
  int GetCellYPosition() override;
  
  int GetCellData() override;
  int GetCellDataOrgID() override;
  int GetCellDataUpdate() override;
  int GetCellDataTerritory() override;
  int GetCellDataForagerType() override;
  void SetCellData(const int newData) override;
  int GetFacedCellData() override;
  int GetFacedCellDataOrgID() override;
  int GetFacedCellDataUpdate() override;
  int GetFacedCellDataTerritory() override;
  
  int GetPrevSeenCellID()  override { return m_prevseen_cell_id; }
  int GetPrevTaskCellID()  override { return m_prev_task_cell; }
  int GetNumTaskCellsReached()  override { return m_num_task_cells; }
  void AddReachedTaskCell() override  { m_num_task_cells++; }
  void SetPrevSeenCellID(int in_id) override { m_prevseen_cell_id = in_id; }
  void SetPrevTaskCellID(int in_id) override { m_prev_task_cell = in_id; }
  
  bool GetLGTFragment(cAvidaContext& ctx, int region, const Genome& dest_genome, InstructionSequence& seq) override;
  
  bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome) override;
  cOrganism* GetNeighbor() override;
  bool IsNeighborCellOccupied() override;
  int GetNumNeighbors() override;
  void GetNeighborhoodCellIDs(Apto::Array<int>& list) override;
  void GetAVNeighborhoodCellIDs(Apto::Array<int>& list, int av_num = 0) override;
  int GetFacing() override; // Returns the facing of this organism.
  int GetFacedCellID() override;
  int GetFacedDir() override; // Returns the human interpretable facing of this org.
  int GetNeighborCellContents() override;
  void Rotate(cAvidaContext& ctx, int direction = 1) override;
  int GetInputAt(int& input_pointer) override;
  void ResetInputs(cAvidaContext& ctx) override;
  const Apto::Array<int>& GetInputs() const override;
  //const CellResAmounts& GetResources(cAvidaContext& ctx); 
  //ResVal GetResourceVal(cAvidaContext& ctx, int res_id);
  //const CellResAmounts& GetFacedCellResources(cAvidaContext& ctx);
  //ResVal GetFacedResourceVal(cAvidaContext& ctx, int res_id);
  //const CellResAmounts& GetCellResources(int cell_id, cAvidaContext& ctx); 
  //const CellResAmounts& GetFrozenResources(cAvidaContext& ctx, int cell_id);
  //ResAmount GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  //ResAmount GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  //const DemeResAmounts& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  
  //int GetCurrPeakX(cAvidaContext& ctx, int res_id); 
  //int GetCurrPeakY(cAvidaContext& ctx, int res_id);
  //int GetFrozenPeakX(cAvidaContext& ctx, int res_id); 
  //int GetFrozenPeakY(cAvidaContext& ctx, int res_id);
  //cResourceCount* GetResourceCount();
  //void TriggerDoUpdates(cAvidaContext& ctx);
  //void UpdateResources(cAvidaContext& ctx, const ResAmount& res_change);
  //void UpdateRandomResources(cAvidaContext& ctx, const ResAmount& res_change);
  //void UpdateDemeResources(cAvidaContext& ctx, const AResAmount& res_change);
  
  const Apto::Array< Apto::Array<int> >& GetCellIdLists();
  void Die(cAvidaContext& ctx) override; 
  void KillCellID(int target, cAvidaContext& ctx) override; 
  void Kaboom(int distance, cAvidaContext& ctx) override;
  void Kaboom(int distance, cAvidaContext& ctx, double effect) override;
  void SpawnDeme(cAvidaContext& ctx) override; 
  int ReceiveValue() override;
  void SellValue(const int data, const int label, const int sell_price, const int org_id);
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code) override;
  bool UpdateMerit(cAvidaContext& ctx, double new_merit) override;
  bool TestOnDivide() override;
  //! Send a message to the faced organism.
  bool SendMessage(cOrgMessage& msg) override;
  //! Send a message to the organism in the given cell.
  bool SendMessage(cOrgMessage& msg, cPopulationCell& rcell);
  //! Send a message to the cell with the given cell id.
  bool SendMessage(cOrgMessage& msg, int cellid);	
  //! Broadcast a message.
  bool BroadcastMessage(cOrgMessage& msg, int depth) override;
  bool BcastAlarm(int jump_label, int bcast_range) override;  
  void DivideOrgTestamentAmongDeme(double value) override;
  //! Send a flash to all neighboring organisms.
  void SendFlash() override;
  
  int GetStateGridID(cAvidaContext& ctx) override;
  
  bool Move(cAvidaContext& ctx, int src_id, int dest_id) override;
  
  // Reputation
  void RotateToGreatestReputation() override;
  void RotateToGreatestReputationWithDifferentTag(int tag) override;
  void RotateToGreatestReputationWithDifferentLineage(int line) override;
  
  // -------- Network creation support --------
public:
  //! Link this organism's cell to the cell it is currently facing.
  void CreateLinkByFacing(double weight=1.0) override;
  //! Link this organism's cell to the cell with coordinates (x,y).
  void CreateLinkByXY(int x, int y, double weight=1.0) override;
  //! Link this organism's cell to the cell with index idx.
  void CreateLinkByIndex(int idx, double weight=1.0) override;
  //! Broadcast a message to all organisms that are connected by this network.
  bool NetworkBroadcast(cOrgMessage& msg) override;
  //! Unicast a message to the current selected organism.
  bool NetworkUnicast(cOrgMessage& msg) override;
  //! Rotate to select a new network link.
  bool NetworkRotate(int x) override;
  //! Select a new network link.
  bool NetworkSelect(int x) override;
  
  int GetNextDemeInput(cAvidaContext& ctx) override;
  void DoDemeInput(int value) override;
  void DoDemeOutput(cAvidaContext& ctx, int value) override;
  
  // -------- HGT support --------
public:
  //! Container type for fragments used during HGT.
  typedef cPopulationCell::fragment_list_type fragment_list_type;
  //! Match record, used to indicate the region within a genome that should be mutated.
  typedef cGenomeUtil::substring_match substring_match;
  //! Called when this organism is the donor during conjugation.
  void DoHGTDonation(cAvidaContext& ctx) override;
  //! Called when this organism "requests" an HGT conjugation.
  void DoHGTConjugation(cAvidaContext& ctx) override;
  //! Perform an HGT mutation on this offspring.
  void DoHGTMutation(cAvidaContext& ctx, Genome& offspring) override;
  
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
  void ReceiveHGTDonation(const InstructionSequence& fragment) override;
  
public:
  void AddLiveOrg() override; 
  void RemoveLiveOrg() override;
  
  bool HasOpinion(cOrganism* in_organism) override;
  void SetOpinion(int opinion, cOrganism* in_organism) override;
  void ClearOpinion(cOrganism* in_organism) override;
  
  void JoinGroup(int group_id) override;
  void MakeGroup() override;
  void LeaveGroup(int group_id) override;
  int NumberOfOrganismsInGroup(int group_id) override;
  int NumberGroupFemales(int group_id) override;
  int NumberGroupMales(int group_id) override;
  int NumberGroupJuvs(int group_id) override;
  void ChangeGroupMatingTypes(cOrganism* org, int group_id, int old_type, int new_type) override; 
  
  // ----- Tolerance/Group support ------
  int IncTolerance(const int tolerance_type, cAvidaContext& ctx) override;
  int DecTolerance(const int tolerance_type, cAvidaContext& ctx) override;
  int CalcGroupToleranceImmigrants(int prop_group_id, int mating_type = -1) override;
  int CalcGroupToleranceOffspring(cOrganism* parent_organism) override;
  double CalcGroupOddsImmigrants(int group_id, int mating_type) override;
  double CalcGroupOddsOffspring(cOrganism* parent) override;
  double CalcGroupOddsOffspring(int group_id) override;
  bool AttemptImmigrateGroup(cAvidaContext& ctx, int group_id, cOrganism* org) override;
  void PushToleranceInstExe(int tol_inst, cAvidaContext& ctx) override;
  int& GetGroupIntolerances(int group_id, int tol_num, int mating_type) override;
  
  void TryWriteGroupAttackBits(unsigned char raw_bits) override;
  void TryWriteGroupAttackString(cString& string) override;
  void DecNumPreyOrganisms() override;
  void DecNumPredOrganisms() override;
  void DecNumTopPredOrganisms() override;
  void IncNumPreyOrganisms() override;
  void IncNumPredOrganisms() override;
  void IncNumTopPredOrganisms() override;
  void AttackFacedOrg(cAvidaContext& ctx, int loser) override;
  
  void TryWriteBirthLocData(int org_idx) override;
  void InjectPreyClone(cAvidaContext& ctx, int gen_id) override;
  void KillRandPred(cAvidaContext& ctx, cOrganism* org) override;
  void KillRandPrey(cAvidaContext& ctx, cOrganism* org) override;
  void TryWriteLookData(cString& string) override;
  void TryWriteLookOutput(cString& string) override;
  void TryWriteLookEXOutput(cString& string) override;
  Apto::Array<int> GetFormedGroupArray() override;
  
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
  bool HasOutputAV(int av_num = 0) override;
  bool FacedHasOutputAV(int av_num = 0) override;
  bool FacedHasAV(int av_num = 0) override;
  bool FacedHasPredAV(int av_num = 0) override;
  bool FacedHasPreyAV(int av_num = 0) override;
  void AddIOAV(cAvidaContext& ctx, int av_cell_id, int av_facing, bool input, bool output) override;
  void AddPredPreyAV(cAvidaContext& ctx, int av_cell_id) override;
  void SwitchPredPrey(cAvidaContext& ctx, int av_num = 0) override;
  void RemoveAllAV() override;
  int GetAVFacing(int av_num = 0) override;
  int GetAVCellID(int av_num = 0) override;
  int GetAVFacedCellID(int av_num = 0) override;
  int GetAVNumNeighbors(int av_num = 0) override;
  int GetAVFacedData(int av_num = 0) override;
  int GetAVFacedDataOrgID(int av_num = 0) override;
  int GetAVFacedDataUpdate(int av_num = 0) override;
  int GetAVFacedDataTerritory(int av_num = 0) override;
  int GetAVData(int av_num = 0) override;
  int GetAVDataOrgID(int av_num = 0) override;
  int GetAVDataUpdate(int av_num = 0) override;
  int GetAVDataTerritory(int av_num = 0) override;
  int FindAV(bool input, bool output, int av_num = 0) override;
  void SetAVFacing(cAvidaContext& ctx, int av_facing, int av_num = 0)  override;
  bool SetAVCellID(cAvidaContext& ctx, int av_cell_id, int av_num = 0) override;
  void SetAVFacedCellID(cAvidaContext& ctx, int av_num = 0) override;
  void SetAVCellData(const int newData, const int org_id, int av_num = 0) override;
  bool MoveAV(cAvidaContext& ctx, int av_num = 0) override;
  bool RotateAV(cAvidaContext& ctx, int increment, int av_num = 0) override;
  cOrganism* GetRandFacedAV(cAvidaContext& ctx, int av_num = 0) override;
  cOrganism* GetRandFacedPredAV(int av_num = 0) override;
  Apto::Array<cOrganism*> GetFacedAVs(int av_num = 0) override;
  Apto::Array<cOrganism*> GetCellAVs(int cell_id, int av_num = 0) override;
  Apto::Array<cOrganism*> GetFacedPreyAVs(int av_num = 0) override;
  
  //const ResAmount& GetAVResources(cAvidaContext& ctx, int av_num = 0);
  //ResAmount GetAVResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  //const ResAmount& GetAVFacedResources(cAvidaContext& ctx, int av_num = 0);
  //ResAmount GetAVFacedResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  //void UpdateAVResources(cAvidaContext& ctx, const ResAmount& res_change, int av_num = 0);
  
  void BeginSleep() override;
  void EndSleep() override;
  
};

#endif
