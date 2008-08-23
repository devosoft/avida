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
    case HARDWARE_TYPE_CPU_GX:
      m_inst_set = new cInstSet(world, cHardwareGX::GetInstLib());
			default_filename = cHardwareGX::GetDefaultInstFilename();
			break;      
		default:
      m_world->GetDriver().RaiseFatalException(1, "Unknown/Unsupported HARDWARE_TYPE specified");
  }
  
  if (filename == "" || filename == "-") {
    filename = default_filename;
    m_world->GetDriver().NotifyComment(cString("Using default instruction set: ") + filename);
  }
  
  
  if (m_world->GetConfig().INST_SET_FORMAT.Get()) {
    m_inst_set->LoadFromConfig();
  } else {
    m_inst_set->LoadFromLegacyFile(filename);
  }
  
}

cHardwareBase* cHardwareManager::Create(cOrganism* in_org, cInstSet* inst_set)
{
  assert(in_org != NULL);
  
  switch (m_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      return new cHardwareCPU(m_world, in_org, inst_set);
    case HARDWARE_TYPE_CPU_SMT:
      return new cHardwareSMT(m_world, in_org, inst_set);
    case HARDWARE_TYPE_CPU_TRANSSMT:
      return new cHardwareTransSMT(m_world, in_org, inst_set);
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      return new cHardwareExperimental(m_world, in_org, inst_set);
    case HARDWARE_TYPE_CPU_GX:
      return new cHardwareGX(m_world, in_org, inst_set);
    default:
      return NULL;
  }
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
