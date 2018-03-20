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
#include "resources/Types.h"

#include "ResourceRegistryInterface.h"

#include "cResourceRegistry.h"
#include "cNonSpatialResource.h"
#include "cSpatialResource.h"
#include "cGradientResource.h"
#include "cCellResource.h"
#include "cResourceHistory.h"

#include <map>

class cAvidaContext;
class cPopulation; 

namespace Avida {
  
  class Feedback;
  
  namespace Resource {
    
    class cResourceRegistry : public ResourceRegistryInterface
    {
      
    protected:

      OwnedResourcePtrs m_resource_ptrs;  // We own these pointers; they are unique_ptrs  
      ResourcePtrs m_resources;              // This and the following point to resources in m_resource_ptrs
      NonSpatialResourcePtrs m_ns_resources; // They do not need to be deleted.  They are provided for convenience.
      SpatialResourcePtrs m_sp_resources;    // <<
      GradientResourcePtrs m_gr_resources;   // <<
      CellResourcePtrs m_cl_resources;       // <<
      
      
      std::map<ResName, cResource*> m_name_res_map;  
      
    public:
      cResourceRegistry() 
      {}
      
      cResourceRegistry(const cResourceRegistry&) = delete;
      cResourceRegistry& operator=(const cResourceRegistry&) = delete;
      
      
      ~cResourceRegistry();
      
      inline int GetSize() const override; 
      inline virtual bool DoesResourceExist(const ResName& res_name) const override;
      
      bool AddResource(cNonSpatialResource* nonspat_res, Feedback& fb);
      bool AddResource(cSpatialResource* spat_res, Feedback& fb);
      bool AddResource(cGradientResource* grad_res, Feedback& fb);
      
      virtual cResource* GetResource(const ResName& res_name) override;
      virtual cNonSpatialResource* GetNonSpatialResource(const ResName& res_name) override;
      virtual cSpatialResource* GetSpatialResource(const ResName& res_name) override;
      virtual cGradientResource* GetGradientResource(const ResName& res_name) override;
      virtual cCellResource* GetCellResource(const ResName& res_name) override;  
      
      virtual const cResource* GetResource(const ResName& res_name) const override;
      virtual const cNonSpatialResource* GetNonSpatialResource(const ResName& res_name) const override;
      virtual const cSpatialResource* GetSpatialResource(const ResName& res_name) const override;
      virtual const cGradientResource* GetGradientResource(const ResName& res_name) const override;
      virtual const cCellResource* GetCellResource(const ResName& res_name) const override;  
      
      virtual ResourcePtrs& GetResources() override;
      virtual NonSpatialResourcePtrs& GetNonSpatialResources() override;
      virtual SpatialResourcePtrs& GetSpatialResources() override;
      virtual GradientResourcePtrs& GetGradientResources()  override;
      virtual CellResourcePtrs& GetCellResources() override;
      
      virtual const ResourcePtrs& GetResources() const override;
      virtual const NonSpatialResourcePtrs& GetNonSpatialResources() const override;
      virtual const SpatialResourcePtrs& GetSpatialResources() const override;
      virtual const GradientResourcePtrs& GetGradientResources()  const override;
      virtual const CellResourcePtrs& GetCellResources() const override;
      
      virtual TotalResAmount GetResAmount(const ResName& res_name) const override;
      virtual CellResAmount GetResAmount(const ResName& res_name, int cell_id) const override;
      
      virtual TotalResAmounts GetResAmounts() const override;
      virtual CellResAmounts GetResAmounts(int cell_id) const override;
      
      virtual TotalResAmounts GetNonSpatResAmounts() const override;
      virtual TotalResAmounts GetSpatResAmounts() const override;
      virtual TotalResAmounts GetGradResAmounts() const override;
      
      virtual CellResAmounts GetNonSpatResAmounts(int cell_id) const override;
      virtual CellResAmounts GetSpatResAmounts(int cell_id) const override;
      virtual CellResAmounts GetGradResAmounts(int cell_id) const override;
      
      virtual HabitatValues GetGradResHabitats(int cell_id) const override;
      
      virtual void ModifyResources(cAvidaContext& ctx, const ResAmounts& res_change) override;
      virtual void ModifyResources(cAvidaContext& ctx, int cell_id, const ResAmounts& res_change) override;
      
      virtual void FUTriggerDoUpdates(cAvidaContext& ctx) override; 

      
      // Resource History
      virtual cResourceHistory InitialHistory();
    };
  }
}




#endif
