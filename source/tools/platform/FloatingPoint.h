/*
 *  FloatingPoint.h
 *  Avida
 *
 *  Created by David on 5/20/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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
