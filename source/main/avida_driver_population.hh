//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_POPULATION_HH
#define AVIDA_DRIVER_POPULATION_HH

#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

class cEnvironment;
class cEventFactoryManager;
class cEventList;
class cString;
class cPopulation;

class cAvidaDriver_Population : public cAvidaDriver_Base {
protected:
  cPopulation * population;
  cEventFactoryManager * event_manager;
  cEventList * event_list;

public:
  cPopulation &GetPopulation(){ return *population; }

  void GetEvents();
  /**
   * Processes one complete update.
   *
   * Returns true when finished.
   **/
  virtual bool ProcessUpdate();
  virtual void ProcessOrganisms();
  void ReadEventListFile(const cString & filename="event_list");
  void SyncEventList();
			
  cAvidaDriver_Population(cEnvironment & environment);
  virtual ~cAvidaDriver_Population();

  virtual void Run();
  virtual void NotifyUpdate();
};

#endif
