//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_CELL_WRAPPED_ACCESSORS_H
#define POPULATION_CELL_WRAPPED_ACCESSORS_H


class cHardwareCPU;
class cGenotype;
class cMutationRates;
class cOrganism;
class cPhenotype;
class cPopulationCell;
class avd_MissionControl;

cPopulationCell *GetPopulationCell(
  avd_MissionControl *mission_control,
  int cell_id
);
cOrganism *GetOrganism(cPopulationCell *);
cGenotype *GetGenotype(cOrganism *);
cMutationRates *GetMutationRates(cPopulationCell *);
cPhenotype *GetPhenotype(cOrganism *);
cHardwareCPU *GetHardwareCPU(cOrganism *);


#endif /* !POPULATION_CELL_WRAPPED_ACCESSORS_H */

