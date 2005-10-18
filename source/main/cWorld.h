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

class cWorld
{
protected:
  cAvidaConfig* m_config;
  cRandom m_rng;

  bool m_test_on_divide;
  bool m_test_sterilize;
  
  void Setup();

public:
  explicit cWorld() : m_config(new cAvidaConfig()) { Setup(); }
  cWorld(cAvidaConfig* cfg) : m_config(cfg) { Setup(); }
  ~cWorld() { delete m_config; }
  
  void SetConfig(cAvidaConfig* cfg) { delete m_config; m_config = cfg; }
  cAvidaConfig& GetConfig() { return *m_config; }
  
  // Config Dependent Modes
  bool GetTestOnDivide() const
  {
    const bool revert_fatal = m_config->REVERT_FATAL.Get() > 0.0;
    const bool revert_neg = m_config->REVERT_DETRIMENTAL.Get() > 0.0;
    const bool revert_neut = m_config->REVERT_NEUTRAL.Get() > 0.0;
    const bool revert_pos = m_config->REVERT_BENEFICIAL.Get() > 0.0;
    const bool fail_implicit = m_config->FAIL_IMPLICIT.Get() > 0;
    return (revert_fatal || revert_neg || revert_neut || revert_pos || fail_implicit);
  }
  bool GetTestSterilize() const
  {
    const bool sterilize_fatal = m_config->STERILIZE_FATAL.Get() > 0.0;
    const bool sterilize_neg = m_config->STERILIZE_DETRIMENTAL.Get() > 0.0;
    const bool sterilize_neut = m_config->STERILIZE_NEUTRAL.Get() > 0.0;
    const bool sterilize_pos = m_config->STERILIZE_BENEFICIAL.Get() > 0.0;
    return (sterilize_fatal || sterilize_neg || sterilize_neut || sterilize_pos);
  }
  
  cRandom& GetRandom() { return m_rng; }
};

#endif
