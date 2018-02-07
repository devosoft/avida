#include "cResourceRegistry.h"
#include "cNonSpatialResource.h"
#include "cSpatialResource.h"
#include "cGradientResource.h"
#include "cCellResource.h"
#include "tools/PtrUtilities.h"
#include <memory>
#include <utility>

void cResourceRegistry::AddResource(std::unique_ptr<cNonSpatialResource>&& res)
{
  assert(res);
  
  if (m_name_map.count(res->GetName()) == 0){
    
    // Register our resource; m_resource_ptrs contains the only ownership
    auto lres = res.get();
    m_ns_resources.push_back(lres);
    auto base_res = static_cast_ptr<cResource>(res);  //Must upcast explicitly
    m_resource_ptrs.push_back(std::move(base_res));
    
    // Create and register our accountant; only m_
    auto acct = std::unique_ptr<cNonSpatialResourceAcct>(new cNonSpatialResourceAcct(*lres));
    m_ns_accts.push_back(acct.get());
    auto base_acct = static_cast_ptr<cResourceAcct>(acct);
    m_accountant_ptrs.push_back(std::move(base_acct));
  } else {
    m_feedback.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

void cResourceRegistry::AddResource(std::unique_ptr<cSpatialResource>&& res)
{
  assert(res);
  
  if (m_name_map.count(res->GetName()) == 0){
    
    // Register our resource; m_resource_ptrs contains the only ownership
    auto lres = res.get();
    m_sp_resources.push_back(lres);
    auto base_res = static_cast_ptr<cResource>(res);  //Must upcast explicitly
    m_resource_ptrs.push_back(std::move(base_res));
    
    // Create and register our accountant; only m_
    auto acct = std::unique_ptr<cSpatialResourceAcct>(new cSpatialResourceAcct(*lres));
    m_sp_accts.push_back(acct.get());
    auto base_acct = static_cast_ptr<cResourceAcct>(acct);
    m_accountant_ptrs.push_back(std::move(base_acct));
  } else {
    m_feedback.Error("Resource of name %s is already registered.", res->GetName().GetData());
  }
}

void cResourceRegistry::AddResource(std::unique_ptr< res)
{
  
}

void cResourceRegistry::AddResource(cCellResource* res)
{
  
}
