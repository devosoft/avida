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
#include "Types.h"

#include <map>

class cAvidaContext;
class cResourceHistory;
class cString;

class cResource;
class cResourceAcct;

class cSpatialResourceAcct;
class cNonSpatialResourceAcct;

class cCellResource;
class cNonSpatialResource;
class cSpatialResource;
class cGradientResource;

namespace Avida{
  class Feedback;
}


class cResourceRegistry
{
protected:
  
  Avida::Feedback& m_feedback;
  
  OwnedResources m_resource_ptrs;  // We own these pointers; they are unique_ptrs  
  Resources m_resources;              // This and the following point to resources in m_resource_ptrs
  NonSpatialResources m_ns_resources; // They do not need to be deleted.  They are provided for convenience.
  SpatialResources m_sp_resources;    // <<
  GradientResources m_gr_resources;   // <<
  CellResources m_cl_resources;       // <<
  
  OwnedAccountants m_accountant_ptrs; // We own these pointers
  ResourceAccts m_res_accts;          // This and the following point to resources in m_account_ptrs
  NonSpatialResourceAccts m_ns_accts; // They do not need to be deleted.  They are provided for convenience.
  SpatialResourceAccts m_sp_accts;    // <<
  GradientResourceAccts m_gr_accts;   // <<
  CellResourceAccts m_cl_accts;       // <<
  
  
  std::map<ResName, cResource*> m_name_map;
  
  void AddResource(cResource* res);
  

public:
  cResourceRegistry(Avida::Feedback& fb) 
  : m_feedback(fb) 
  {}
  
  cResourceRegistry(const cResourceRegistry&) = delete;
  cResourceRegistry& operator=(const cResourceRegistry&) = delete;
  
  
  ~cResourceRegistry();
  
  inline int GetSize() const; 
  
  void AddResource(cNonSpatialResource* nonspat_res);
  void AddResource(cSpatialResource* spat_res);
  void AddResource(cGradientResource* grad_res);
  void AddResource(cCellResource* res);
    
  cResource* GetResource(const ResName& res_name);
  cNonSpatialResource* GetNonSpatialResource(const ResName& res_name);
  cSpatialResource* GetSpatialResource(const ResName& res_name);
  cGradientResource* GetGradientResource(const ResName& res_name);
  cCellResource* GetCellResource(const ResName& res_name);  
  
  Resources& GetResources()  { return m_resources; }
  NonSpatialResources& GetNonSpatialResources()  { return m_ns_resources; }
  SpatialResources& GetSpatialResources() { return m_sp_resources; }
  GradientResources& GetGradientResources() { return m_gr_resources; }
  CellResources& GetCellResources() { return m_cl_resources; }
  
  cResourceAcct* GetResourceAcct(const ResName& res_name);
  cSpatialResourceAcct* GetSpatialResoureAcct(const ResName& res_name);
  cNonSpatialResourceAcct* GetNonSpatialResourceAcct(const ResName& res_name);
  cGradientResourceAcct* GetGradientResourceAcct(const ResName& res_name);
  
  bool DoesResourceExist(const ResName& res_name) const;
  
  
  ResAmount GetResourceAmount(cAvidaContext& ctx, const ResName& res_name) const;
  GlobalResAmounts GetResAmounts(cAvidaContext& ctx) const;
  CellResAmounts GetResAmounts(cAvidaContext& ctx, int cell_id);
  CellResAmounts GetFrozenResAmounts(cAvidaContext& ctx, int cell_id) const;  
  
  
  void TriggerDoUpdates(cAvidaContext& ctx); 
  
  //const Apto::Array< Apto::Array<int> >& GetCellIdLists() const; 
  
  
  void UpdateResources(cAvidaContext& ctx, const GlobalResAmounts& res_change);
  void UpdateRandomResources(cAvidaContext& ctx, const GlobalResAmounts& res_change);
  void UpdateResource(cAvidaContext& ctx, ResID id, ResAmount change);
  void UpdateCellResources(cAvidaContext& ctx, 
                           const CellResAmounts& res_change, const int cell_id);
  
  
  // Resource History
  cResourceHistory InitialHistory();
  
};

inline int cResourceRegistry::GetSize() const
{
 return m_resource_ptrs.size(); 
}

#endif
