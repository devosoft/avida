/*
 *  FloatingPoint.h
 *  Avida
 *
 *  Created by David on 5/20/07.
 *  Copyright 2007 Michigan State University. All rights reserved.
 *
 */

#ifndef FloatingPoint_h
#define FloatingPoint_h

#if !defined(__APPLE__) && (defined(__i386__) || defined(i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL))
# define FPE_X86 1
#endif

#ifdef FPE_X86
void set_fpu (unsigned int mode)
{ 
  asm("fldcw %0" : : "m" (*&mode));
}
#endif

inline void SetupFloatingPointEnvironment()
{
#ifdef FPE_X86
  set_fpu(0x27F); // Set the global rounding mode to double-precision
#endif
}

#endif
