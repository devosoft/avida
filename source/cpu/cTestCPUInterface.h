/*
 *  cTestCPUOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cTestCPUInterface_h
#define cTestCPUInterface_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif

class cOrganism;
class cGenome;
template <class T> class tArray;
class cOrgMessage;
class cTestCPU;

class cTestCPUInterface : public cOrgInterface
{
private:
  cTestCPU* m_testcpu;

public:
  cTestCPUInterface(cTestCPU* testcpu) : m_testcpu(testcpu) { ; }
  virtual ~cTestCPUInterface() { ; }

  int GetCellID() { return -1; }
  void SetCellID(int in_id) { ; }

  bool Divide(cOrganism* parent, cGenome& child_genome);
  cOrganism* GetNeighbor();
  int GetNumNeighbors();
  void Rotate(int direction = 1);
  void Breakpoint() { ; }
  double TestFitness();
  int GetInput();
  int GetInputAt(int& input_pointer);
  int Debug();
  const tArray<double>& GetResources();
  void UpdateResources(const tArray<double>& res_change);
  void Die();
  void Kaboom();
  bool SendMessage(cOrgMessage& mess);
  int ReceiveValue();
  bool InjectParasite(cOrganism* parent, const cGenome& injected_code);
  bool UpdateMerit(double new_merit);
};

#endif
