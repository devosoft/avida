/*
 *  cPopulationInterface.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cPopulationInterface_h
#define cPopulationInterface_h

#ifndef cOrgInterface_h
#include "cOrgInterface.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cPopulation;


class cPopulationInterface : public cOrgInterface
{
private:
  cWorld* m_world;
  int m_cell_id;

  cPopulationInterface(); // @not_implemented
  cPopulationInterface(const cPopulationInterface&); // @not_implemented
  cPopulationInterface operator=(const cPopulationInterface&); // @not_implemented
  
public:
  cPopulationInterface(cWorld* world) : m_world(world), m_cell_id(-1) { ; }
  virtual ~cPopulationInterface() { ; }

  int GetCellID() { return m_cell_id; }
  void SetCellID(int in_id) { m_cell_id = in_id; }

  bool Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome);
  cOrganism* GetNeighbor();
  int GetNumNeighbors();
  void Rotate(int direction = 1);
  void Breakpoint() { m_world->GetDriver().SignalBreakpoint(); }
  double TestFitness();
  int GetInput();
  int GetInputAt(int& input_pointer);
  int Debug();
  const tArray<double>& GetResources();
  void UpdateResources(const tArray<double>& res_change);
  void Die();
  void Kaboom();
  bool SendMessage(cOrgMessage& mess);
  cOrgSinkMessage* NetReceive();
  bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg);
  int ReceiveValue();
  bool InjectParasite(cOrganism* parent, const cGenome& injected_code);
  bool UpdateMerit(double new_merit);
};


#ifdef ENABLE_UNIT_TESTS
namespace nPopulationInterface {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
