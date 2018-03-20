#include "cResourceRegistry.h"
#include <memory>
#include <utility>

using namespace Avida::Resource;



bool cResourceRegistry::AddResource(cNonSpatialResource* res, Feedback& fb)
{
  assert(res);
  
  if (m_name_res_map.count(res->GetName()) == 0){
    // Register our resource; m_resource_ptrs contains the only ownership
    m_ns_resources.push_back(res);
    m_resources.push_back(res);
    auto base_res = std::unique_ptr<cResource>(res);
    m_resource_ptrs.push_back(std::move(base_res));
  } else {
    fb.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

bool cResourceRegistry::AddResource(cSpatialResource* res, Feedback& fb)
{
  assert(res);
  
  if (m_name_res_map.count(res->GetName()) == 0){
    // Register our resource; m_resource_ptrs contains the only ownership
    m_sp_resources.push_back(res);
    m_resources.push_back(res);
    auto base_res = std::unique_ptr<cResource>(res);
    m_resource_ptrs.push_back(std::move(base_res));
  } else {
    fb.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

bool cResourceRegistry::AddResource(cGradientResource* res, Feedback& fb)
{
  assert(res);
  if (m_name_res_map.count(res->GetName()) == 0){
    // Register our resource; m_resource_ptrs contains the only ownership
    m_gr_resources.push_back(res);
    m_resources.push_back(res);
    auto base_res = std::unique_ptr<cResource>(res);
    m_resource_ptrs.push_back(std::move(base_res));
  } else {
    fb.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}




int cResourceRegistry::GetSize() const
{
 return m_resource_ptrs.size(); 
}


// RETRIEVE RESOURCES ==========================================================================

cResource* cResourceRegistry::GetResource(const ResName& res_name)
{
  const auto found = m_name_res_map.find(res_name);
  return (found != m_name_res_map.end()) ? found->second : nullptr; 
}

cNonSpatialResource* cResourceRegistry::GetNonSpatialResource(const ResName& res_name)
{
  cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<cNonSpatialResource*>(res) : nullptr;
}

cSpatialResource* cResourceRegistry::GetSpatialResource(const ResName& res_name)
{
  cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<cSpatialResource*>(res) : nullptr;

}

cGradientResource* cResourceRegistry::GetGradientResource(const ResName& res_name)
{
  cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<cGradientResource*>(res) : nullptr;

}

cCellResource* cResourceRegistry::GetCellResource(const ResName& res_name)
{
  cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<cCellResource*>(res) : nullptr;
}  


const cResource* cResourceRegistry::GetResource(const ResName& res_name) const
{
  const auto found = m_name_res_map.find(res_name);
  return (found != m_name_res_map.end()) ? found->second : nullptr; 
}

const cNonSpatialResource* cResourceRegistry::GetNonSpatialResource(const ResName& res_name) const
{
  const cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<const cNonSpatialResource*>(res) : nullptr;
}

const cSpatialResource* cResourceRegistry::GetSpatialResource(const ResName& res_name) const
{
  const cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<const cSpatialResource*>(res) : nullptr;

}

const cGradientResource* cResourceRegistry::GetGradientResource(const ResName& res_name) const
{
  const cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<const cGradientResource*>(res) : nullptr;

}

const cCellResource* cResourceRegistry::GetCellResource(const ResName& res_name) const
{
  const cResource* res = GetResource(res_name);
  return (res != nullptr) ? dynamic_cast<const cCellResource*>(res) : nullptr;
}  


ResourcePtrs& cResourceRegistry::GetResources()
{ 
  return m_resources; 
}

NonSpatialResourcePtrs& cResourceRegistry::GetNonSpatialResources()  
{ 
  return m_ns_resources; 
}

SpatialResourcePtrs& cResourceRegistry::GetSpatialResources() 
{ 
  return m_sp_resources; 
}

GradientResourcePtrs& cResourceRegistry::GetGradientResources()
{ 
  return m_gr_resources; 
}

CellResourcePtrs& cResourceRegistry::GetCellResources() 
{ 
  return m_cl_resources; 
}


const ResourcePtrs& cResourceRegistry::GetResources() const
{ 
  return m_resources; 
}

const NonSpatialResourcePtrs& cResourceRegistry::GetNonSpatialResources() const  
{ 
  return m_ns_resources; 
}

const SpatialResourcePtrs& cResourceRegistry::GetSpatialResources() const
{ 
  return m_sp_resources; 
}

const GradientResourcePtrs& cResourceRegistry::GetGradientResources() const
{ 
  return m_gr_resources; 
}

const CellResourcePtrs& cResourceRegistry::GetCellResources() const 
{ 
  return m_cl_resources; 
}


TotalResAmount cResourceRegistry::GetResAmount(const ResName& res_name) const
{
  const cResource* res = GetResource(res_name);
  if (res == nullptr){
    return NO_RESOURCE;
  }
  return res->GetBasicAcct()->GetTotalAmount();
}


CellResAmount cResourceRegistry::GetResAmount(const ResName& res_name, int cell_id) const
{
  const cResource* res = GetResource(res_name);
  if (res == nullptr){
    return NO_RESOURCE;
  }
  return res->GetBasicAcct()->GetCellAmount(cell_id);
}


TotalResAmounts cResourceRegistry::GetResAmounts() const
{
  TotalResAmounts amts;
  for (auto res : m_resources){
    amts.push_back(res->GetBasicAcct()->GetTotalAmount());
  }
  return amts;
}


CellResAmounts cResourceRegistry::GetResAmounts(int cell_id) const
{
  CellResAmounts amts;
  for (auto res : m_resources){
    amts.push_back(res->GetBasicAcct()->GetCellAmount(cell_id));
  }
  return amts;
}


TotalResAmounts cResourceRegistry::GetNonSpatResAmounts() const
{
  TotalResAmounts amts;
  for (auto res : m_ns_resources){
    amts.push_back(res->GetBasicAcct()->GetTotalAmount());
  }
  return amts;
}


TotalResAmounts cResourceRegistry::GetSpatResAmounts() const
{
  TotalResAmounts amts;
  for (auto res : m_sp_resources){
    amts.push_back(res->GetBasicAcct()->GetTotalAmount());
  }
  return amts;
}


TotalResAmounts cResourceRegistry::GetGradResAmounts() const
{
  TotalResAmounts amts;
  for (auto res : m_gr_resources){
    amts.push_back(res->GetBasicAcct()->GetTotalAmount());
  }
  return amts;
}


CellResAmounts cResourceRegistry::GetNonSpatResAmounts(int cell_id) const
{
  CellResAmounts amts;
  for (auto res : m_ns_resources){
    amts.push_back(res->GetBasicAcct()->GetCellAmount(cell_id));
  }
  return amts;
}

CellResAmounts cResourceRegistry::GetSpatResAmounts(int cell_id) const
{
  CellResAmounts amts;
  for (auto res : m_sp_resources){
    amts.push_back(res->GetBasicAcct()->GetCellAmount(cell_id));
  }
  return amts;
}

CellResAmounts cResourceRegistry::GetGradResAmounts(int cell_id) const
{
  CellResAmounts amts;
  for (auto res : m_gr_resources){
    amts.push_back(res->GetBasicAcct()->GetCellAmount(cell_id));
  }
  return amts;
}

HabitatValues cResourceRegistry::GetGradResHabitats(int cell_id) const
{
  HabitatValues habits;
  for (auto res : m_gr_resources){
    habits.push_back(res->GetHabitat());
  }
  return habits;
}

void ModifyResources(cAvidaContext& ctx, int cell_id, const ResAmounts& res_change)
{
  
}


