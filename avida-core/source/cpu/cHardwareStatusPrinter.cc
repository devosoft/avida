/*
 *  cHardwareStatusPrinter.cc
 *  Avida
 *
 *  Called "hardware_status_printer.cc" prior to 11/30/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cHardwareStatusPrinter.h"

#include "cAvidaContext.h"
#include "cHardwareBase.h"
#include "cHeadCPU.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cString.h"
#include "cStringUtil.h"

void cHardwareStatusPrinter::TraceHardware(cAvidaContext& ctx, cHardwareBase& hardware, bool bonus, bool minitrace, const int exec_success)
{
  cOrganism* organism = hardware.GetOrganism();

  if (!organism) return;
  
  bool in_setup = false;
  if (m_minitracer && minitrace && !m_file->HeaderDone()) {
    Apto::String genotype_name = organism->SystematicsGroup("genotype")->Properties().Get("genotype").StringValue();
    hardware.SetupMiniTraceFileHeader(*m_file, organism->SystematicsGroup("genotype")->ID(), genotype_name);
    in_setup = true;
  }
    
  if (exec_success == -2 || in_setup) {
    if (!m_minitracer && !minitrace) organism->PrintStatus(m_file->OFStream());
    else if (m_minitracer && minitrace) organism->PrintMiniTraceStatus(ctx, m_file->OFStream());
  }
  if (exec_success != -2 && (in_setup || (m_minitracer && minitrace))) {
    organism->PrintMiniTraceSuccess(m_file->OFStream(), exec_success);
  }
}

void cHardwareStatusPrinter::PrintSuccess(cOrganism* organism, const int exec_success)
{
  organism->PrintMiniTraceSuccess(m_file->OFStream(), exec_success);
}

void cHardwareStatusPrinter::TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)
{
  organism.PrintFinalStatus(m_file->OFStream(), time_used, time_allocated);
}
