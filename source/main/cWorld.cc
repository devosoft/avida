/*
 *  cWorld.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cWorld.h"

#include "avida.h"
#include "cAnalyze.h"
#include "cAvidaTriggers.h"
#include "cClassificationManager.h"
#include "cEnvironment.h"
#include "cEventList.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cPopulation.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cTools.h"
#include "cFallbackWorldDriver.h"


cWorld::~cWorld()
{
  m_data_mgr->FlushAll();

  // m_actlib is not owned by cWorld, DO NOT DELETE
  delete m_analyze;
  delete m_conf;
  delete m_data_mgr;
  delete m_env;
  delete m_event_list;
  delete m_hw_mgr;
  delete m_pop;
  delete m_stats;

  // cleanup driver object, if needed
  if (m_own_driver) delete m_driver;
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
  
  m_actlib = cDriverManager::GetActionLibrary();
  
  m_data_mgr = new cDataFileManager(m_conf->DATA_DIR.Get(), (m_conf->VERBOSITY.Get() > VERBOSE_ON));
  if (m_conf->VERBOSITY.Get() > VERBOSE_NORMAL)
    cout << "Data Directory: " << m_data_mgr->GetTargetDir() << endl;
  
  m_class_mgr = new cClassificationManager(this);
  m_env = new cEnvironment(this);
  
  // Initialize the default environment...
  if (m_env->Load(m_conf->ENVIRONMENT_FILE.Get()) == false) {
    cerr << "Unable to load environment... aborting!" << endl;
    ExitAvida(-1);
  }

  m_hw_mgr = new cHardwareManager(this);
  m_stats = new cStats(this);
  m_pop = new cPopulation(this);

  // Setup Event List
  m_event_list = new cEventList(this, new cAvidaTriggers(*m_stats));
  
  ReadEventListFile(m_conf->EVENT_FILE.Get());
    
  const bool revert_fatal = m_conf->REVERT_FATAL.Get() > 0.0;
  const bool revert_neg = m_conf->REVERT_DETRIMENTAL.Get() > 0.0;
  const bool revert_neut = m_conf->REVERT_NEUTRAL.Get() > 0.0;
  const bool revert_pos = m_conf->REVERT_BENEFICIAL.Get() > 0.0;
  const bool fail_implicit = m_conf->FAIL_IMPLICIT.Get() > 0;
  m_test_on_div = (revert_fatal || revert_neg || revert_neut || revert_pos || fail_implicit);

  const bool sterilize_fatal = m_conf->STERILIZE_FATAL.Get() > 0.0;
  const bool sterilize_neg = m_conf->STERILIZE_DETRIMENTAL.Get() > 0.0;
  const bool sterilize_neut = m_conf->STERILIZE_NEUTRAL.Get() > 0.0;
  const bool sterilize_pos = m_conf->STERILIZE_BENEFICIAL.Get() > 0.0;
  m_test_sterilize = (sterilize_fatal || sterilize_neg || sterilize_neut || sterilize_pos);
}

cAnalyze& cWorld::GetAnalyze()
{
  if (m_analyze == NULL) m_analyze = new cAnalyze(this);
  return *m_analyze;
}

void cWorld::ReadEventListFile(const cString & filename)
{
  cInitFile event_file(filename);
  
  // Load in the proper event list and set it up.
  event_file.Load();
  event_file.Compress();
  
  // Loop through the line_list and change the lines to events.
  for (int line_id = 0; line_id < event_file.GetNumLines(); line_id++) {
    m_event_list->AddEventFileFormat(event_file.GetLine(line_id));
  }
  
  ////////// Adding events from the Genesis file //////////
  // Max Updates and/or Max Generations
  if(m_conf->END_CONDITION_MODE.Get() == 0 ){
    if(m_conf->MAX_UPDATES.Get() >= 0 ){
      m_event_list->AddEvent( cEventTriggers::UPDATE, m_conf->MAX_UPDATES.Get(),
                            cEventTriggers::TRIGGER_ONCE,
                            cEventTriggers::TRIGGER_END, "exit", "");
    }
    if(m_conf->MAX_GENERATIONS.Get() >= 0 ){
      m_event_list->AddEvent( cEventTriggers::GENERATION,
                            m_conf->MAX_GENERATIONS.Get(),
                            cEventTriggers::TRIGGER_ONCE,
                            cEventTriggers::TRIGGER_END, "exit", "");
    }
  }else{
    if(m_conf->MAX_UPDATES.Get() >= 0 && m_conf->MAX_GENERATIONS.Get() >= 0 ){
      cString max_gen_str;
      max_gen_str.Set("%d", m_conf->MAX_GENERATIONS.Get());
      cString max_update_str;
      max_update_str.Set("%d", m_conf->MAX_UPDATES.Get());
      m_event_list->AddEvent( cEventTriggers::UPDATE, m_conf->MAX_UPDATES.Get(),
                            1, cEventTriggers::TRIGGER_END,
                            "exit_if_generation_greater_than", max_gen_str);
      m_event_list->AddEvent( cEventTriggers::GENERATION,
                            m_conf->MAX_GENERATIONS.Get(),
                            .01, cEventTriggers::TRIGGER_END,
                            "exit_if_update_greater_than", max_update_str);
    }
  }
}

void cWorld::SyncEventList() { m_event_list->Sync(); }

void cWorld::GetEvents()
{  
  if (m_pop->GetSyncEvents() == true) {
    SyncEventList();
    m_pop->SetSyncEvents(false);
  }
  m_event_list->Process();
}

int cWorld::GetNumInstructions()
{
  return m_hw_mgr->GetInstSet().GetSize();
}

int cWorld::GetNumTasks()
{
  return m_env->GetTaskLib().GetSize(); 
}

int cWorld::GetNumReactions()
{
  return m_env->GetReactionLib().GetSize();
}

int cWorld::GetNumResources()
{
  return m_env->GetResourceLib().GetSize();
}

void cWorld::SetDriver(cWorldDriver* driver, bool take_ownership)
{
  // cleanup current driver, if needed
  if (m_own_driver) delete m_driver;
  
  // store new driver information
  m_driver = driver;
  m_own_driver = take_ownership;
}
