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

#include "avida/core/Types.h"
#include "avida/data/Types.h"

#include "apto/rng.h"

#include "cAvidaConfig.h"
#include "cAvidaContext.h"

#include <cassert>

class cAnalyze;
class cAnalyzeGenotype;
class cEnvironment;
class cEventList;
class cHardwareManager;
class cMigrationMatrix; 
class cOrganism;
class cPopulation;
class cMerit;
class cPopulationCell;
class cStats;
class cTestCPU;
class cUserFeedback;
template<class T> class tDataEntry;

using namespace Avida;


class cWorld
{
protected:
  World* m_new_world;
  cString m_working_dir;
  
  cAnalyze* m_analyze;
  cAvidaConfig* m_conf;
  cAvidaContext* m_ctx;
  cEnvironment* m_env;
  cEventList* m_event_list;
  cHardwareManager* m_hw_mgr;
  Apto::SmartPtr<cPopulation, Apto::InternalRCObject> m_pop;
  Apto::SmartPtr<cStats, Apto::InternalRCObject> m_stats;
  cMigrationMatrix* m_mig_mat;  
  WorldDriver* m_driver;
  
  Data::ManagerPtr m_data_mgr;

  Apto::RNG::AvidaRNG m_rng;
  
  bool m_test_on_div;     // flag derived from a collection of configuration settings
  bool m_test_sterilize;  // flag derived from a collection of configuration settings
  
  bool m_own_driver;      // specifies whether this world object should manage its driver object

  cWorld(cAvidaConfig* cfg, const cString& wd);
  
  
public:
  static cWorld* Initialize(cAvidaConfig* cfg, const cString& working_dir, World* new_world, cUserFeedback* feedback = NULL, const Apto::Map<Apto::String, Apto::String>* mappings = NULL);
  virtual ~cWorld();
  
  void SetDriver(WorldDriver* driver, bool take_ownership = false);
  
  const cString& GetWorkingDir() const { return m_working_dir; }
  
  // General Object Accessors
  cAnalyze& GetAnalyze();
  cAvidaConfig& GetConfig() { return *m_conf; }
  cAvidaContext& GetDefaultContext() { return *m_ctx; }
  cEnvironment& GetEnvironment() { return *m_env; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cMigrationMatrix& GetMigrationMatrix(){ return *m_mig_mat; };
  cPopulation& GetPopulation() { return *m_pop; }
  Apto::Random& GetRandom() { return m_rng; }
  cStats& GetStats() { return *m_stats; }
  WorldDriver& GetDriver() { return *m_driver; }
  World* GetNewWorld() { return m_new_world; }
  
  Data::ManagerPtr& GetDataManager() { return m_data_mgr; }
  
  Data::ProviderPtr GetStatsProvider(World*);
  Data::ArgumentedProviderPtr GetPopulationProvider(World*);
  
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
	virtual void MigrateOrganism(cOrganism* org, const cPopulationCell& cell, const cMerit& merit, int lineage);

	//! Returns true if an organism should be migrated to a different world.
	virtual bool TestForMigration() { return false; }
		
	//! Returns true if the given cell is on the boundary of the world, false otherwise.
	virtual bool IsWorldBoundary(const cPopulationCell& cell);
	
	//! Process post-update events.
	virtual void ProcessPostUpdate(cAvidaContext&) { }
	
	//! Returns true if this world allows early exits, e.g., when the population reaches 0.
	virtual bool AllowsEarlyExit() const { return true; }
	
	//! Calculate the size (in virtual CPU cycles) of the current update.
	virtual int CalculateUpdateSize();
  
protected:
  // Internal Methods
  bool setup(World* new_world, cUserFeedback* errors,  const Apto::Map<Apto::String, Apto::String>* mappings);

};

#endif
