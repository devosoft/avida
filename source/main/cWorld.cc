/*
 *  cWorld.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cWorld.h"

#include "avida.h"
#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "cClassificationManager.h"
#include "cEnvironment.h"
#include "cEventList.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cPopulation.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cTools.h"
#include "cFallbackWorldDriver.h"

#include <cassert>


cWorld::~cWorld()
{
  // m_actlib is not owned by cWorld, DO NOT DELETE
  
  // These must be deleted first
  delete m_analyze; m_analyze = NULL;
  delete m_pop; m_pop = NULL;
  
  delete m_class_mgr; m_class_mgr = NULL;
  delete m_env; m_env = NULL;
  delete m_event_list; m_event_list = NULL;
  delete m_hw_mgr; m_hw_mgr = NULL;
  delete m_stats; m_stats = NULL;

  // Delete after all classes that may be logging items
  if (m_data_mgr) { m_data_mgr->FlushAll(); }
  delete m_data_mgr; m_data_mgr = NULL;
  
  // Delete Last
  delete m_conf; m_conf = NULL;

  // cleanup driver object, if needed
  if (m_own_driver) { delete m_driver; m_driver = NULL; }
}


void cWorld::Setup()
{
  m_own_driver = true;
  m_driver = new cFallbackWorldDriver();
  
  // Setup Random Number Generator
  const int rand_seed = m_conf->RANDOM_SEED.Get();
  cout << "Random Seed: " << rand_seed;
  m_rng.ResetSeed(rand_seed);
  if (rand_seed != m_rng.GetSeed()) cout << " -> " << m_rng.GetSeed();
  cout << endl;
  
  m_data_mgr = new cDataFileManager(m_conf->DATA_DIR.Get(), (m_conf->VERBOSITY.Get() > VERBOSE_ON));
  if (m_conf->VERBOSITY.Get() > VERBOSE_NORMAL)
    cout << "Data Directory: " << m_data_mgr->GetTargetDir() << endl;
  
  m_class_mgr = new cClassificationManager(this);
  m_env = new cEnvironment(this);
  m_hw_mgr = new cHardwareManager(this);
  
  // Initialize the default environment...
  // This must be after the HardwareManager in case REACTIONS that trigger instructions are used.
  if (!m_env->Load(m_conf->ENVIRONMENT_FILE.Get())) {
    cerr << "error: unable to load environment" << endl;
    Avida::Exit(-1);
  }
  
  // Setup Stats Object
  m_stats = new cStats(this);
    
  const cInstSet& inst_set = m_hw_mgr->GetInstSet();
  for (int i = 0; i < inst_set.GetSize(); i++)
    m_stats->SetInstName(i, inst_set.GetName(i));
  
  // @MRR CClade Tracking
//	if (m_conf->TRACK_CCLADES.Get() > 0)
//		m_class_mgr->LoadCCladeFounders(m_conf->TRACK_CCLADES_IDS.Get());
  
	m_pop = new cPopulation(this);
  m_pop->InitiatePop();
  
  // Setup Event List
  m_event_list = new cEventList(this);
  if (!m_event_list->LoadEventFile(m_conf->EVENT_FILE.Get())) {
    cerr << "error: unable to load events" << endl;
    Avida::Exit(-1);
  }
	
  
  const bool revert_fatal = m_conf->REVERT_FATAL.Get() > 0.0;
  const bool revert_neg = m_conf->REVERT_DETRIMENTAL.Get() > 0.0;
  const bool revert_neut = m_conf->REVERT_NEUTRAL.Get() > 0.0;
  const bool revert_pos = m_conf->REVERT_BENEFICIAL.Get() > 0.0;
  const bool revert_taskloss = m_conf->REVERT_TASKLOSS.Get() > 0.0;
  const bool fail_implicit = m_conf->FAIL_IMPLICIT.Get() > 0;
  m_test_on_div = (revert_fatal || revert_neg || revert_neut || revert_pos || revert_taskloss || fail_implicit);
  
  const bool sterilize_fatal = m_conf->STERILIZE_FATAL.Get() > 0.0;
  const bool sterilize_neg = m_conf->STERILIZE_DETRIMENTAL.Get() > 0.0;
  const bool sterilize_neut = m_conf->STERILIZE_NEUTRAL.Get() > 0.0;
  const bool sterilize_pos = m_conf->STERILIZE_BENEFICIAL.Get() > 0.0;
  const bool sterilize_taskloss = m_conf->STERILIZE_TASKLOSS.Get() > 0.0;
  m_test_sterilize = (sterilize_fatal || sterilize_neg || sterilize_neut || sterilize_pos || sterilize_taskloss);
}

cAnalyze& cWorld::GetAnalyze()
{
  if (m_analyze == NULL) m_analyze = new cAnalyze(this);
  return *m_analyze;
}

void cWorld::GetEvents(cAvidaContext& ctx)
{  
  if (m_pop->GetSyncEvents() == true) {
    m_event_list->Sync();
    m_pop->SetSyncEvents(false);
  }
  m_event_list->Process(ctx);
}

int cWorld::GetNumInstructions()
{
  return m_hw_mgr->GetInstSet().GetSize();
}

int cWorld::GetNumResources()
{
  return m_env->GetResourceLib().GetSize();
}

// Given number of resources and number of nops, how many possible collect-type resource specifications exist?
// If no nops or no resources, return 0
int cWorld::GetNumResourceSpecs()
{
  int num_resources = GetEnvironment().GetResourceLib().GetSize();
  int num_nops = GetHardwareManager().GetInstSet().GetNumNops();
  
  if (num_resources <= 0 || num_nops <= 0) { return 0; }
  
  double most_nops_needed = ceil(log((double)num_resources)/log((double)num_nops));
  double numerator = pow((double)num_nops, most_nops_needed + 1) - 1;
  double denominator = (double)(num_nops - 1);
  return (int)(numerator / denominator);
}

void cWorld::SetDriver(cWorldDriver* driver, bool take_ownership)
{
  // cleanup current driver, if needed
  if (m_own_driver) delete m_driver;
  
  // store new driver information
  m_driver = driver;
  m_own_driver = take_ownership;
}
