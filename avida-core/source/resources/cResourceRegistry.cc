#include "cResourceRegistry.h"
#include "cNonSpatialResource.h"
#include "cSpatialResource.h"
#include "cGradientResource.h"
#include "cCellResource.h"
#include "tools/PtrUtilities.h"
#include <memory>
#include <utility>

void cResourceRegistry::AddResource(cNonSpatialResource* res)
{
  assert(res);
  
  if (m_name_map.count(res->GetName()) == 0){
    // Register our resource; m_resource_ptrs contains the only ownership
    m_ns_resources.push_back(res);
    auto base_res = std::unique_ptr<cResource>(res);
    m_resource_ptrs.push_back(std::move(base_res));
    
    // Register our accountant; m_accountant_ptrs contains the only ownership
    auto acct = std::unique_ptr<cNonSpatialResourceAcct>(new cNonSpatialResourceAcct(*res));
    m_ns_accts.push_back(acct.get());
    auto base_acct = static_cast_ptr<cResourceAcct>(acct);
    m_accountant_ptrs.push_back(std::move(base_acct));
  } else {
    m_feedback.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

void cResourceRegistry::AddResource(cSpatialResource* res)
{
  assert(res);
  
 if (m_name_map.count(res->GetName()) == 0){
    // Register our resource; m_resource_ptrs contains the only ownership
    m_sp_resources.push_back(res);
    auto base_res = std::unique_ptr<cResource>(res);
    m_resource_ptrs.push_back(std::move(base_res));
    
    // Register our accountant; m_accountant_ptrs contains the only ownership
    auto acct = std::unique_ptr<cSpatialResourceAcct>(new cSpatialResourceAcct(*res));
    m_sp_accts.push_back(acct.get());
    auto base_acct = static_cast_ptr<cResourceAcct>(acct);
    m_accountant_ptrs.push_back(std::move(base_acct));
  } else {
    m_feedback.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

void cResourceRegistry::AddResource(cGradientResource* res)
{
  
}

void cResourceRegistry::AddResource(cCellResource* res)
{
  
}
