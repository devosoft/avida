/*
 *  cHardwareTracer.h
 *  Avida
 *
 *  Called "hardware_tracer.hh" prior to 11/30/05.
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

#ifndef cHardwareTracer_h
#define cHardwareTracer_h

#include "avida/core/Types.h"

class cAvidaContext;
class cHardwareBase;
class cOrganism;
class cString;


class cHardwareTracer : public Apto::RefCountObject<Apto::SingleThreaded>
{
public:
  virtual ~cHardwareTracer() { ; }
  virtual void TraceHardware(cAvidaContext& ctx, cHardwareBase&, bool bonus = false, bool mini = false, int exec_success = -2) = 0;
  virtual void PrintSuccess(cOrganism* organism, int exec_success) = 0;
  virtual void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism) = 0;
};

typedef Apto::SmartPtr<cHardwareTracer, Apto::InternalRCObject> HardwareTracerPtr;

#endif
