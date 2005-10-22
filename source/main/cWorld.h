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

class cEnvironment;
class cEventManager;
class cEventList;
class cHardwareManager;
class cPopulation;
class cTestCPU;

class cWorld
{
protected:
  cAvidaConfig* m_conf;
  cEventManager* m_event_mgr;
  cEventList* m_event_list;
  cEnvironment* m_env;
  cHardwareManager* m_hw_mgr;
  cPopulation* m_pop;
  cTestCPU* m_test_cpu;

  cRandom m_rng;
  
  bool m_test_on_div;
  bool m_test_sterilize;

  void Setup();

public:
  explicit cWorld() : m_conf(new cAvidaConfig()) { Setup(); }
  cWorld(cAvidaConfig* cfg) : m_conf(cfg) { Setup(); }
  ~cWorld();
  
  void SetConfig(cAvidaConfig* cfg) { delete m_conf; m_conf = cfg; }
  
  cAvidaConfig& GetConfig() { return *m_conf; }
  cEnvironment& GetEnvironment() { return *m_env; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cPopulation& GetPopulation() { return *m_pop; }
  cRandom& GetRandom() { return m_rng; }
  cTestCPU& GetTestCPU() { return *m_test_cpu; }

  // Config Dependent Modes
  bool GetTestOnDivide() const { return m_test_on_div; }
  bool GetTestSterilize() const { return m_test_sterilize; }
  
  // Convenience Accessors
  int GetNumInstructions();
  int GetNumTasks();
  int GetNumReactions();
  int GetNumResources();

  // DDD - Inherited from cAvidaDriver heritage
  void GetEvents();
  void ReadEventListFile(const cString & filename);
  void SyncEventList();
};

#endif
