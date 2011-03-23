/*
 *  FloatingPoint.h
 *  Avida
 *
 *  Created by David on 5/20/07.
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
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

#ifndef FloatingPoint_h
#define FloatingPoint_h

namespace AvidaTools {
  namespace Platform {

#if !defined(__APPLE__) && (defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL))
# define FPE_X86 1
#endif

#ifdef FPE_X86
void set_fpu (unsigned int mode)
{ 
#ifdef WIN32
	__asm fldcw mode;
#else
  asm("fldcw %0" : : "m" (*&mode));
#endif
}
#endif

inline void SetupFloatingPointEnvironment()
{
#ifdef FPE_X86
  set_fpu(0x27F); // Set the global rounding mode to double-precision
#endif
}
    
  };
};

#endif
