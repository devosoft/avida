/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Called "pop_interface.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

class cPopulation;
class cOrgMessage;


class cPopulationInterface : public cOrgInterface
{
private:
  cWorld* m_world;
  int m_cell_id;
  int m_deme_id;

  cPopulationInterface(); // @not_implemented
  cPopulationInterface(const cPopulationInterface&); // @not_implemented
  cPopulationInterface operator=(const cPopulationInterface&); // @not_implemented
  
public:
  cPopulationInterface(cWorld* world) : m_world(world), m_cell_id(-1), m_deme_id(-1) { ; }
  virtual ~cPopulationInterface() { ; }

  int GetCellID() { return m_cell_id; }
  int GetDemeID() { return m_deme_id; }
  void SetCellID(int in_id) { m_cell_id = in_id; }
  void SetDemeID(int in_id) { m_deme_id = in_id; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome);
  cOrganism* GetNeighbor();
  int GetNumNeighbors();
  int GetFacing(); // Returns the facing of this organism.
  void Rotate(int direction = 1);
  void Breakpoint() { m_world->GetDriver().SignalBreakpoint(); }
  int GetInputAt(int& input_pointer);
  void ResetInputs(cAvidaContext& ctx);
  const tArray<int>& GetInputs() const;
  int Debug();
  const tArray<double>& GetResources();
  const tArray<double>& GetDemeResources(int deme_id);
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
