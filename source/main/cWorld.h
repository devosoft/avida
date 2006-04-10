/*
 *  cWorld.h
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cWorld_h
#define cWorld_h

#ifndef cAvidaConfig_h
#include "cAvidaConfig.h"
#endif
#ifndef cAvidaContext_h
#include "cAvidaContext.h"
#endif
#ifndef cDataFileManager_h
#include "cDataFileManager.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif

class cActionLibrary;
class cAnalyze;
class cAvidaDriver;
class cClassificationManager;
class cEnvironment;
class cEventManager;
class cEventList;
class cHardwareManager;
class cPopulation;
class cStats;
class cTestCPU;
class cWorldDriver;

class cWorld
{
protected:
  cActionLibrary* m_actlib;
  cAnalyze* m_analyze;
  cAvidaConfig* m_conf;
  cAvidaContext m_ctx;
  cClassificationManager* m_class_mgr;
  cDataFileManager* m_data_mgr;
  cEnvironment* m_env;
  cEventManager* m_event_mgr;
  cEventList* m_event_list;
  cHardwareManager* m_hw_mgr;
  cPopulation* m_pop;
  cStats* m_stats;
  cWorldDriver* m_driver;

  cRandom m_rng;
  
  bool m_test_on_div;     // flag derived from a collection of configuration settings
  bool m_test_sterilize;  // flag derived from a collection of configuration settings
  
  bool m_own_driver;      // specifies whether this world object should manage its driver object

  // Internal Methods
  void Setup();
  
  cWorld(const cWorld&); // @not_implemented
  cWorld& operator=(const cWorld&); // @not_implemented
  
public:
  explicit cWorld() : m_analyze(NULL), m_conf(new cAvidaConfig()), m_ctx(m_rng) { Setup(); }
  cWorld(cAvidaConfig* cfg) : m_analyze(NULL), m_conf(cfg), m_ctx(m_rng) { Setup(); }
  ~cWorld();
  
  void SetConfig(cAvidaConfig* cfg) { delete m_conf; m_conf = cfg; }
  void SetDriver(cWorldDriver* driver, bool take_ownership = false);
  
  // General Object Accessors
  cActionLibrary& GetActionLibrary() { return *m_actlib; }
  cAnalyze& GetAnalyze();
  cAvidaConfig& GetConfig() { return *m_conf; }
  cAvidaContext& GetDefaultContext() { return m_ctx; }
  cClassificationManager& GetClassificationManager() { return *m_class_mgr; }
  cDataFileManager& GetDataFileManager() { return *m_data_mgr; }
  cEnvironment& GetEnvironment() { return *m_env; }
  cHardwareManager& GetHardwareManager() { return *m_hw_mgr; }
  cPopulation& GetPopulation() { return *m_pop; }
  cRandom& GetRandom() { return m_rng; }
  cStats& GetStats() { return *m_stats; }
  cWorldDriver& GetDriver() { return *m_driver; }
  
  // Access to Data File Manager
  std::ofstream& GetDataFileOFStream(const cString& fname) { return m_data_mgr->GetOFStream(fname); }
  cDataFile& GetDataFile(const cString& fname) { return m_data_mgr->Get(fname); }  

  // Config Dependent Modes
  bool GetTestOnDivide() const { return m_test_on_div; }
  bool GetTestSterilize() const { return m_test_sterilize; }
  
  // Convenience Accessors
  int GetNumInstructions();
  int GetNumTasks();
  int GetNumReactions();
  int GetNumResources();

  // @DMB - Inherited from cAvidaDriver heritage
  void GetEvents();
  void ReadEventListFile(const cString & filename);
  void SyncEventList();
};

#endif
