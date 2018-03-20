//
//  ResourceRegistryInterface.h
//  Avida-Core
//
//  Created by Matthew Rupp on 2/15/18.
//

#ifndef ResourceRegistryInterface_h
#define ResourceRegistryInterface_h

#include "resources/Types.h"


class cAvidaContext;

namespace Avida {
  namespace Resource {
    class ResourceRegistryInterface{
    public:
      
      virtual int GetSize() const = 0;
      
      virtual bool DoesResourceExist(const ResName& res_name) const = 0;
      
      virtual cResource* GetResource(const ResName& res_name) = 0;
      virtual cNonSpatialResource* GetNonSpatialResource(const ResName& res_name) = 0;
      virtual cSpatialResource* GetSpatialResource(const ResName& res_name) = 0;
      virtual cGradientResource* GetGradientResource(const ResName& res_name) = 0;
      virtual cCellResource* GetCellResource(const ResName& res_name) = 0;  
      
      virtual const cResource* GetResource(const ResName& res_name) const = 0;
      virtual const cNonSpatialResource* GetNonSpatialResource(const ResName& res_name) const = 0;
      virtual const cSpatialResource* GetSpatialResource(const ResName& res_name) const = 0;
      virtual const cGradientResource* GetGradientResource(const ResName& res_name) const = 0;
      virtual const cCellResource* GetCellResource(const ResName& res_name) const = 0;     
      
      virtual ResourcePtrs& GetResources() = 0;
      virtual NonSpatialResourcePtrs& GetNonSpatialResources() = 0;
      virtual SpatialResourcePtrs& GetSpatialResources() = 0;
      virtual GradientResourcePtrs& GetGradientResources() = 0;
      virtual CellResourcePtrs& GetCellResources() = 0;
      
      virtual const ResourcePtrs& GetResources() const = 0;
      virtual const NonSpatialResourcePtrs& GetNonSpatialResources() const = 0;
      virtual const SpatialResourcePtrs& GetSpatialResources() const = 0;
      virtual const GradientResourcePtrs& GetGradientResources() const = 0;
      virtual const CellResourcePtrs& GetCellResources() const = 0;
      
      
      virtual TotalResAmount GetResAmount(const ResName& res_name) const = 0;
      virtual CellResAmount GetResAmount(const ResName& res_name, int cell_id) const = 0;
      
      virtual TotalResAmounts GetResAmounts() const = 0;
      virtual CellResAmounts GetResAmounts(int cell_id) const = 0;
      
      virtual TotalResAmounts GetNonSpatResAmounts() const  = 0;
      virtual TotalResAmounts GetSpatResAmounts() const  = 0;
      virtual TotalResAmounts GetGradResAmounts() const  = 0;
      
      virtual CellResAmounts GetNonSpatResAmounts(int cell_id) const  = 0;
      virtual CellResAmounts GetSpatResAmounts(int cell_id) const  = 0;
      virtual CellResAmounts GetGradResAmounts(int cell_id) const  = 0;
      
      virtual HabitatValues GetGradResHabitats(int cell_id) const = 0;
      
      virtual void FUTriggerDoUpdates(cAvidaContext& ctx) = 0; 
      
      //const Apto::Array< Apto::Array<int> >& GetCellIdLists() const; 
      
      virtual void ModifyResources(cAvidaContext& ctx, const ResAmounts& res_change) = 0;
      virtual void ModifyResources(cAvidaContext& ctx, int cell_id, const ResAmounts& res_change) = 0;
      
      virtual void SetResources(cAvidaContext& ctx, const TotalResAmounts& new_amount) = 0;
      virtual void SetResources(cAvidaContext& ctx, int cell_id, const CellResAmounts& new_amount) = 0;
      
//      virtual void UpdateRandomResources(cAvidaContext& ctx, const TotalResAmounts& res_change) = 0;
//      virtual void UpdateCellResources(cAvidaContext& ctx, 
//                                       const CellResAmounts& res_change, const int cell_id) = 0;
//      
      
      // Resource History
      //virtual cResourceHistory InitialHistory() = 0;
    };
  }
}









#endif /* ResourceRegistryInterface_h */
