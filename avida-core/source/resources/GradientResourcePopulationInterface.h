//
//  GradientResourcePopulationInterface.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 3/20/18.
//

#ifndef GradientResourcePopulationInterface_h
#define GradientResourcePopulationInterface_h

#include "PopulationInterface.h"

class cPopulation;

namespace Avida {
  namespace Resource {
    class GradientResourcePopulationInterface : public AbstractGradientPopulationInterface
    {
      public: 
        explicit GradientResourcePopulationInterface(cPopulation* pop);
    };
  }
}

#endif /* GradientResourcePopulationInterface_h */
