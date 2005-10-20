//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// This class is used by organisms to interface with their population.

#ifndef POPULATION_INTERFACE_HH
#define POPULATION_INTERFACE_HH

#include "cWorld.h"
#include "cAvidaDriver_Base.h"

class cHardwareBase;
class cPopulation;
class cOrganism;
class cGenome;
template <class T> class tArray;
class cOrgMessage;

class cPopulationInterface {
private:
  cWorld* m_world;
  int cell_id;

public:
  cPopulationInterface(cWorld* world) : m_world(world), cell_id(-1) { ; }
  virtual ~cPopulationInterface() { ; }

  int GetCellID() { return cell_id; }
  void SetCellID(int in_id) { cell_id = in_id; }

  bool InTestPop() { return (cell_id == -1); }

  // Activate callbacks...
  cHardwareBase* NewHardware(cOrganism * owner);
  bool Divide(cOrganism * parent, cGenome & child_genome);
  bool TestOnDivide() const { return m_world->GetTestOnDivide(); }
  cOrganism * GetNeighbor();
  int GetNumNeighbors();
  void Rotate(int direction=1);
  void Breakpoint() { cAvidaDriver_Base::main_driver->SignalBreakpoint(); }
  double TestFitness();
  int GetInput();
  int GetInputAt(int & input_pointer);
  int Debug();
  const tArray<double> & GetResources();
  void UpdateResources(const tArray<double> & res_change);
  void Die();
  void Kaboom();
  bool SendMessage(cOrgMessage & mess);
  int ReceiveValue();
  bool InjectParasite(cOrganism * parent, const cGenome & injected_code);
  bool UpdateMerit(double new_merit);
};

#endif
