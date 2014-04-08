/*
 *  cHardwareStatusPrinter.h
 *  Avida
 *
 *  Called "hardware_status_printer.hh" prior to 11/30/05.
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

#ifndef cHardwareStatusPrinter_h
#define cHardwareStatusPrinter_h

#include "avida/output/File.h"

#include "cHardwareTracer.h"

class cAvidaContext;


class cHardwareStatusPrinter : public cHardwareTracer
{
protected:
  Avida::Output::FilePtr m_file;
  bool m_minitracer;

public:
  cHardwareStatusPrinter(Avida::World* world, const Apto::String& filename, bool minitracer = false)
    : m_file(Avida::Output::File::CreateWithPath(world, filename)), m_minitracer(minitracer) { ; }

  virtual void TraceHardware(cAvidaContext& ctx, cHardwareBase& hardware, bool bonus, bool mini, int exec_success);
  virtual void PrintSuccess(cOrganism* organism, int exec_success);
  virtual void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism);
};

#endif
