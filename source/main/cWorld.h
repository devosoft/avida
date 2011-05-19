/*
 *  cWorld.h
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cWorld_h
#define cWorld_h

#include "apto/core/SmartPtr.h"

#include "cAvidaConfig.h"
#include "cAvidaContext.h"
#include "cDataFileManager.h"
#include "cRandom.h"

#include <cassert>

namespace Avida {
  class cWorldDriver;
  
  namespace Data {
    class cManager;
    class cProvider;
  };
};

class cAnalyze;
class cAnalyzeGenotype;
class cClassificationManager;
class cEnvironment;
class cEventList;
class cHardwareManager;
class cOrganism;
class cPopulation;
class cMerit;
class cPopulationCell;
class cStats;
class cTestCPU;
class cUserFeedback;
template<class T> class tDataEntry;
template<class T> class tDictionary;

using namespace Avida;


class cWorld
{
protected:
  cString m_working_dir;
  
  cAnalyze* m_analyze;
  cAvidaConfig* m_conf;
  cAvidaContext m_ctx;
  cClassificationManager* m_class_mgr;
  cDataFileManager* m_datafile_mgr;
  cEnvironment* m_env;
  cEventList* m_event_list;
  cHardwareManager* m_hw_mgr;
  cPopulation* m_pop;
  Apto::SmartPtr<cStats, Apto::ThreadSafeRefCount> m_stats;
  cWorldDriver* m_driver;
  
  Avida::Data::cManager* m_data_mgr;

  cRandom m_rng;
  
  bool m_test_on_div;     // flag derived from a collection of configuration settings
  bool m_test_sterilize;  // flag derived from a collection of configuration settings
  
  bool m_own_driver;      // specifies whether this world object should manage its driver object

  cWorld(cAvidaConfig* cfg, const cString& wd);
  
private:
  cWorld(); // @not_implemented
  cWorld(const cWorld&); // @not_implemented
  cWorld& operator=(const cWorld&); // @not_implemented  
  
  
public:
  static cWorld* Initialize(cAvidaConfig* cfg, const cString& working_dir, cUserFeedback* feedback = NULL); 
  virtual ~cWorld();
  
  void SetDriver(cWorldDriver* driver, bool take_ownership = false);
  
  const cString& GetWorkingDir() const { return m_working_dir; }
  
  // General Object Accessors
  cAnalyze& GetAnalyze();
  cAvidaConfig& GetConfig() { return *m_conf; }
  cAvidaContext& GetDefaultContext() { return m_ctx; }
  cClassificationManager& GetClassificationManager() { return *m_class_mgr; }
  cDataFileManager& GetDataFileManager() { return *m_datafile_mgr; }
  cEnvironment& GetEnvironment() { return *m_env; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cPopulation& GetPopulation() { return *m_pop; }
  cRandom& GetRandom() { return m_rng; }
  cStats& GetStats() { return *m_stats; }
  cWorldDriver& GetDriver() { return *m_driver; }
  
  Data::cManager& GetDataManager() { return *m_data_mgr; }
  
  Apto::SmartPtr<Data::cProvider, Apto::ThreadSafeRefCount> GetStatsProvider(cWorld*);
  
  // Access to Data File Manager
  std::ofstream& GetDataFileOFStream(const cString& fname) { return m_datafile_mgr->GetOFStream(fname); }
  cDataFile& GetDataFile(const cString& fname) { return m_datafile_mgr->Get(fname); }  

  // Config Dependent Modes
  bool GetTestOnDivide() const { return m_test_on_div; }
  bool GetTestSterilize() const { return m_test_sterilize; }
  
  // Convenience Accessors
  int GetNumResources();
  inline int GetVerbosity() { return m_conf->VERBOSITY.Get(); }
  inline void SetVerbosity(int v) { m_conf->VERBOSITY.Set(v); }

  void GetEvents(cAvidaContext& ctx);
	
	cEventList* GetEventsList() { return m_event_list; }

	//! Migrate this organism to a different world (does nothing here; see cMultiProcessWorld).
	virtual void MigrateOrganism(cOrganism* org, const cPopulationCell& cell, const cMerit& merit, int lineage) { }

	//! Returns true if an organism should be migrated to a different world.
	virtual bool TestForMigration() { return false; }
		
	//! Returns true if the given cell is on the boundary of the world, false otherwise.
	virtual bool IsWorldBoundary(const cPopulationCell& cell) { return false; }
	
	//! Process post-update events.
	virtual void ProcessPostUpdate(cAvidaContext& ctx) { }
	
	//! Returns true if this world allows early exits, e.g., when the population reaches 0.
	virtual bool AllowsEarlyExit() const { return true; }
	
	//! Calculate the size (in virtual CPU cycles) of the current update.
	virtual int CalculateUpdateSize();
  
protected:
  // Internal Methods
  bool setup(cUserFeedback* errors); 

};

#endif
