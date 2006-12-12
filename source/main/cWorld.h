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

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


#include <assert.h>

class cActionLibrary;
class cAnalyze;
class cAvidaDriver;
class cClassificationManager;
class cEnvironment;
class cEventList;
class cHardwareManager;
class cPopulation;
class cStats;
class cTestCPU;
class cWorldDriver;

class cWorld
{
#if USE_tMemTrack
  tMemTrack<cWorld> mt;
#endif
protected:
  cActionLibrary* m_actlib;
  cAnalyze* m_analyze;
  cAvidaConfig* m_conf;
  cAvidaContext m_ctx;
  cClassificationManager* m_class_mgr;
  cDataFileManager* m_data_mgr;
  cEnvironment* m_env;
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
  explicit cWorld()
  : m_actlib(NULL)
  , m_analyze(NULL)
  , m_conf(new cAvidaConfig())
  , m_ctx(m_rng)
  , m_class_mgr(NULL)
  , m_data_mgr(NULL)
  , m_env(NULL)
  , m_event_list(NULL)
  , m_hw_mgr(NULL)
  , m_pop(NULL)
  , m_stats(NULL)
  , m_driver(NULL)
  { Setup(); }
  cWorld(cAvidaConfig* cfg) : m_analyze(NULL), m_conf(cfg), m_ctx(m_rng) { Setup(); }
  ~cWorld();
  
  void SetConfig(cAvidaConfig* cfg) { assert(m_conf); delete m_conf; m_conf = cfg; }
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
  inline int GetVerbosity() { return m_conf->VERBOSITY.Get(); }
  inline void SetVerbosity(int v) { m_conf->VERBOSITY.Set(v); }

  // @DMB - Inherited from cAvidaDriver heritage
  void GetEvents(cAvidaContext& ctx);

  // Save to archive 
  template<class Archive>
  void save(Archive & a, const unsigned int version) const {
    a.ArkvObj("m_actlib", m_actlib);
    a.ArkvObj("m_analyze", m_analyze);
    a.ArkvObj("m_conf", m_conf);
    a.ArkvObj("m_ctx", m_ctx);
    a.ArkvObj("m_class_mgr", m_class_mgr);
    a.ArkvObj("m_data_mgr", m_data_mgr);
    a.ArkvObj("m_env", m_env);
    a.ArkvObj("m_event_list", m_event_list);
    a.ArkvObj("m_hw_mgr", m_hw_mgr);
    a.ArkvObj("m_pop", m_pop);
    a.ArkvObj("m_stats", m_stats);
    a.ArkvObj("m_driver", m_driver);
    a.ArkvObj("m_rng", m_rng);
    int __m_test_on_div = (m_test_on_div == false)?(0):(1);
    int __m_test_sterilize = (m_test_sterilize == false)?(0):(1);
    int __m_own_driver = (m_own_driver == false)?(0):(1);
    a.ArkvObj("m_test_on_div", __m_test_on_div);
    a.ArkvObj("m_test_sterilize", __m_test_sterilize);
    a.ArkvObj("m_own_driver", __m_own_driver);
  }
  
  // Load from archive 
  template<class Archive>
  void load(Archive & a, const unsigned int version){
    a.ArkvObj("m_actlib", m_actlib);
    a.ArkvObj("m_analyze", m_analyze);
    a.ArkvObj("m_conf", m_conf);
    a.ArkvObj("m_ctx", m_ctx);
    a.ArkvObj("m_class_mgr", m_class_mgr);
    a.ArkvObj("m_data_mgr", m_data_mgr);
    a.ArkvObj("m_env", m_env);
    a.ArkvObj("m_event_list", m_event_list);
    a.ArkvObj("m_hw_mgr", m_hw_mgr);
    a.ArkvObj("m_pop", m_pop);
    a.ArkvObj("m_stats", m_stats);
    a.ArkvObj("m_driver", m_driver);
    a.ArkvObj("m_rng", m_rng);
    int __m_test_on_div;
    int __m_test_sterilize;
    int __m_own_driver;
    a.ArkvObj("m_test_on_div", __m_test_on_div);
    a.ArkvObj("m_test_sterilize", __m_test_sterilize);
    a.ArkvObj("m_own_driver", __m_own_driver);
    m_test_on_div = (__m_test_on_div == 0)?(false):(true);
    m_test_sterilize = (__m_test_sterilize == 0)?(false):(true);
    m_own_driver = (__m_own_driver == 0)?(false):(true);
  } 
    
  // Ask archive to handle loads and saves separately
  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.SplitLoadSave(*this, version);
  }

};


#ifdef ENABLE_UNIT_TESTS
namespace nWorld {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
