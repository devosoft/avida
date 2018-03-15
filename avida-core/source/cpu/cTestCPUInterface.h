/*
 *  cTestCPUOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cTestCPUInterface_h
#define cTestCPUInterface_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif

#include "avida/systematics/Unit.h"

class cCPUTestInfo;
class cTestCPU;

#ifndef NULL
#define NULL 0
#endif

class cTestCPUInterface : public cOrgInterface
{
private:
  cTestCPU* m_testcpu;
  cCPUTestInfo& m_test_info;
  int m_cur_depth;
  Apto::Array<cOrganism*, Apto::Smart> m_empty_live_org_list;
  
public:
  cTestCPUInterface(cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_depth)
    : m_testcpu(testcpu), m_test_info(test_info), m_cur_depth(cur_depth) { ; }
  virtual ~cTestCPUInterface() { ; }

  
  const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const;
  cPopulationCell* GetCell() { return NULL; }
	cPopulationCell* GetCell(int) { return NULL; }
  int GetCellID() { return -1; }
  int GetDemeID() { return -1; }
  cDeme* GetDeme() { return 0; }
  void SetCellID(int) { ; }
  void SetDemeID(int) { ; }
  
  int GetAVCellID() { return -1; }
  void SetAVCellID(int) { ; }
  void SetAvatarFacing(int) { ; }
  void SetAvatarFacedCell(int) { ; }
  int GetAVFacedCellID() { return -1; }

  int GetCellXPosition() { return -1; }
  int GetCellYPosition() { return -1; }
  
  int GetCellData() { return -1; }
  int GetCellDataOrgID() { return -1; }
  int GetCellDataUpdate() { return -1; }
  int GetCellDataTerritory() { return -1; }
  int GetCellDataForagerType() { return -99; }
  void SetCellData(const int) { ; }
  void SetAVCellData(const int, const int) { ; }
  int GetFacedCellData() { return -1; }
  int GetFacedCellDataOrgID() { return -1; }
  int GetFacedCellDataUpdate() { return -1; }
  int GetFacedCellDataTerritory() { return -1; }

  int GetPrevSeenCellID() { return 0; }
  int GetPrevTaskCellID() { return 0; }
  void AddReachedTaskCell() { ; }
  int GetNumTaskCellsReached() { return 0; }
  void SetPrevSeenCellID(int) { ; }
  void SetPrevTaskCellID(int) { ; }

  bool GetLGTFragment(cAvidaContext& ctx, int region, const Genome& dest_genome, InstructionSequence& seq) { return false; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome);
  cOrganism* GetNeighbor() { return NULL; }
  bool IsNeighborCellOccupied() { return false; }
  int GetNumNeighbors() { return 0; }
  void GetNeighborhoodCellIDs(Apto::Array<int>& list) { ; }
  void GetAVNeighborhoodCellIDs(Apto::Array<int>& list, int av_num = 0) { ; }
  int GetNeighborCellContents() { return 0; }
  void Rotate(cAvidaContext& ctx, int direction = 1) { ; }
  
  void Breakpoint() { ; }
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const Apto::Array<int>& GetInputs() const;
  const Apto::Array<double>& GetResources(cAvidaContext& ctx); 
  double GetResourceVal(cAvidaContext& ctx, int res_id);
  const Apto::Array<double>& GetFacedCellResources(cAvidaContext& ctx); 
  double GetFacedResourceVal(cAvidaContext& ctx, int res_id);
  const Apto::Array<double>& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx); 
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id);
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id);
  const Apto::Array< Apto::Array<int> >& GetCellIdLists();
  
  int GetCurrPeakX(cAvidaContext& ctx, int res_id) { return 0; } 
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) { return 0; } 
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) { return 0; } 
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) { return 0; } 

  cResourceCount* GetResourceCount() { return NULL; }
  void TriggerDoUpdates(cAvidaContext&) { }
  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateRandomResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateAVResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateDemeResources(cAvidaContext&, const Apto::Array<double>&) { ; }
  
  void Die(cAvidaContext& ctx) { ; } 
  void KillCellID(int target, cAvidaContext& ctx) { ; } 
  void Kaboom(int distance, cAvidaContext& ctx);
  void Kaboom(int distance, cAvidaContext& ctx, double effect);
  void SpawnDeme(cAvidaContext& ctx) { ; }
  cOrgSinkMessage* NetReceive() { return NULL; } // @DMB - todo: receive message
  bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg) { return false; } // @DMB - todo: validate message
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price, const int org_id) { ; }
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code) { return false; }
  bool UpdateMerit(cAvidaContext& ctx, double new_merit);
  bool TestOnDivide() { return false; }
  int GetFacing() { return 0; }
  int GetFacedCellID() { return -1; }
  int GetFacedDir() { return 0; }
  bool SendMessage(cOrgMessage&) { return false; }
  bool SendMessage(cOrganism*, cOrgMessage&) { return false; }
	bool BroadcastMessage(cOrgMessage&, int) { return false; }
	bool BcastAlarm(int, int) { return false; }
  void DivideOrgTestamentAmongDeme(double) { ; }
	void SendFlash() { ; }
  
  int GetNortherly() { return 0; }
  int GetEasterly() { return 0; }
	
	void RotateToGreatestReputation(){ }
	void RotateToGreatestReputationWithDifferentTag(int) { ; }
	void RotateToGreatestReputationWithDifferentLineage(int) { ; }	
  
  int GetStateGridID(cAvidaContext& ctx);
	
	//! Link this organism's cell to the cell it is currently facing.
	void CreateLinkByFacing(double) { ; }
	//! Link this organism's cell to the cell with coordinates (x,y).
	void CreateLinkByXY(int, int, double) { ; }
	//! Link this organism's cell to the cell with index idx.
	void CreateLinkByIndex(int, double) { ; }
	//! Broadcast a message to all organisms that are connected by this network.
	bool NetworkBroadcast(cOrgMessage&) { return false; }
	//! Unicast a message to the current selected organism.
	bool NetworkUnicast(cOrgMessage&) { return false; }
	//! Rotate to select a new network link.
	bool NetworkRotate(int) { return false; }
	//! Select a new network link.
	bool NetworkSelect(int) { return false; }	

  int GetNextDemeInput(cAvidaContext&) { return -1; }
  void DoDemeInput(int) { ; }
  void DoDemeOutput(cAvidaContext& ctx, int) { ; }

	//! HGT donation (does nothing).
	void DoHGTDonation(cAvidaContext&) { ; }
	//! HGT conjugation (does nothing).
	void DoHGTConjugation(cAvidaContext&) { ; }
	//! HGT mutation (does nothing).
	void DoHGTMutation(cAvidaContext&, Genome&) { ; }
	//! Receive HGT donation (does nothing).
	void ReceiveHGTDonation(const InstructionSequence&) { ; }
  
  bool Move(cAvidaContext&, int, int) { return false; }

  void AddLiveOrg() { ; }  
  void RemoveLiveOrg() { ; }  
  
  bool HasOpinion(cOrganism*) { return false; }
  void SetOpinion(int, cOrganism*) { ; }
  void ClearOpinion(cOrganism*) { ; }

  void JoinGroup(int) { ; }
  void MakeGroup() { ; }
  void LeaveGroup(int) { ; }
  int NumberOfOrganismsInGroup(int) { return 0; }
  int NumberGroupFemales(int) { return 0; }
  int NumberGroupMales(int) { return 0; }
  int NumberGroupJuvs(int) { return 0; }
  void ChangeGroupMatingTypes(cOrganism*, int, int, int) { ; }
    
  int IncTolerance(int, cAvidaContext&) { return 0; }
  int DecTolerance(int, cAvidaContext&) { return 0; }
  int CalcGroupToleranceImmigrants(int, int) { return 0; }
  int CalcGroupToleranceOffspring(cOrganism*) { return 0; }
  double CalcGroupOddsImmigrants(int, int) { return 0.0; }
  double CalcGroupOddsOffspring(cOrganism*) { return 0.0; }
  double CalcGroupOddsOffspring(int) { return 0.0; }
  bool AttemptImmigrateGroup(cAvidaContext& ctx, int, cOrganism*) { return false; }
  void PushToleranceInstExe(int, cAvidaContext&) { ; }
  int& GetGroupIntolerances(int, int, int) { return *(new int(0)); }

  void TryWriteGroupAttackBits(unsigned char) { ; }
  void TryWriteGroupAttackString(cString&) { ; }
  void DecNumPreyOrganisms() { ; }
  void DecNumPredOrganisms() { ; }
  void DecNumTopPredOrganisms() { ; }
  void IncNumPreyOrganisms() { ; }
  void IncNumPredOrganisms() { ; }
  void IncNumTopPredOrganisms() { ; }
  void AttackFacedOrg(cAvidaContext& ctx, int) { ; }
  void TryWriteBirthLocData(int) { ; }
  void InjectPreyClone(cAvidaContext& ctx, int gen_id) { ; }
  void KillRandPred(cAvidaContext& ctx, cOrganism* org) { ; }
  void KillRandPrey(cAvidaContext& ctx, cOrganism* org) { ; }
  void TryWriteLookData(cString& string) { ; }
  void TryWriteLookOutput(cString& string) { ; }
  void TryWriteLookEXOutput(cString& string) { ; }
  Apto::Array<int> GetFormedGroupArray();

  bool HasOutputAV(int av_num = 0) { return false; }
  bool FacedHasOutputAV(int av_num = 0) { return false; }
  bool FacedHasAV(int av_num = 0) { return false; }
  bool FacedHasPredAV(int av_num = 0) { return false; }
  bool FacedHasPreyAV(int av_num = 0) { return false; }
  void AddIOAV(cAvidaContext& ctx, int in_av_cell_id, int in_av_facing, bool input, bool output) { ; }
  void AddPredPreyAV(cAvidaContext& ctx, int av_cell_id) { ; }
  void SwitchPredPrey(cAvidaContext& ctx, int av_num = 0) { ; }
  void RemoveAllAV() { ; }
  int GetAVFacing(int av_num = 0) { return 0; }
  int GetAVCellID(int av_num = 0) { return -1; }
  int GetAVFacedCellID(int av_num = 0) { return -1; }
  int GetAVNumNeighbors(int av_num = 0) { return 0; }
  int GetAVFacedData(int av_num = 0) { return -1; }
  int GetAVFacedDataOrgID(int av_num = 0) { return -1; }
  int GetAVFacedDataUpdate(int av_num = 0) { return -1; }
  int GetAVFacedDataTerritory(int av_num = 0) { return -1; }
  int GetAVData(int av_num = 0) { return -1; }
  int GetAVDataOrgID(int av_num = 0) { return -1; }
  int GetAVDataUpdate(int av_num = 0) { return -1; }
  int GetAVDataTerritory(int av_num = 0) { return -1; }
  int FindAV(bool input, bool output, int av_num = 0) { return -1; }
  void SetAVFacing(cAvidaContext& ctx, int av_facing, int av_num = 0) { ; }
  bool SetAVCellID(cAvidaContext& ctx, int av_cell_id, int av_num = 0) { return false; }
  void SetAVFacedCellID(cAvidaContext& ctx, int av_num = 0) { ; }
  void SetAVCellData(const int newData, const int org_id, int av_num = 0) { ; }
  bool MoveAV(cAvidaContext& ctx, int av_num = 0) { return false; }
  bool RotateAV(cAvidaContext& ctx, int increment, int av_num = 0) { return false; }
  cOrganism* GetRandFacedAV(cAvidaContext& ctx, int av_num = 0) { return NULL; }
  cOrganism* GetRandFacedPredAV(int av_num = 0) { return NULL; }
  cOrganism* GetRandFacedPreyAV(int av_num = 0) { return NULL; }
  Apto::Array<cOrganism*> GetFacedAVs(int av_num = 0);
  Apto::Array<cOrganism*> GetCellAVs(int cell_id, int av_num = 0);
  Apto::Array<cOrganism*> GetFacedPreyAVs(int av_num = 0);
  const Apto::Array<double>& GetAVResources(cAvidaContext& ctx, int av_num = 0);
  double GetAVResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  const Apto::Array<double>& GetAVFacedResources(cAvidaContext& ctx, int av_num = 0);
  double GetAVFacedResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0);
  void UpdateAVResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, int av_num = 0);
  
  void BeginSleep() { ; }
  void EndSleep() { ; }
};

#endif
