/*
 *  platform.h
 *  Avida
 *
 *  Created by David on 3/2/07.
 *  Copyright 2007 Michigan State University. All rights reserved.
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

#ifndef platform_h
#define platform_h

#define AVIDA_PLATFORM(PROP) (defined(AVIDA_PLATFORM_##PROP) && AVIDA_PLATFORM_##PROP)

#if defined(WIN32) || defined(_WIN32)
# define AVIDA_PLATFORM_WINDOWS 1
# ifdef ENABLE_THREADS
#  define AVIDA_PLATFORM_THREADS 1
# else
#  define AVIDA_PLATFORM_THREADS 0
# endif
#endif

#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__) || defined (__NetBSD__) || defined(_AIX)
# define AVIDA_PLATFORM_UNIX 1
# define AVIDA_PLATFORM_THREADS 1
#endif


#ifdef DEBUG
# include <cstdlib>
# include <iostream>
# define ASSERT_MSG(VALUE, MSG) if (!(VALUE)) { std::cerr << "Error: " << MSG << std::endl; abort(); }
#else
# define ASSERT_MSG(VALUE, MSG)
#endif


#ifndef NULL
#define NULL 0
#endif

#endif
