/*
 *  tcmalloc-config.h
 *  Avida
 *
 *  Created by David on 3/20/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "tcmalloc-platform.h"

#if PLATFORM(DARWIN)

#define HAVE_MMAP 1
#define HAVE_SBRK 1
#define PRIuS "u"

#else

#define HAVE_MMAP 1
#define HAVE_SBRK 1
#define PRIuS "u"

#endif

