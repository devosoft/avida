/*
 *  cHardwareManager.h
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cHardwareManager_h
#define cHardwareManager_h

#ifndef cInstSet_h
#include "cInstSet.h"
#endif
#ifndef cTestCPU_h
#include "cTestCPU.h"
#endif

class cOrganism;
class cHardwareBase;
class cWorld;

class cHardwareManager
{
private:
  cWorld* m_world;
  cInstSet m_inst_set;
  int m_type;
  cTestResources m_testres;
  
  cHardwareManager(); // @not_implemented
  cHardwareManager(const cHardwareManager&); // @not_implemented
  cHardwareManager& operator=(const cHardwareManager&); // @not_implemented
  
public:
  cHardwareManager(cWorld* world);
  ~cHardwareManager() { ; }
  
  cHardwareBase* Create(cOrganism* in_org);
  cTestCPU* CreateTestCPU() { return new cTestCPU(m_world, &m_testres); }

  void LoadInstSet(cString filename);
  
  const cInstSet& GetInstSet() const { return m_inst_set; }
  cInstSet& GetInstSet() { return m_inst_set; }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
