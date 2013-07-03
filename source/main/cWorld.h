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
  cString m_working_dir;
  
  cAnalyze* m_analyze;
  cAvidaConfig* m_conf;
  cAvidaContext* m_ctx;
  
  cEnvironment* m_env;
  cEventList* m_event_list;
  cHardwareManager* m_hw_mgr;
  
  Apto::SmartPtr<cPopulation, Apto::InternalRCObject> m_pop;
  Apto::SmartPtr<cStats, Apto::InternalRCObject> m_stats;
  
  Data::ManagerPtr m_data_mgr;

  Apto::RNG::AvidaRNG m_rng;
  

  cWorld(cAvidaConfig* cfg, const cString& wd);
  
  
public:
  static cWorld* Initialize(cAvidaConfig* cfg, const cString& working_dir, Universe* new_world, cUserFeedback* feedback = NULL, const Apto::Map<Apto::String, Apto::String>* mappings = NULL);
  virtual ~cWorld();
  
  void SetDriver(UniverseDriver* driver, bool take_ownership = false);
  
  const cString& GetWorkingDir() const { return m_working_dir; }
  
  // General Object Accessors
  cAnalyze& GetAnalyze();
  cAvidaConfig& GetConfig() { return *m_conf; }
  cEnvironment& GetEnvironment() { return *m_env; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cPopulation& GetPopulation() { return *m_pop; }
  Apto::Random& GetRandom() { return m_rng; }
  cStats& GetStats() { return *m_stats; }
  
  Data::ManagerPtr& GetDataManager() { return m_data_mgr; }
  
  Data::ProviderPtr GetStatsProvider(Universe*);
  Data::ArgumentedProviderPtr GetPopulationProvider(Universe*);
  
  
  // Convenience Accessors
  inline int GetVerbosity() { return m_conf->VERBOSITY.Get(); }
  inline void SetVerbosity(int v) { m_conf->VERBOSITY.Set(v); }

  void GetEvents(cAvidaContext& ctx);
	
	cEventList* GetEventsList() { return m_event_list; }


  
protected:
  // Internal Methods
  bool setup(Universe* new_world, cUserFeedback* errors,  const Apto::Map<Apto::String, Apto::String>* mappings);

};

#endif
