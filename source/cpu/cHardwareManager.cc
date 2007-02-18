/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cHardwareManager.h"

#include "cHardwareCPU.h"
#include "cHardwareExperimental.h"
#include "cHardwareSMT.h"
#include "cHardwareTransSMT.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tDictionary.h"

cHardwareManager::cHardwareManager(cWorld* world)
: m_world(world), m_type(world->GetConfig().HARDWARE_TYPE.Get()) /*, m_testres(world) */
{
  cString filename = world->GetConfig().INST_SET.Get();

  // Setup the instruction library and collect the default filename
  cString default_filename;
	switch (m_type)
	{
		case HARDWARE_TYPE_CPU_ORIGINAL:
      m_inst_set = new cInstSet(world, cHardwareCPU::GetInstLib());
			default_filename = cHardwareCPU::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_SMT:
      m_inst_set = new cInstSet(world, cHardwareSMT::GetInstLib());
			default_filename = cHardwareSMT::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_TRANSSMT:
      m_inst_set = new cInstSet(world, cHardwareTransSMT::GetInstLib());
			default_filename = cHardwareTransSMT::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      m_inst_set = new cInstSet(world, cHardwareExperimental::GetInstLib());
			default_filename = cHardwareExperimental::GetDefaultInstFilename();
			break;
		default:
      m_world->GetDriver().RaiseFatalException(1, "Unknown/Unsupported HARDWARE_TYPE specified");
  }
  
  if (filename == "" || filename == "-") {
    filename = default_filename;
    m_world->GetDriver().NotifyComment(cString("Using default instruction set: ") + filename);
  }
  
  cInitFile file(filename);
  
  if (file.IsOpen() == false) {
    m_world->GetDriver().RaiseFatalException(1, cString("Could not open instruction set '") + filename + "'.");
  }
  
  file.Load();
  file.Compress();
  
  const cInstLib& inst_lib = *m_inst_set->GetInstLib();
  for (int line_id = 0; line_id < file.GetNumLines(); line_id++) {
    cString cur_line = file.GetLine(line_id);
    cString inst_name = cur_line.PopWord();
    int redundancy = cur_line.PopWord().AsInt();
    int cost = cur_line.PopWord().AsInt();
    int ft_cost = cur_line.PopWord().AsInt();
    double prob_fail = cur_line.PopWord().AsDouble();
    int addl_time_cost = cur_line.PopWord().AsInt();

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
    int inst_idx = inst_lib.GetIndex(inst_name);
    
    if (inst_idx == -1) {
      // Oh oh!  Didn't find an instruction!
      cString errorstr("Could not find instruction '");
      errorstr += inst_name + "'\n        (Best match = '" + inst_lib.GetNearMatch(inst_name) + "').";
      m_world->GetDriver().RaiseFatalException(1, errorstr);
    }
    
    if (inst_lib[inst_idx].IsNop()) {
      m_inst_set->AddNop(inst_idx, redundancy, ft_cost, cost, prob_fail, addl_time_cost);
    } else {
      m_inst_set->AddInst(inst_idx, redundancy, ft_cost, cost, prob_fail, addl_time_cost);
    }
  }
}

cHardwareBase* cHardwareManager::Create(cOrganism* in_org)
{
  assert(in_org != NULL);
  
  switch (m_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      return new cHardwareCPU(m_world, in_org, m_inst_set);
    case HARDWARE_TYPE_CPU_SMT:
      return new cHardwareSMT(m_world, in_org, m_inst_set);
    case HARDWARE_TYPE_CPU_TRANSSMT:
      return new cHardwareTransSMT(m_world, in_org, m_inst_set);
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      return new cHardwareExperimental(m_world, in_org, m_inst_set);
    default:
      return NULL;
  }
}
