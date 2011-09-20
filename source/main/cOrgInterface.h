/*
 *  cOrgInterface.h
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

// This class is responsible for establishing the interface that links an
// organism back to its population.  It is a base class that is derived in:
//
//   cpu/cTestCPUInterface.{h,cc}      - Test CPU interface
//   main/cPopulationInterface.{h,cc}  - Main population interface.
//
// Make sure that any new function you implment here also has versions in
// those classes.

#ifndef cOrgInterface_h
#define cOrgInterface_h

namespace Avida {
  class Genome;
  class Sequence;
};

class cAvidaContext;
class cBioUnit;
class cDeme;
class cOrganism;
class cOrgMessage;
class cOrgSinkMessage;
class cString;
template <class T> class tArray;

using namespace Avida;


class cOrgInterface
{
private:
  cOrgInterface(const cOrgInterface&); // @not_implemented
  cOrgInterface& operator=(const cOrgInterface&); // @not_implemented
  
public:
  cOrgInterface() { ; }
  virtual ~cOrgInterface() { ; }

  virtual int GetCellID() = 0;
  virtual int GetDemeID() = 0;
  virtual cDeme* GetDeme() = 0;
  virtual void SetCellID(int in_id) = 0;
  virtual void SetDemeID(int in_id) = 0;
  
  virtual int GetCellData() = 0;
  virtual int GetCellDataOrgID() = 0;
  virtual int GetCellDataUpdate() = 0;
  virtual int GetCellDataTerritory() = 0;
  virtual void SetCellData(const int newData) = 0;
  virtual int GetFacedCellData() = 0;
  virtual int GetFacedCellDataOrgID() = 0;
  virtual int GetFacedCellDataUpdate() = 0;
  virtual int GetFacedCellDataTerritory() = 0;
  
  virtual int GetPrevSeenCellID() = 0;
  virtual int GetPrevTaskCellID() = 0;
  virtual int GetNumTaskCellsReached() = 0;
  virtual void AddReachedTaskCell() = 0;
  virtual void SetPrevSeenCellID(int in_id) = 0;
  virtual void SetPrevTaskCellID(int in_id) = 0;

  virtual bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome) = 0;
  
  virtual cOrganism* GetNeighbor() = 0;
  virtual bool IsNeighborCellOccupied() = 0;
  virtual int GetNumNeighbors() = 0;
  virtual void GetNeighborhoodCellIDs(tArray<int>& list) = 0;
  virtual int GetFacing() = 0; //!< Returns the facing of this organism.
  virtual int GetFacedCellID() = 0;
  virtual int GetFacedDir() = 0; // Returns the human interpretable facing of this org.
  virtual int GetNeighborCellContents() = 0;
  virtual void Rotate(int direction = 1) = 0;
  
  virtual void Breakpoint() = 0;
  
  virtual int GetInputAt(int& input_pointer) = 0;
  virtual void ResetInputs(cAvidaContext& ctx) = 0;
  virtual const tArray<int>& GetInputs() const = 0;
  virtual const tArray<double>& GetResources(cAvidaContext& ctx) = 0; 
  virtual const tArray<double>& GetFacedCellResources(cAvidaContext& ctx) = 0; 
  virtual const tArray<double>& GetDemeResources(int deme_id, cAvidaContext& ctx) = 0; 
  virtual const tArray<double>& GetCellResources(int cell_id, cAvidaContext& ctx) = 0; 
  virtual const tArray< tArray<int> >& GetCellIdLists() = 0; 
  virtual void UpdateResources(cAvidaContext& ctx, const tArray<double>& res_change) = 0;
  virtual void UpdateDemeResources(cAvidaContext& ctx, const tArray<double>& res_change) = 0;
  virtual void Die(cAvidaContext& ctx) = 0; 
  virtual void KillCellID(int target, cAvidaContext& ctx) = 0; 
  virtual void Kaboom(int distance, cAvidaContext& ctx) = 0; 
  virtual void SpawnDeme(cAvidaContext& ctx) = 0; 
  virtual cOrgSinkMessage* NetReceive() = 0;
  virtual bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg) = 0;
  virtual int ReceiveValue() = 0;
  virtual void SellValue(const int data, const int label, const int sell_price, const int org_id) = 0;
  virtual int BuyValue(const int label, const int buy_price) = 0;
  virtual bool InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const Sequence& injected_code) = 0;
  virtual bool UpdateMerit(double new_merit) = 0;
  virtual bool TestOnDivide() = 0;
  virtual bool SendMessage(cOrgMessage& msg) = 0;
	virtual bool BroadcastMessage(cOrgMessage& msg, int depth) = 0;
  virtual bool BcastAlarm(int jump_jabel, int bcast_range) = 0;
  virtual void DivideOrgTestamentAmongDeme(double value) = 0;
	virtual void SendFlash() = 0;
  
  virtual int GetStateGridID(cAvidaContext& ctx) = 0;
	virtual void RotateToGreatestReputation() =0;
	virtual void RotateToGreatestReputationWithDifferentTag(int tag) =0;
	virtual void RotateToGreatestReputationWithDifferentLineage(int line) =0;	

	virtual void CreateLinkByFacing(double weight=1.0) = 0;
	virtual void CreateLinkByXY(int x, int y, double weight=1.0) = 0;
	virtual void CreateLinkByIndex(int idx, double weight=1.0) = 0;
	virtual bool NetworkBroadcast(cOrgMessage& msg) = 0;
	virtual bool NetworkUnicast(cOrgMessage& msg) = 0;
	virtual bool NetworkRotate(int x) = 0;
	virtual bool NetworkSelect(int x) = 0;
	
	virtual void DoHGTDonation(cAvidaContext& ctx) = 0;
	virtual void DoHGTConjugation(cAvidaContext& ctx) = 0;
	virtual void DoHGTMutation(cAvidaContext& ctx, Genome& offspring) = 0;
	virtual void ReceiveHGTDonation(const Sequence& fragment) = 0;
  
  virtual bool Move(cAvidaContext& ctx, int src_id, int dest_id) = 0;

  virtual void AddLiveOrg() = 0;
  virtual void RemoveLiveOrg() = 0;
  
  virtual bool HasOpinion(cOrganism* in_organism) = 0;
  virtual void SetOpinion(int opinion, cOrganism* in_organism) = 0;
  virtual void ClearOpinion(cOrganism* in_organism) = 0;

  virtual void JoinGroup(int group_id) = 0;
  virtual void LeaveGroup(int group_id) = 0;
  virtual int NumberOfOrganismsInGroup(int group_id) = 0;
    
  virtual int CalcGroupToleranceImmigrants(int target_group_id) = 0;
  virtual int CalcGroupToleranceOffspring(cOrganism* parent_organism) = 0;
  virtual double CalcGroupOddsImmigrants(int group_id) = 0;
  virtual double CalcGroupOddsOffspring(cOrganism* parent) = 0;
  virtual double CalcGroupOddsOffspring(int group_id) = 0;
  virtual bool AttemptImmigrateGroup(int group_id, cOrganism* org) = 0;
  virtual void PushToleranceInstExe(int tol_inst, int group_id, int group_size, double resource_level, double odds_immi,
                  double odds_own, double odds_others, int tol_immi, int tol_own, int tol_others, int tol_max) = 0;
    
  virtual void BeginSleep() = 0;
  virtual void EndSleep() = 0;
};

#endif
