/*
 *  cTestCPUOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cTestCPUInterface_h
#define cTestCPUInterface_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif

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

public:
  cTestCPUInterface(cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_depth)
    : m_testcpu(testcpu), m_test_info(test_info), m_cur_depth(cur_depth) { ; }
  virtual ~cTestCPUInterface() { ; }

  int GetCellID() { return -1; }
  int GetDemeID() { return -1; }
  cDeme* GetDeme() { return 0; }
  void SetCellID(int in_id) { ; }
  void SetDemeID(int in_id) { ; }
  
  int GetCellData() { return -1; }
  void SetCellData(const int newData) { ; }

  int GetPrevSeenCellID() { return 0; }
  int GetPrevTaskCellID() { return 0; }
  void AddReachedTaskCell() { }
  int GetNumTaskCellsReached() { return 0; }
  void SetPrevSeenCellID(int in_id) { ; }
  void SetPrevTaskCellID(int in_id) { ; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const cMetaGenome& offspring_genome);
  cOrganism* GetNeighbor();
  bool IsNeighborCellOccupied();
  int GetNumNeighbors();
  void GetNeighborhoodCellIDs(tArray<int>& list);
  int GetNeighborCellContents() { return 0; }
  void Rotate(int direction = 1);
  void Breakpoint() { ; }
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const tArray<int>& GetInputs() const;
  const tArray<double>& GetResources();
  const tArray<double>& GetDemeResources(int deme_id);
  const tArray< tArray<int> >& GetCellIdLists();  
  void UpdateResources(const tArray<double>& res_change);
  void UpdateDemeResources(const tArray<double>& res_change) {;}
  void Die();
  void Kaboom(int distance);
  void SpawnDeme();
  cOrgSinkMessage* NetReceive() { return NULL; } // @DMB - todo: receive message
  bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg) { return false; } // @DMB - todo: validate message
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price, const int org_id);
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const cGenome& injected_code);
  bool UpdateMerit(double new_merit);
  bool TestOnDivide() { return false; }
  int GetFacing() { return 0; }
  bool SendMessage(cOrgMessage& msg) { return false; }
  bool SendMessage(cOrganism* recvr, cOrgMessage& msg) { return false; }
	bool BroadcastMessage(cOrgMessage& msg, int depth) { return false; }
	bool BcastAlarm(int jump_label, int bcast_range) { return false; }
  void DivideOrgTestamentAmongDeme(double value) {;}
	void SendFlash() { }
	
	void RotateToGreatestReputation(){ }
	void RotateToGreatestReputationWithDifferentTag(int tag){ }
	void RotateToGreatestReputationWithDifferentLineage(int tag){ }	
  
  int GetStateGridID(cAvidaContext& ctx);
	
	//! Link this organism's cell to the cell it is currently facing.
	void CreateLinkByFacing(double weight=1.0) { }
	//! Link this organism's cell to the cell with coordinates (x,y).
	void CreateLinkByXY(int x, int y, double weight=1.0) { }
	//! Link this organism's cell to the cell with index idx.
	void CreateLinkByIndex(int idx, double weight=1.0) { }
	
	//! HGT donation (does nothing).
	void DoHGTDonation(cAvidaContext& ctx) { }
	//! HGT conjugation (does nothing).
	void DoHGTConjugation(cAvidaContext& ctx) { }
	//! HGT mutation (does nothing).
	void DoHGTMutation(cAvidaContext& ctx, cGenome& offspring) { }
	//! Receive HGT donation (does nothing).
	void ReceiveHGTDonation(const cGenome& fragment) { }
};

#endif
