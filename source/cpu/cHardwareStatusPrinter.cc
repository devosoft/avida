/*
 *  cHardwareStatusPrinter.cc
 *  Avida
 *
 *  Called "hardware_status_printer.cc" prior to 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
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
