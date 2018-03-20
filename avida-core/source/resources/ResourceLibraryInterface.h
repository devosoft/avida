//
//  ResourceLibraryInterface.h
//  Avida-Core
//
//  Created by Matthew Rupp on 2/15/18.
//

#ifndef ResourceLibraryInterface_h
#define ResourceLibraryInterface_h

namespace Avida {
  namespace Resource {
    class ResourceLibraryInterface
    {
    public:
      virtual bool GlobalResourceExists(const ResName& res_name) const = 0;
      virtual bool DemeResourceExists(const ResName& res_name) const = 0;
      virtual bool ResourceExists(const ResName& res_name) const = 0;
      
      virtual GlobalResRegistryPtr GetGlobalResReg() = 0;
      virtual DemeResRegistryPtr GetDemeResReg(int deme_id) = 0;
      
      virtual CellResAmounts GetGlobalAndDemeResAmounts(int cell_id, int deme_id) = 0;
    };
  }
}



#endif /* ResourceLibraryInterface_h */
