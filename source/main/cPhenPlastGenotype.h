/*
*  cPhenPlastGenotype.h
*  Avida
*
*  Created by Matthew Rupp on 7/27/07.
*/


#ifndef cPhenPlastGenotype_h
#define cPhenPlastGenotype_h

#include <set>
#include <utility>

#ifndef functions_h
#include "functions.h"
#endif
#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringList_h
#include "cStringList.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cPlasticPhenotype_h
#include "cPlasticPhenotype.h"
#endif
#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif
#ifndef cHardwareManager_h
#include "cHardwareManager.h"
#endif

class cAvidaContext;
class cTestCPU;
class cWorld;

/**
 * This class examines a genotype for evidence of phenotypic plasticity. 
**/
 

class cPhenPlastGenotype
{
  private:

  typedef set<cPhenotype*, cPhenotype::lt_phenotype  > UniquePhenotypes;  //Actually, these are cPlasticPhenotype*
    cGenome m_genome;
    int m_num_trials;  
    UniquePhenotypes m_unique;
    cWorld* m_world;

  public:
      cPhenPlastGenotype(const cGenome& in_genome, int num_trials, cWorld* world, cAvidaContext& ctx);
    ~cPhenPlastGenotype();
    
    int GetNumPhenotypes() const { return m_unique.size(); }
    cPlasticPhenotype GetPlasticPhenotype(int num) const;
    
};

#endif

