/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Called "pop_interface.hh" prior to 12/5/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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
#include "cDeme.h"

class cPopulation;
class cOrgMessage;


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
  cPopulationInterface(cWorld* world) : m_world(world), m_cell_id(-1), m_deme_id(-1) { ; }
  virtual ~cPopulationInterface() { ; }

  int GetCellID() { return m_cell_id; }
  int GetDemeID() { return m_deme_id; }
  cDeme* GetDeme();
  void SetCellID(int in_id) { m_cell_id = in_id; }
  void SetDemeID(int in_id) { m_deme_id = in_id; }
  
  int GetCellData();
  void SetCellData(const int newData);

  int GetPrevSeenCellID() { return m_prevseen_cell_id; }
  int GetPrevTaskCellID() { return m_prev_task_cell; }
  int GetNumTaskCellsReached() { return m_num_task_cells; }
  void AddReachedTaskCell() { m_num_task_cells++; }
  void SetPrevSeenCellID(int in_id) { m_prevseen_cell_id = in_id; }
  void SetPrevTaskCellID(int in_id) { m_prev_task_cell = in_id; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, const cMetaGenome& offspring_genome);
  cOrganism* GetNeighbor();
  bool IsNeighborCellOccupied();
  int GetNumNeighbors();
  void GetNeighborhoodCellIDs(tArray<int>& list);
  int GetFacing(); // Returns the facing of this organism.
  int GetNeighborCellContents();
  void Rotate(int direction = 1);
  void Breakpoint() { m_world->GetDriver().SignalBreakpoint(); }
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const tArray<int>& GetInputs() const;
  int Debug();
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
  bool InjectParasite(cOrganism* parent, const cCodeLabel& label, const cGenome& injected_code);
  bool UpdateMerit(double new_merit);
  bool TestOnDivide();
  //! Send a message to the faced organism.
  bool SendMessage(cOrgMessage& msg);
  bool SendMessage(cOrganism* recvr, cOrgMessage& msg);
  bool BroadcastMessage(cOrgMessage& msg, int depth);
  bool BcastAlarm(int jump_label, int bcast_range);  
  void DivideOrgTestamentAmongDeme(double value);
	//! Send a flash to all neighboring organisms.
  void SendFlash();

  int GetStateGridID(cAvidaContext& ctx);
};


#ifdef ENABLE_UNIT_TESTS
namespace nPopulationInterface {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
