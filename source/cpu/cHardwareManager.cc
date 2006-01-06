/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cHardwareManager.h"

#include "cHardwareCPU.h"
#include "cHardware4Stack.h"
#include "cHardwareSMT.h"
#include "cInitFile.h"
#include "cInstLibCPU.h"
#include "cInstSet.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tDictionary.h"

cHardwareManager::cHardwareManager(cWorld* world) : m_world(world), m_inst_set(world), m_type(world->GetConfig().HARDWARE_TYPE.Get())
{
  LoadInstSet(world->GetConfig().INST_SET.Get());
}

void cHardwareManager::LoadInstSet(cString filename)
{
  // Setup the instruction library and collect the default filename
  cString default_filename;
	switch (m_type)
	{
		case HARDWARE_TYPE_CPU_ORIGINAL:
      m_inst_set.SetInstLib(cHardwareCPU::GetInstLib());
			default_filename = cHardwareCPU::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_4STACK:
      m_inst_set.SetInstLib(cHardware4Stack::GetInstLib());
			default_filename = cHardware4Stack::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_SMT:
      m_inst_set.SetInstLib(cHardwareSMT::GetInstLib());
			default_filename = cHardwareSMT::GetDefaultInstFilename();
			break;		
		default:
			default_filename = "unknown";
  }
  
  if (filename == "") {
    filename = default_filename;
    m_world->GetDriver().NotifyWarning(cString("No instruction set specified, using default '") + filename + "'.");
  }
  
  cInitFile file(filename);
  
  if (file.IsOpen() == false) {
    m_world->GetDriver().RaiseFatalException(1, cString("Could not open instruction set '") + filename + "'.");
  }
  
  file.Load();
  file.Compress();
  
  tDictionary<int> nop_dict;
  for(int i = 0; i < m_inst_set.GetInstLib()->GetNumNops(); i++)
    nop_dict.Add(m_inst_set.GetInstLib()->GetNopName(i), i);
  
  tDictionary<int> inst_dict;
  for(int i = 0; i < m_inst_set.GetInstLib()->GetSize(); i++)
    inst_dict.Add(m_inst_set.GetInstLib()->GetName(i), i);
  
  for (int line_id = 0; line_id < file.GetNumLines(); line_id++) {
    cString cur_line = file.GetLine(line_id);
    cString inst_name = cur_line.PopWord();
    int redundancy = cur_line.PopWord().AsInt();
    int cost = cur_line.PopWord().AsInt();
    int ft_cost = cur_line.PopWord().AsInt();
    double prob_fail = cur_line.PopWord().AsDouble();
    
    // If this instruction has 0 redundancy, we don't want it!
    if (redundancy < 0) continue;
    if (redundancy > 256) {
      cString msg("Max redundancy is 256.  Resetting redundancy of \"");
      msg += inst_name; msg += "\" from "; msg += redundancy; msg += " to 256.";
      m_world->GetDriver().NotifyWarning(msg);
      redundancy = 256;
    }
    
    // Otherwise, this instruction will be in the set.
    // First, determine if it is a nop...
    int nop_mod = -1;
    if(nop_dict.Find(inst_name, nop_mod) == true) {
      m_inst_set.AddNop(nop_mod, redundancy, ft_cost, cost, prob_fail);
      continue;
    }
    
    // Otherwise, it had better be in the main dictionary...
    int fun_id = -1;
    if(inst_dict.Find(inst_name, fun_id) == true){
      m_inst_set.AddInst(fun_id, redundancy, ft_cost, cost, prob_fail);
      continue;
    }
    
    // Oh oh!  Didn't find an instruction!
    m_world->GetDriver().RaiseFatalException(1, cString("Could not find instruction '") + inst_name +
                                             "'\n       (Best match = '" + inst_dict.NearMatch(inst_name) + "').");
  }
}

cHardwareBase* cHardwareManager::Create(cOrganism* in_org)
{
  assert(in_org != NULL);
  
  switch (m_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      return new cHardwareCPU(m_world, in_org, &m_inst_set);
    case HARDWARE_TYPE_CPU_4STACK:
      return new cHardware4Stack(m_world, in_org, &m_inst_set);
    case HARDWARE_TYPE_CPU_SMT:
      return new cHardwareSMT(m_world, in_org, &m_inst_set);
    default:
      return NULL;
  }
}
