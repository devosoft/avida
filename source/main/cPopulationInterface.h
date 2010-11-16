/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Called "pop_interface.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cPopulationInterface_h
#define cPopulationInterface_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif
#include "cGenomeUtil.h"
#include "cPopulationCell.h"

class cAvidaContext;
class cDeme;
class cSequence;
class cPopulation;
class cOrgMessage;
class cOrganism;

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
  cPopulationInterface operator=(const cPopulationInterface&); // @not_implemented
  
public:
  cPopulationInterface(cWorld* world);
  virtual ~cPopulationInterface();

	//! Retrieve this organism.
	cOrganism* GetOrganism();
	//! Retrieve the ID of this cell.
  int GetCellID() { return m_cell_id; }
	//! Retrieve the cell in which this organism lives.
	cPopulationCell* GetCell();
	//! Retrieve the cell currently faced by this organism.
	cPopulationCell* GetCellFaced();
  int GetDemeID() { return m_deme_id; }
	//! Retrieve the deme in which this organism lives.
  cDeme* GetDeme();
  void SetCellID(int in_id) { m_cell_id = in_id; }
  void SetDemeID(int in_id) { m_deme_id = in_id; }
  
  int GetCellData();
  int GetCellDataOrgID();
  int GetCellDataUpdate();
  void SetCellData(const int newData);
  int GetFacedCellData();
  int GetFacedCellDataOrgID();
  int GetFacedCellDataUpdate();

  int GetPrevSeenCellID() { return m_prevseen_cell_id; }
  int GetPrevTaskCellID() { return m_prev_task_cell; }
  int GetNumTaskCellsReached() { return m_num_task_cells; }
  void AddReachedTaskCell() { m_num_task_cells++; }
  void SetPrevSeenCellID(int in_id) { m_prevseen_cell_id = in_id; }
  void SetPrevTaskCellID(int in_id) { m_prev_task_cell = in_id; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const cGenome& offspring_genome);
  cOrganism* GetNeighbor();
  bool IsNeighborCellOccupied();
  int GetNumNeighbors();
  void GetNeighborhoodCellIDs(tArray<int>& list);
  int GetFacing(); // Returns the facing of this organism.
  int GetFacedCellID();
  int GetNeighborCellContents();
  void Rotate(int direction = 1);
  void Breakpoint() { m_world->GetDriver().SignalBreakpoint(); }
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const tArray<int>& GetInputs() const;
  const tArray<double>& GetResources();
  const tArray<double>& GetDemeResources(int deme_id);
  const tArray< tArray<int> >& GetCellIdLists();
  void UpdateResources(const tArray<double>& res_change);
  void UpdateDemeResources(const tArray<double>& res_change);
  void Die();
  void Kaboom(int distance);
  void SpawnDeme();
  cOrgSinkMessage* NetReceive();
  bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg);
  int ReceiveValue();
  void SellValue(const int data, const int label, const int sell_price, const int org_id);
  int BuyValue(const int label, const int buy_price);
  bool InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const cSequence& injected_code);
  bool UpdateMerit(double new_merit);
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
	
  void Move(cAvidaContext& ctx, int src_id, int dest_id);

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
	void DoHGTMutation(cAvidaContext& ctx, cGenome& offspring);

protected:
	//! Place the fragment at the location of best match.
	void HGTMatchPlacement(cAvidaContext& ctx, const cSequence& offspring,
												 fragment_list_type::iterator& selected,
												 substring_match& location);
	//! Place the fragment at the location of best match, with redundant instructions trimmed.
	void HGTTrimmedPlacement(cAvidaContext& ctx, const cSequence& offspring,
													 fragment_list_type::iterator& selected,
													 substring_match& location);	
	//! Place the fragment at a random location.
	void HGTRandomPlacement(cAvidaContext& ctx, const cSequence& offspring,
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
	void ReceiveHGTDonation(const cSequence& fragment);
  
  
public:
  void JoinGroup(int group_id);
  void LeaveGroup(int group_id);
  
  void BeginSleep();
  void EndSleep();
};

#endif
