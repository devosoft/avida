/*
 *  cResourceRegistry.h
 *  Avida
 *
 *  Called "resource_lib.hh" prior to 12/5/05.
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

#ifndef cResourceRegistry_h
#define cResourceRegistry_h

#include "avida/core/Types.h"
#include "cResourceCount.h"
#include "cSpatialResource.h"
#include "cNonSpatialResource.h"

class cResource;
class cResourceHistory;
class cString;




class cResourceRegistry
{
private:
  
  Apto::Array<cAbstractResourceAcct> m_resource_accts;
  Apto::Array<cResource*> m_resources;
  
  cResourceRegistry(const cResourceRegistry&); // @not_implemented
  cResourceRegistry& operator=(const cResourceRegistry&); // @not_implemented
  
public:
  cResourceRegistry();
  ~cResourceRegistry();

  int GetSize() const { return m_resources.GetSize(); }

  cResource* AddResource(const cString& res_name);
  cResource* GetResource(const cString& res_name) const;
  inline cResource* GetResource(int id) const { return m_resources[id]; }
  const cResourceHistory& GetInitialResourceLevels() const;
  bool DoesResourceExist(const cString& res_name);
  void SetResourceIndex(cResource* res);
  
  
  Apto::Array<double> GetResAbundances(cAvidaContext& ctx) const;
  Apto::Array<double> GetResAbundances(cAvidaContext& ctx, int cell_id);
  
  Apto::Array<double> GetFrozenResources(cAvidaContext& ctx, int cell_id) const;
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  
  
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;

//  const Apto::Array<double>& GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx) { return GetDeme(deme_id).GetDemeResourceCount().GetCellResources( GetDeme(deme_id).GetRelativeCellID(cell_id), ctx ); } 

  void TriggerDoUpdates(cAvidaContext& ctx); // { m_resource_count.UpdateResources(ctx); }
  
  const Apto::Array< Apto::Array<int> >& GetCellIdLists() const; // { return resource_count.GetCellIdLists(); }

  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const; // { return m_resource_count.GetCurrPeakX(ctx, res_id); } 
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const; // { return m_resource_count.GetCurrPeakY(ctx, res_id); } 
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const; // { return m_resource_count.GetFrozenPeakX(ctx, res_id); } 
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const; // { return m_resource_count.GetFrozenPeakY(ctx, res_id); } 
  double GetResource(cAvidaContext& ctx, int res_id) const { return m_resource_accts[res_id].GetTotalAbundance(); }
  Apto::Array<int>* GetWallCells(int res_id); // { return m_resource_count.GetWallCells(res_id); }
  
  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateRandomResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateResource(cAvidaContext& ctx, int id, double change);
  void UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  
  void SetResource(cAvidaContext& ctx, int res_id, double new_level);
  void SetResource(cAvidaContext& ctx, const cString res_name, double new_level);
  void SetResourceInflow(const cString res_name, double new_level);
  void SetResourceOutflow(const cString res_name, double new_level);

  
  

};

#endif
