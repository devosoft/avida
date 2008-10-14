/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
#include "cHardwareGX.h"
#include "cHardwareStatusPrinter.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cWorld.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
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
    case HARDWARE_TYPE_CPU_GX:
      m_inst_set = new cInstSet(world, cHardwareGX::GetInstLib());
			default_filename = cHardwareGX::GetDefaultInstFilename();
			break;      
		default:
      cDriverManager::Status().SignalFatalError(1, "Unknown/Unsupported HARDWARE_TYPE specified");
  }

  if (filename == "" || filename == "-") {
    filename = default_filename;
    cDriverManager::Status().NotifyComment(cString("Using default instruction set: ") + filename);
  }
  
  if (m_world->GetConfig().INST_SET_FORMAT.Get()) {
    m_inst_set->LoadFromConfig();
  } else {
    m_inst_set->LoadFromLegacyFile(filename);
  }
}

cHardwareBase* cHardwareManager::Create(cOrganism* in_org, cInstSet* inst_set)
{
  static unsigned int cpu=0;
  assert(in_org != NULL);
	
  cHardwareBase* hw=0;
	
  switch (m_type) {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      hw = new cHardwareCPU(m_world, in_org, m_inst_set);
      break;
    case HARDWARE_TYPE_CPU_SMT:
      hw = new cHardwareSMT(m_world, in_org, m_inst_set);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      hw = new cHardwareTransSMT(m_world, in_org, m_inst_set);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      hw = new cHardwareExperimental(m_world, in_org, m_inst_set);
      break;
    case HARDWARE_TYPE_CPU_GX:
      hw = new cHardwareGX(m_world, in_org, m_inst_set);
      break;
    default:
      cDriverManager::Status().SignalFatalError(1, "Unknown/Unsupported HARDWARE_TYPE specified");
      break;
  }
  
  // Are we tracing the execution of this cpu?
  if(m_world->GetConfig().TRACE_EXECUTION.Get()) {
    std::ostringstream filename;
    filename << "trace-" << cpu++ << ".trace";    
    hw->SetTrace(new cHardwareStatusPrinter(m_world->GetDataFileOFStream(filename.str().c_str())));
  }
  
  assert(hw != 0);
  return hw;
}

bool cHardwareManager::SupportsSpeculative()
{
  switch (m_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:      return true;
    case HARDWARE_TYPE_CPU_SMT:           return false;
    case HARDWARE_TYPE_CPU_TRANSSMT:      return false;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:  return true;
    case HARDWARE_TYPE_CPU_GX:            return false;
    default:                              return false;
  }
}
