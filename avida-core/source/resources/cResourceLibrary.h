//
//  cResourceLibrary.cc
//  avida-core (static)
//
//  Created by Matthew Rupp on 2/7/18.
//

#ifndef cResourceLibrary_h
#define cResourceLibrary_h

#include "resources/Types.h"
#include "resources/ResourceLibraryInterface.h"
#include "cResourceRegistry.h"


namespace Avida{
  class Feedback;
}


namespace Avida {
  namespace Resource {
    class cResourceLibrary : public ResourceLibraryInterface
    {
    protected:
      std::unique_ptr<cResourceRegistry> m_global_resreg;
      std::vector< std::unique_ptr<cResourceRegistry> > m_deme_resreg;
      
    public:
      
      explicit cResourceLibrary(int num_demes, Avida::Feedback& fb);
      cResourceLibrary(const cResourceLibrary&) = delete;
      cResourceLibrary& operator=(const cResourceLibrary&) = delete;
      
      void AddResource(cNonSpatialResource* nonspat_res);
      void AddResource(cSpatialResource* spat_res);
      void AddResource(cGradientResource* grad_res, cPopulation* pop);
      void AddResource(cCellResource* res);
      
      inline bool GlobalResourceExists(const ResName& res_name) const override;
      
      inline bool DemeResourceExists(const ResName& res_name) const override;
      
      inline bool ResourceExists(const ResName& res_name) const override;
      
      inline GlobalResRegistryPtr GetGlobalResReg() override;
      
      inline DemeResRegistryPtr GetDemeResReg(int deme_id) override;
      
      CellResAmounts GetGlobalAndDemeResAmounts(int cell_id, int deme_id) override;
    };
    inline GlobalResRegistryPtr cResourceLibrary::GetGlobalResReg()
    {
      return m_global_resreg.get();
    }
    
    inline DemeResRegistryPtr cResourceLibrary::GetDemeResReg(int deme_id)
    {
      return (deme_id >= 0 && deme_id < (int) m_deme_resreg.size()) ? m_deme_resreg[deme_id].get() : nullptr;
    }
    
    inline bool cResourceLibrary::ResourceExists(const ResName& res_name) const
    {
      return GlobalResourceExists(res_name) || DemeResourceExists(res_name);
    }
    
    inline bool cResourceLibrary::GlobalResourceExists(const ResName& res_name) const
    {
      return (m_global_resreg->GetResource(res_name) != nullptr);
    }
    
    inline bool cResourceLibrary::DemeResourceExists(const ResName& res_name) const
    {
      return (m_deme_resreg.size() > 0) ? m_deme_resreg[0]->GetResource(res_name) != nullptr : false;
    }
    
  }
}





#endif 
