/*
 *  tcmalloc-platform.h
 *  Avida
 *
 *  Added by David on 10/14/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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
#ifndef TCMALLOC_PLATFORM_H
#define TCMALLOC_PLATFORM_H

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Always the empty-string on non-windows systems. On windows, should be
 "__declspec(dllexport)". This way, when we compile the dll, we export our
 functions/classes. It's safe to define this here because config.h is only
 used internally, to compile the DLL, and every DLL source file #includes
 "tcmalloc-platform.h" before anything else. */
#if defined(WIN32) || defined(_WIN32)
# define __declspec(dllimport)
#else
# define PERFTOOLS_DLL_DECL 
#endif

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1


#define HAVE_MMAP 1
#define HAVE_SBRK 0

/* printf format code for printing a size_t and ssize_t */
#define PRIuS "zu"

#define HAVE_SCHED_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_SYSCALL_H 1


/* Define to 1 if the system has the type `struct mallinfo'. */
/* #undef HAVE_STRUCT_MALLINFO */

/* Define to 1 if compiler supports __thread */
/* #undef HAVE_TLS */


/* define if your compiler has __attribute__ */
#define HAVE___ATTRIBUTE__ 1


/* C99 says: define this to get the PRI... macros from stdint.h */
#ifndef __STDC_FORMAT_MACROS
# define __STDC_FORMAT_MACROS 1
#endif


#ifdef __MINGW32__
#include "windows/mingw.h"
#endif


#endif
