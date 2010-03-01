/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
#include "cHardwareCPU.h"
#include "cHardwareExperimental.h"
#include "cHardwareSMT.h"
#include "cHardwareTransSMT.h"
#include "cHardwareGX.h"
#include "cHardwareStatusPrinter.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cMetaGenome.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tDictionary.h"


cHardwareManager::cHardwareManager(cWorld* world)
: m_world(world), m_cpu_count(0)
{
  cString filename = world->GetConfig().INST_SET.Get();

  // Setup the instruction library and collect the default filename
  cString default_filename;
	switch (world->GetConfig().HARDWARE_TYPE.Get())
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
    case HARDWARE_TYPE_CPU_GX:
      m_inst_set = new cInstSet(world, cHardwareGX::GetInstLib());
			default_filename = cHardwareGX::GetDefaultInstFilename();
			break;      
		default:
      cDriverManager::Status().SignalError("Unknown/Unsupported HARDWARE_TYPE specified", -1);
  }

  if (filename == "" || filename == "-") {
    filename = default_filename;
    cDriverManager::Status().NotifyComment(cString("Using default instruction set: ") + filename);
    // set INST_SET so that the proper name will show up in the text viewer
    world->GetConfig().INST_SET.Set(filename);
  }
  
  if (m_world->GetConfig().INST_SET_FORMAT.Get()) {
    m_inst_set->LoadFromConfig();
  } else {
    m_inst_set->LoadFromLegacyFile(filename);
  }
}

cHardwareBase* cHardwareManager::Create(cAvidaContext& ctx, cOrganism* org, const cMetaGenome& mg, cInstSet* is)
{
  assert(org != NULL);
	
  int inst_set_id = (is == NULL) ? 1 : -1;
  cInstSet* inst_set = (is == NULL) ? m_inst_set : is;
  
  cHardwareBase* hw = 0;
	
  switch (mg.GetHardwareType()) {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      hw = new cHardwareCPU(ctx, m_world, org, inst_set, inst_set_id);
      break;
    case HARDWARE_TYPE_CPU_SMT:
      hw = new cHardwareSMT(ctx, m_world, org, inst_set, inst_set_id);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      hw = new cHardwareTransSMT(ctx, m_world, org, inst_set, inst_set_id);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      hw = new cHardwareExperimental(ctx, m_world, org, inst_set, inst_set_id);
      break;
    case HARDWARE_TYPE_CPU_GX:
      hw = new cHardwareGX(ctx, m_world, org, inst_set, inst_set_id);
      break;
    default:
      cDriverManager::Status().SignalError("Unknown/Unsupported HARDWARE_TYPE specified", -1);
      break;
  }
  
  // Are we tracing the execution of this cpu?
  if (m_world->GetConfig().TRACE_EXECUTION.Get()) {
    cString filename =  cStringUtil::Stringf("trace-%d.trace", m_cpu_count++);
    hw->SetTrace(new cHardwareStatusPrinter(m_world->GetDataFileOFStream(filename)));
  }
  
  assert(hw != 0);
  return hw;
}
