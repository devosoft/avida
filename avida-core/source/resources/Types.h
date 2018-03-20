//
//  Types.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/23/18.
//

#ifndef Types_h
#define Types_h

#include "cString.h"
#include <vector>



namespace Avida
{
  namespace Resource {
    
    class cResource;
    class cRatedResource;
    class cNonSpatialResource;
    class cSpatialResource;
    class cGradientResource;
    class cCellResource;
    
    
    class cResourceAcct;
    class cNonSpatialResourceAcct;
    class cSpatialResourceAcct;
    class cGradientResourceAcct;
    class cCellResourceAcct;
    class cResourceRegistry;
    
    using ResID = int;
    using ResName = cString;
    using ResRate = double;
    using ResDescr = cString;
    
    using ResID = int;
    using ResAmount = double;
    using CellResAmount = double;
    using DemeResAmount = double;
    using TotalResAmount = double;
    
    using ResAmounts = std::vector<ResAmount>;
    using CellResAmounts = std::vector<CellResAmount>;
    using DemeResAmounts = std::vector<DemeResAmount>;
    using TotalResAmounts = std::vector<TotalResAmount>;
    
    using HabitatValue = int;
    using HabitatValues = std::vector<HabitatValue>;
    
    using SpatialResAmounts = std::vector< Apto::Array<CellResAmount> >;
    
    using OwnedResourcePtrs = std::vector< std::unique_ptr<cResource> >;
    using ResourcePtrs = std::vector< cResource* >;
    using NonSpatialResourcePtrs = std::vector< cNonSpatialResource* >;
    using SpatialResourcePtrs = std::vector< cSpatialResource* >;
    using GradientResourcePtrs = std::vector< cGradientResource* >;
    using CellResourcePtrs = std::vector< cCellResource* >;
    
    using OwnedAccountantPtrs = std::vector< std::unique_ptr<cResourceAcct> >;
    using ResourceAcctPtrs = std::vector<cResourceAcct*>;
    using NonSpatialResourceAcctPtrs = std::vector<cNonSpatialResourceAcct*>;
    using SpatialResourceAcctPtrs = std::vector<cSpatialResourceAcct*>;
    using GradientResourceAcctPtrs = std::vector<cGradientResourceAcct*>;
    using CellResourceAcctPtrs = std::vector<cCellResourceAcct*>;
    
    using DemeResRegistryPtr = cResourceRegistry*;
    using GlobalResRegistryPtr = cResourceRegistry*;
    
    constexpr ResAmount NO_RESOURCE = 0.0;
  }
}

#endif /* Types_h */

