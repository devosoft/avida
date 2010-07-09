/*
 *  cHardwareStatusPrinter.cc
 *  Avida
 *
 *  Called "hardware_status_printer.cc" prior to 11/30/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
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

#include "cHardwareStatusPrinter.h"

#include "cHardwareBase.h"
#include "cHeadCPU.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cString.h"
#include "cStringUtil.h"

void cHardwareStatusPrinter::TraceHardware(cHardwareBase& hardware, bool bonus)
{
  cString next_name(hardware.GetInstSet().GetName(hardware.IP().GetInst()));
  if (bonus) next_name = cStringUtil::Stringf("%s (bonus instruction)", static_cast<const char*>(next_name));

  cOrganism* organism = hardware.GetOrganism();
  if (organism) organism->PrintStatus(m_trace_fp, next_name);
}

void cHardwareStatusPrinter::TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)
{
  organism.PrintFinalStatus(m_trace_fp, time_used, time_allocated);
}
