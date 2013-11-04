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

#include "avida/systematics/Types.h"

namespace Avida {
  class Genome;
  class InstructionSequence;
};

class cAvidaContext;
class cOrganism;
class cOrgSinkMessage;
class cPopulationCell;
class cString;

using namespace Avida;


class cOrgInterface
{
private:
  cOrgInterface(const cOrgInterface&); // @not_implemented
  cOrgInterface& operator=(const cOrgInterface&); // @not_implemented
  
public:
  cOrgInterface() { ; }
  virtual ~cOrgInterface() { ; }

  virtual const Apto::Array<cOrganism*, Apto::Smart>& GetLiveOrgList() const = 0;
  virtual int GetCellID() = 0;
  virtual void SetCellID(int in_id) = 0;
  virtual int GetCellXPosition() = 0;
  virtual int GetCellYPosition() = 0;
  
  virtual bool Divide(cAvidaContext& ctx, cOrganism* parent, const Genome& offspring_genome) = 0;
  
  virtual cOrganism* GetNeighbor() = 0;
  virtual bool IsNeighborCellOccupied() = 0;
  virtual int GetNumNeighbors() = 0;
  virtual void GetNeighborhoodCellIDs(Apto::Array<int>& list) = 0;
  virtual void GetAVNeighborhoodCellIDs(Apto::Array<int>& list, int av_num = 0) = 0;

  virtual int GetFacing() = 0; //!< Returns the facing of this organism.
  virtual int GetFacedCellID() = 0;
  virtual int GetFacedDir() = 0; // Returns the human interpretable facing of this org.
  virtual void KillOrganism(cAvidaContext& ctx, int cell_id) = 0;
  virtual void Rotate(cAvidaContext& ctx, int direction = 1) = 0;
  
  virtual bool GetLGTFragment(cAvidaContext& ctx, int region, const Genome& dest_genome, InstructionSequence& seq) = 0;
  
  virtual int GetInputAt(int& input_pointer) = 0;
  virtual void ResetInputs(cAvidaContext& ctx) = 0;
  virtual const Apto::Array<int>& GetInputs() const = 0;
  virtual const Apto::Array<double>& GetResources(cAvidaContext& ctx) = 0; 
  virtual double GetResourceVal(cAvidaContext& ctx, int res_id) = 0;
  virtual const Apto::Array<double>& GetFacedCellResources(cAvidaContext& ctx) = 0; 
  virtual double GetFacedResourceVal(cAvidaContext& ctx, int res_id) = 0;
  virtual const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx) = 0;
  virtual double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) = 0;
  virtual double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) = 0;
  virtual const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) = 0;
  virtual const Apto::Array< Apto::Array<int> >& GetCellIdLists() = 0; 

  virtual int GetCurrPeakX(cAvidaContext& ctx, int res_id) = 0;
  virtual int GetCurrPeakY(cAvidaContext& ctx, int res_id) = 0;
  virtual int GetFrozenPeakX(cAvidaContext& ctx, int res_id) = 0; 
  virtual int GetFrozenPeakY(cAvidaContext& ctx, int res_id) = 0;
  virtual void TriggerDoUpdates(cAvidaContext& ctx) = 0;
  virtual void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change) = 0;
  virtual void Die(cAvidaContext& ctx) = 0;
  virtual bool UpdateMerit(double new_merit) = 0;

  virtual bool Move(cAvidaContext& ctx, int src_id, int dest_id) = 0;

  virtual void AddLiveOrg() = 0;
  virtual void RemoveLiveOrg() = 0;
  
  virtual void DecNumPreyOrganisms() = 0;
  virtual void DecNumPredOrganisms() = 0;
  virtual void DecNumTopPredOrganisms() = 0;
  virtual void IncNumPreyOrganisms() = 0;
  virtual void IncNumPredOrganisms() = 0;
  virtual void IncNumTopPredOrganisms() = 0;
  virtual void AttackFacedOrg(cAvidaContext& ctx, int loser) = 0;
  
  virtual void TryWriteBirthLocData(int org_idx) = 0;
  virtual void InjectPreyClone(cAvidaContext& ctx, int gen_id) = 0;
  virtual void KillRandPred(cAvidaContext& ctx, cOrganism* org) = 0;
  virtual void KillRandPrey(cAvidaContext& ctx, cOrganism* org) = 0;
  virtual void TryWriteLookData(cString& string) = 0;
  virtual void TryWriteLookOutput(cString& string) = 0;
  virtual void TryWriteLookEXOutput(cString& string) = 0;
  
  virtual bool HasOutputAV(int av_num = 0) = 0;
  virtual bool FacedHasOutputAV(int av_num = 0) = 0;
  virtual bool FacedHasAV(int av_num = 0) = 0;
  virtual bool FacedHasPredAV(int av_num = 0) = 0;
  virtual bool FacedHasPreyAV(int av_num = 0) = 0;
  virtual void AddIOAV(cAvidaContext& ctx, int av_cell_id, int av_facing, bool input, bool output) = 0;
  virtual void AddPredPreyAV(cAvidaContext& ctx, int av_cell_id) = 0;
  virtual void SwitchPredPrey(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual void RemoveAllAV() = 0;
  virtual int GetAVFacing(int av_num = 0) = 0;
  virtual int GetAVCellID(int av_num = 0) = 0;
  virtual int GetAVFacedCellID(int av_num = 0) = 0;
  virtual int GetAVNumNeighbors(int av_num = 0) = 0;
  virtual int FindAV(bool input, bool output, int av_num = 0) = 0;
  virtual void SetAVFacing(cAvidaContext& ctx, int av_facing, int av_num = 0) = 0;
  virtual bool SetAVCellID(cAvidaContext& ctx, int av_cell_id, int av_num = 0) = 0;
  virtual void SetAVFacedCellID(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual bool MoveAV(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual bool RotateAV(cAvidaContext& ctx, int increment, int av_num = 0) = 0;
  virtual cOrganism* GetRandFacedAV(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual cOrganism* GetRandFacedPredAV(int av_num = 0) = 0;
  virtual cOrganism* GetRandFacedPreyAV(int av_num = 0) = 0;

  virtual Apto::Array<cOrganism*> GetFacedAVs(int av_num = 0) = 0;
  virtual Apto::Array<cOrganism*> GetCellAVs(int av_cell_id, int av_num=0) =0;
  virtual Apto::Array<cOrganism*> GetFacedPreyAVs(int av_num = 0) = 0;
  virtual const Apto::Array<double>& GetAVResources(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual double GetAVResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0) = 0;
  virtual const Apto::Array<double>& GetAVFacedResources(cAvidaContext& ctx, int av_num = 0) = 0;
  virtual double GetAVFacedResourceVal(cAvidaContext& ctx, int res_id, int av_num = 0) = 0;
  virtual void UpdateAVResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, int av_num = 0) = 0;
};

#endif
