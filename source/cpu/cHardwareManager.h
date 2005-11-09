/*
 *  cHardwareManager.h
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef cHardwareManager_h
#define cHardwareManager_h

#ifndef cInstSet_h
#include "cInstSet.h";
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
  
public:
  cHardwareManager(cWorld* world);
  ~cHardwareManager() { ; }
  
  cHardwareBase* Create(cOrganism* in_org);

  void LoadInstSet(cString filename);
  
  const cInstSet& GetInstSet() const { return m_inst_set; }
  cInstSet& GetInstSet() { return m_inst_set; }
};

#endif
