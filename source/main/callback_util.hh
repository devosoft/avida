//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CALLBACK_UTIL_HH
#define CALLBACK_UTIL_HH

class cGenome;
class cHardwareBase;
class cOrganism;
class cPopulation;
class cPopulationInterface;
class cOrgMessage;
class cCodeLabel;
template <class T> class tArray;

class cCallbackUtil {
public:
  // Callbacks from organism...
  static cHardwareBase * CB_NewHardware(cPopulation * pop, cOrganism * owner);
  static void CB_RecycleHardware(cHardwareBase * out_hardware);
  static bool CB_Divide(cPopulation * pop, int cell_id,
			cOrganism * parent, cGenome & child_genome);
  static bool CB_TestDivide(cPopulation * pop, int cell_id,
			    cOrganism * parent, cGenome & child_genome);
  static bool CB_TestOnDivide(cPopulation * pop, int cell_id);
  static cOrganism * CB_GetNeighbor(cPopulation * pop, int cell_id);
  static int CB_GetNumNeighbors(cPopulation * pop, int cell_id);
  static void CB_Rotate(cPopulation * pop, int cell_id, int direction);
  static void CB_Breakpoint();
  static double CB_TestFitness(cPopulation * pop, int cell_id);
  static int CB_GetInput(cPopulation * pop, int cell_id);
  static int CB_GetInputAt(cPopulation * pop, int cell_id, int & input_pointer);
  static int CB_Debug(cPopulation * pop, int cell_id);
  static const tArray<double>& CB_GetResources(cPopulation * pop, int cell_id);
  static void CB_UpdateResources(cPopulation  * pop, int cell_id,
				 const tArray<double> & res_change);
  static void CB_KillCell(cPopulation * pop, int death_id);
  static void CB_KillSurroundCell(cPopulation * pop, int commie_id);
  static bool CB_SendMessage(cPopulation * pop, int cell_id, cOrgMessage & mess);
  static int CB_ReceiveValue(cPopulation * pop, int cell_id);
  static bool CB_InjectParasite(cPopulation * pop, int cell_id, cOrganism * parent,
				const cGenome & injected_code);

  static bool CB_UpdateMerit(cPopulation * pop, int cell_id, double new_merit);
};

#endif
