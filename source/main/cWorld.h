/*
 *  cWorld.h
 *  Avida2
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef cWorld_h
#define cWorld_h

#ifndef cAvidaConfig_h
#include "cAvidaConfig.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif
#include "cEventList.h"

class cEnvironment;
class cEventManager;
class cHardwareManager;
class cPopulation;

class cWorld
{
protected:
  cAvidaConfig* m_conf;
  cEventManager* m_event_mgr;
  cEventList* m_event_list;
  cEnvironment* m_env;
  cHardwareManager* m_hw_mgr;
  cPopulation* m_pop;

  cRandom m_rng;

  void Setup();

public:
  explicit cWorld() : m_conf(new cAvidaConfig()) { Setup(); }
  cWorld(cAvidaConfig* cfg) : m_conf(cfg) { Setup(); }
  ~cWorld();
  
  void SetConfig(cAvidaConfig* cfg) { delete m_conf; m_conf = cfg; }
  cAvidaConfig& GetConfig() { return *m_conf; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cPopulation& GetPopulation() { return *m_pop; }
  cEnvironment& GetEnvironment() { return *m_env; }

  // Config Dependent Modes
  bool GetTestOnDivide() const
  {
    const bool revert_fatal = m_conf->REVERT_FATAL.Get() > 0.0;
    const bool revert_neg = m_conf->REVERT_DETRIMENTAL.Get() > 0.0;
    const bool revert_neut = m_conf->REVERT_NEUTRAL.Get() > 0.0;
    const bool revert_pos = m_conf->REVERT_BENEFICIAL.Get() > 0.0;
    const bool fail_implicit = m_conf->FAIL_IMPLICIT.Get() > 0;
    return (revert_fatal || revert_neg || revert_neut || revert_pos || fail_implicit);
  }
  bool GetTestSterilize() const
  {
    const bool sterilize_fatal = m_conf->STERILIZE_FATAL.Get() > 0.0;
    const bool sterilize_neg = m_conf->STERILIZE_DETRIMENTAL.Get() > 0.0;
    const bool sterilize_neut = m_conf->STERILIZE_NEUTRAL.Get() > 0.0;
    const bool sterilize_pos = m_conf->STERILIZE_BENEFICIAL.Get() > 0.0;
    return (sterilize_fatal || sterilize_neg || sterilize_neut || sterilize_pos);
  }
  
  cRandom& GetRandom() { return m_rng; }

  void GetEvents();
  void ReadEventListFile(const cString & filename);
  void SyncEventList() { m_event_list->Sync(); }

  int GetNumInstructions();
  int GetNumTasks();
  int GetNumReactions();
  int GetNumResources();
};

#endif
