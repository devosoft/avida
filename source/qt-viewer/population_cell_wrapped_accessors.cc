//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////



#ifndef POPULATION_CELL_WRAPPED_ACCESSORS_H
#include "population_cell_wrapped_accessors.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
#include "population_cell.hh"
#include "population.hh"
#include "organism.hh"


using namespace std;


cPopulationCell *GetPopulationCell(
  avd_MissionControl *mission_control,
  int cell_id
){
  if(mission_control == NULL) return NULL;
  cPopulation *population = mission_control->getPopulation();
  if(population == NULL) return NULL;
  return &population->GetCell(cell_id);
}

cOrganism *GetOrganism(cPopulationCell *cell){
  if(cell == NULL) return NULL;
  else return cell->GetOrganism();
}

cGenotype *GetGenotype(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return organism->GetGenotype();
}

cMutationRates *GetMutationRates(cPopulationCell *cell){
  if(cell == NULL) return NULL;
  else return &cell->MutationRates();
}

cPhenotype *GetPhenotype(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return &organism->GetPhenotype();
}

cHardwareCPU *GetHardwareCPU(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return (cHardwareCPU *) &organism->GetHardware();
}
