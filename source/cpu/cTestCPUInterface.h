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
  int GetAVFacedDir() { return 0; }
  
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
  int GetFacedAVData() { return -1; }
  int GetFacedAVDataOrgID() { return -1; }
  int GetFacedAVDataUpdate() { return -1; }
  int GetFacedAVDataTerritory() { return -1; }

  int GetPrevSeenCellID() { return 0; }
  int GetPrevTaskCellID() { return 0; }
  void AddReachedTaskCell() { }
  int GetNumTaskCellsReached() { return 0; }
  void SetPrevSeenCellID(int) { ; }
  void SetPrevTaskCellID(int) { ; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome);
  cOrganism* GetNeighbor();
  cOrganism* GetAVRandNeighbor();
  cOrganism* GetAVRandNeighborPrey();
  cOrganism* GetAVRandNeighborPred();
  tArray<cOrganism*> GetAVNeighbors();
  tArray<cOrganism*> GetAVNeighborPrey();
  bool IsNeighborCellOccupied();
  bool HasAVNeighbor();
  bool HasAVNeighborPrey();
  bool HasAVNeighborPred();
  int GetNumNeighbors();
  int GetAVNumNeighbors();
  void GetNeighborhoodCellIDs(tArray<int>& list);
  int GetNeighborCellContents() { return 0; }
  void Rotate(int direction = 1);
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const tArray<int>& GetInputs() const;
  const tArray<double>& GetResources(cAvidaContext& ctx); 
  const tArray<double>& GetAVResources(cAvidaContext& ctx); 
  const tArray<double>& GetFacedCellResources(cAvidaContext& ctx); 
  const tArray<double>& GetFacedAVResources(cAvidaContext& ctx); 
  const tArray<double>& GetDemeResources(int deme_id, cAvidaContext& ctx); 
  const tArray<double>& GetCellResources(int cell_id, cAvidaContext& ctx); 
  const tArray<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id);
  const tArray< tArray<int> >& GetCellIdLists();  
  int GetCurrPeakX(cAvidaContext&, int) { return 0; } 
  int GetCurrPeakY(cAvidaContext&, int) { return 0; } 
  int GetFrozenPeakX(cAvidaContext&, int) { return 0; } 
  int GetFrozenPeakY(cAvidaContext&, int) { return 0; } 
  void TriggerDoUpdates(cAvidaContext&) { }
  void UpdateResources(cAvidaContext& ctx, const tArray<double>& res_change);
  void UpdateAVResources(cAvidaContext& ctx, const tArray<double>& res_change);
  void UpdateDemeResources(cAvidaContext&, const tArray<double>&) { ; }
  void Die(cAvidaContext& ctx); 
  void KillCellID(int target, cAvidaContext& ctx); 
  void Kaboom(int distance, cAvidaContext& ctx); 
  void SpawnDeme(cAvidaContext& ctx); 
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price, const int org_id);
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code);
  bool UpdateMerit(double new_merit);
  bool TestOnDivide() { return false; }
  int GetFacing() { return 0; }
  int GetFacedCellID() { return -1; }
  int GetFacedDir() { return 0; }
  bool SendMessage(cOrgMessage&) { return false; }
  bool SendMessage(cOrganism*, cOrgMessage&) { return false; }
	bool BroadcastMessage(cOrgMessage&, int) { return false; }
	bool BcastAlarm(int, int) { return false; }
  void DivideOrgTestamentAmongDeme(double) { ; }
	void SendFlash() { }
  
  int GetNortherly() {return 0; }
  int GetEasterly() {return 0; }
	
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

	//! HGT donation (does nothing).
	void DoHGTDonation(cAvidaContext&) { ; }
	//! HGT conjugation (does nothing).
	void DoHGTConjugation(cAvidaContext&) { ; }
	//! HGT mutation (does nothing).
	void DoHGTMutation(cAvidaContext&, Genome&) { ; }
	//! Receive HGT donation (does nothing).
	void ReceiveHGTDonation(const InstructionSequence&) { ; }
  
  bool Move(cAvidaContext&, int, int) { return false; }
  bool MoveAvatar(cAvidaContext&, int, int, int) { return false; }

  void AddLiveOrg() { ; }  
  void RemoveLiveOrg() { ; }  
  
  bool HasOpinion(cOrganism*) { return false; }
  void SetOpinion(int, cOrganism*) { ; }
  void ClearOpinion(cOrganism*) { ; }

  void JoinGroup(int) { ; }
  void MakeGroup() { ; }
  void LeaveGroup(int) { ; }
  int NumberOfOrganismsInGroup(int) { return 0; }
    
  int IncTolerance(int, cAvidaContext&) { return 0; }
  int DecTolerance(int, cAvidaContext&) { return 0; }
  int CalcGroupToleranceImmigrants(int) { return 0; }
  int CalcGroupToleranceOffspring(cOrganism*) { return 0; }
  double CalcGroupOddsImmigrants(int) { return 0.0; }
  double CalcGroupOddsOffspring(cOrganism*) { return 0.0; }
  double CalcGroupOddsOffspring(int) { return 0.0; }
  bool AttemptImmigrateGroup(int, cOrganism*) { return false; }
  void PushToleranceInstExe(int, cAvidaContext&) { ; }
  int& GetGroupIntolerances(int, int) { return *(new int(0)); }

  void AttackFacedOrg(cAvidaContext&, int) { ; }
  
  void BeginSleep() { ; }
  void EndSleep() { ; }
};

#endif
