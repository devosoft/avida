//
//  cResourceLibrary.cc
//  avida-core (static)
//
//  Created by Matthew Rupp on 2/7/18.
//

#ifndef cResourceLibrary_h
#define cResourceLibrary_h

#include "resources/Types.h"

namespace Avida{
  class Feedback;
}

class cResourceRegistry;

class cResourceLibrary 
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
    void AddResource(cGradientResource* grad_res);
    void AddResource(cCellResource* res);
    
    inline bool GlobalResourceExists(const ResName& res_name);
    
    inline bool DemeResourceExists(const ResName& res_name);
    
    inline bool ResourceExists(const ResName& res_name);
    
    inline GlobalResRegistry GetGlobalResRegistry();
    
    inline DemeResRegistry GetDemeResRegistry(int deme_id);
};


inline GlobalResRegistry cResourceLibrary::GetGlobalResRegistry()
{
  return m_global_resreg.get();
}

inline DemeResRegistry cResourceLibrary::GetDemeResRegistry(int deme_id)
{
  return (deme_id >= 0 && deme_id < (int) m_deme_resreg.size()) ? m_deme_resreg[deme_id].get() : nullptr;
}

inline bool cResourceLibrary::ResourceExists(const ResName& res_name)
{
  return GlobalResourceExists(res_name) || DemeResourceExists(res_name);
}

#endif 
