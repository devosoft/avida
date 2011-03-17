/*
 *  cHardwareStatusPrinter.h
 *  Avida
 *
 *  Called "hardware_status_printer.hh" prior to 11/30/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cHardwareStatusPrinter_h
#define cHardwareStatusPrinter_h

#include <iostream>

#ifndef cHardwareTracer_h
#include "cHardwareTracer.h"
#endif


class cHardwareStatusPrinter : public cHardwareTracer
{
protected:
  std::ostream& m_trace_fp;


private: 
  cHardwareStatusPrinter(); // @not_implemented


public:
  cHardwareStatusPrinter(std::ostream& trace_fp) : m_trace_fp(trace_fp) { ; }

  virtual void TraceHardware(cHardwareBase& hardware, bool bonus);
  virtual void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism);
  virtual std::ostream * GetStream() { return &m_trace_fp; }  
};

#endif
