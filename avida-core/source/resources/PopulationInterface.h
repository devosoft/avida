//
//  PopulationInterface.h
//  Avida-Core
//
//  Created by Matthew Rupp on 3/20/18.
//

#ifndef PopulationInterface_h
#define PopulationInterface_h

namespace Avida {
  namespace Resource {
    class AbstractGradientPopulationInterface
    {
      public:
        virtual void ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per, const bool hammer) = 0;
        virtual void ExecuteDeadlyResource(cAvidaContext& ctx, const int cell_id, const double odds, const bool hammer) = 0;
        virtual void ExecuteDamagingResource(cAvidaContext& ctx, const int cell_id, const double damage, const bool hammer) = 0;
      }
  }
}

#endif /* PopulationInterface.h */
