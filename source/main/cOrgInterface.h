/*
 *  cOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

class cAvidaContext;
class cCodeLabel;
class cDeme;
class cGenome;
class cOrganism;
class cOrgMessage;
class cOrgSinkMessage;
template <class T> class tArray;


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
  virtual void SetCellData(const int newData) = 0;

  virtual int GetPrevSeenCellID() = 0;
  virtual int GetPrevTaskCellID() = 0;
  virtual int GetNumTaskCellsReached() = 0;
  virtual void AddReachedTaskCell() = 0;
  virtual void SetPrevSeenCellID(int in_id) = 0;
  virtual void SetPrevTaskCellID(int in_id) = 0;

  virtual bool Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome) = 0;
  
  virtual cOrganism* GetNeighbor() = 0;
  virtual bool IsNeighborCellOccupied() = 0;
  virtual int GetNumNeighbors() = 0;
  virtual int GetFacing() = 0; //!< Returns the facing of this organism.
  virtual int GetNeighborCellContents() = 0;
  virtual void Rotate(int direction = 1) = 0;
  
  virtual void Breakpoint() = 0;
  
  virtual int GetInputAt(int& input_pointer) = 0;
  virtual void ResetInputs(cAvidaContext& ctx) = 0;
  virtual const tArray<int>& GetInputs() const = 0;
  virtual int Debug() = 0;
  virtual const tArray<double>& GetResources() = 0;
  virtual const tArray<double>& GetDemeResources(int deme_id) = 0;  
  virtual const tArray< tArray<int> >& GetCellIdLists() = 0; 
  virtual void UpdateResources(const tArray<double>& res_change) = 0;
  virtual void UpdateDemeResources(const tArray<double>& res_change) = 0;
  virtual void Die() = 0;
  virtual void Kaboom(int distance) = 0;
  virtual void SpawnDeme() = 0;
  virtual cOrgSinkMessage* NetReceive() = 0;
  virtual bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg) = 0;
  virtual int ReceiveValue() = 0;
  virtual void SellValue(const int data, const int label, const int sell_price, const int org_id) = 0;
  virtual int BuyValue(const int label, const int buy_price) = 0;
  virtual bool InjectParasite(cOrganism* parent, const cCodeLabel& label, const cGenome& injected_code) = 0;
  virtual bool UpdateMerit(double new_merit) = 0;
  virtual bool TestOnDivide() = 0;
  virtual bool SendMessage(cOrgMessage& msg) = 0;
  virtual bool BroadcastMessage(cOrgMessage& msg) = 0;
  virtual bool BcastAlarm(int jump_jabel, int bcast_range) = 0;
  virtual void DivideOrgTestamentAmongDeme(double value) = 0;
	virtual void SendFlash() = 0;
  
  virtual int GetStateGridID(cAvidaContext& ctx) = 0;
};

#endif
