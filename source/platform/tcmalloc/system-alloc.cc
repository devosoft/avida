/*
 *  system-alloc.cc
 *  Avida
 *
 *  Created by David on 3/20/06.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

// Copyright (c) 2005, Google Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: Sanjay Ghemawat

#include "tcmalloc-config.h"

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "spinlock.h"
#include "tcmalloc-logging.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

// Structure for discovering alignment
union MemoryAligner {
  void*  p;
  double d;
  size_t s;
};

static SpinLock spinlock = SPINLOCK_INITIALIZER;

// Page size is initialized on demand
static size_t pagesize = 0;

// Configuration parameters.
//
// if use_devmem is true, either use_sbrk or use_mmap must also be true.
// For 2.2 kernels, it looks like the sbrk address space (500MBish) and
// the mmap address space (1300MBish) are disjoint, so we need both allocators
// to get as much virtual memory as possible.
// @not_used -static bool use_devmem = true;
static bool use_sbrk = false;
static bool use_mmap = true;

// Flags to keep us from retrying allocators that failed.
static bool devmem_failure = false;
static bool sbrk_failure = false;
static bool mmap_failure = false;

// @not_used -DEFINE_int32(malloc_devmem_start, 0,
// @not_used -             "Physical memory starting location in MB for /dev/mem allocation."
// @not_used -             "  Setting this to 0 disables /dev/mem allocation");
// @not_used -DEFINE_int32(malloc_devmem_limit, 0,
// @not_used -             "Physical memory limit location in MB for /dev/mem allocation."
// @not_used -             "  Setting this to 0 means no limit.");
static const int32_t FLAGS_malloc_devmem_start = 0;
static const int32_t FLAGS_malloc_devmem_limit = 0;

#ifdef HAVE_SBRK

static void* TrySbrk(size_t size, size_t alignment) {
  // sbrk will release memory if passed a negative number, so we do
  // a strict check here
  if (static_cast<int>(size + alignment) < 0) return NULL;
  
  size = ((size + alignment - 1) / alignment) * alignment;
  void* result = sbrk(size);
  if (result == reinterpret_cast<void*>(-1)) {
    sbrk_failure = true;
    return NULL;
  }
  
  // Is it aligned?
  uintptr_t ptr = reinterpret_cast<uintptr_t>(result);
  if ((ptr & (alignment-1)) == 0)  return result;
  
  // Try to get more memory for alignment
  size_t extra = alignment - (ptr & (alignment-1));
  void* r2 = sbrk(extra);
  if (reinterpret_cast<uintptr_t>(r2) == (ptr + size)) {
    // Contiguous with previous result
    return reinterpret_cast<void*>(ptr + extra);
  }
  
  // Give up and ask for "size + alignment - 1" bytes so
  // that we can find an aligned region within it.
  result = sbrk(size + alignment - 1);
  if (result == reinterpret_cast<void*>(-1)) {
    sbrk_failure = true;
    return NULL;
  }
  ptr = reinterpret_cast<uintptr_t>(result);
  if ((ptr & (alignment-1)) != 0) {
    ptr += alignment - (ptr & (alignment-1));
  }
  return reinterpret_cast<void*>(ptr);
}

#endif /* HAVE_SBRK */

#ifdef HAVE_MMAP

static void* TryMmap(size_t size, size_t alignment) {
  // Enforce page alignment
  if (pagesize == 0) pagesize = getpagesize();
  if (alignment < pagesize) alignment = pagesize;
  size = ((size + alignment - 1) / alignment) * alignment;
  
  // Ask for extra memory if alignment > pagesize
  size_t extra = 0;
  if (alignment > pagesize) {
    extra = alignment - pagesize;
  }
  
  // Note: size + extra does not overflow since:
  //            size + alignment < (1<<NBITS).
  // and        extra <= alignment
  // therefore  size + extra < (1<<NBITS)
  void* result = mmap(NULL, size + extra,
                      PROT_READ|PROT_WRITE,
                      MAP_PRIVATE|MAP_ANONYMOUS,
                      -1, 0);
  if (result == reinterpret_cast<void*>(MAP_FAILED)) {
    mmap_failure = true;
    return NULL;
  }
  
  // Adjust the return memory so it is aligned
  uintptr_t ptr = reinterpret_cast<uintptr_t>(result);
  size_t adjust = 0;
  if ((ptr & (alignment - 1)) != 0) {
    adjust = alignment - (ptr & (alignment - 1));
  }
  
  // Return the unused memory to the system
  if (adjust > 0) {
    munmap(reinterpret_cast<void*>(ptr), adjust);
  }
  if (adjust < extra) {
    munmap(reinterpret_cast<void*>(ptr + adjust + size), extra - adjust);
  }
  
  ptr += adjust;
  return reinterpret_cast<void*>(ptr);
}

#endif /* HAVE_MMAP */

// @not_used -static void* TryDevMem(size_t size, size_t alignment) {
// @not_used -  static bool initialized = false;
// @not_used -  static off_t physmem_base;  // next physical memory address to allocate
// @not_used -  static off_t physmem_limit; // maximum physical address allowed
// @not_used -  static int physmem_fd;      // file descriptor for /dev/mem
// @not_used -  
// @not_used -  // Check if we should use /dev/mem allocation.  Note that it may take
// @not_used -  // a while to get this flag initialized, so meanwhile we fall back to
// @not_used -  // the next allocator.  (It looks like 7MB gets allocated before
// @not_used -  // this flag gets initialized -khr.)
// @not_used -  if (FLAGS_malloc_devmem_start == 0) {
// @not_used -    // NOTE: not a devmem_failure - we'd like TCMalloc_SystemAlloc to
// @not_used -    // try us again next time.
// @not_used -    return NULL;
// @not_used -  }
// @not_used -  
// @not_used -  if (!initialized) {
// @not_used -    physmem_fd = open("/dev/mem", O_RDWR);
// @not_used -    if (physmem_fd < 0) {
// @not_used -      devmem_failure = true;
// @not_used -      return NULL;
// @not_used -    }
// @not_used -    physmem_base = FLAGS_malloc_devmem_start*1024LL*1024LL;
// @not_used -    physmem_limit = FLAGS_malloc_devmem_limit*1024LL*1024LL;
// @not_used -    initialized = true;
// @not_used -  }
// @not_used -  
// @not_used -  // Enforce page alignment
// @not_used -  if (pagesize == 0) pagesize = getpagesize();
// @not_used -  if (alignment < pagesize) alignment = pagesize;
// @not_used -  size = ((size + alignment - 1) / alignment) * alignment;
// @not_used -  
// @not_used -  // Ask for extra memory if alignment > pagesize
// @not_used -  size_t extra = 0;
// @not_used -  if (alignment > pagesize) {
// @not_used -    extra = alignment - pagesize;
// @not_used -  }
// @not_used -  
// @not_used -  // check to see if we have any memory left
// @not_used -  if (physmem_limit != 0 &&
// @not_used -      ((size + extra) > (physmem_limit - physmem_base))) {
// @not_used -    devmem_failure = true;
// @not_used -    return NULL;
// @not_used -  }
// @not_used -  
// @not_used -  // Note: size + extra does not overflow since:
// @not_used -  //            size + alignment < (1<<NBITS).
// @not_used -  // and        extra <= alignment
// @not_used -  // therefore  size + extra < (1<<NBITS)
// @not_used -  void *result = mmap(0, size + extra, PROT_WRITE|PROT_READ,
// @not_used -                      MAP_SHARED, physmem_fd, physmem_base);
// @not_used -  if (result == reinterpret_cast<void*>(MAP_FAILED)) {
// @not_used -    devmem_failure = true;
// @not_used -    return NULL;
// @not_used -  }
// @not_used -  uintptr_t ptr = reinterpret_cast<uintptr_t>(result);
// @not_used -  
// @not_used -  // Adjust the return memory so it is aligned
// @not_used -  size_t adjust = 0;
// @not_used -  if ((ptr & (alignment - 1)) != 0) {
// @not_used -    adjust = alignment - (ptr & (alignment - 1));
// @not_used -  }
// @not_used -  
// @not_used -  // Return the unused virtual memory to the system
// @not_used -  if (adjust > 0) {
// @not_used -    munmap(reinterpret_cast<void*>(ptr), adjust);
// @not_used -  }
// @not_used -  if (adjust < extra) {
// @not_used -    munmap(reinterpret_cast<void*>(ptr + adjust + size), extra - adjust);
// @not_used -  }
// @not_used -  
// @not_used -  ptr += adjust;
// @not_used -  physmem_base += adjust + size;
// @not_used -  
// @not_used -  return reinterpret_cast<void*>(ptr);
// @not_used -}

void* TCMalloc_SystemAlloc(size_t size, size_t alignment) {
  // Discard requests that overflow
  if (size + alignment < size) return NULL;
  
  if (TCMallocDebug::level >= TCMallocDebug::kVerbose) {
    MESSAGE("TCMalloc_SystemAlloc(%" PRIuS ", %" PRIuS")\n", 
            size, alignment);
  }
  SpinLockHolder lock_holder(&spinlock);
  
  // Enforce minimum alignment
  if (alignment < sizeof(MemoryAligner)) alignment = sizeof(MemoryAligner);
  
  // Try twice, once avoiding allocators that failed before, and once
  // more trying all allocators even if they failed before.
  for (int i = 0; i < 2; i++) {
// @not_used -    if (use_devmem && !devmem_failure) {
// @not_used -      void* result = TryDevMem(size, alignment);
// @not_used -      if (result != NULL) return result;
// @not_used -    }
    
#ifdef HAVE_SBRK
    if (use_sbrk && !sbrk_failure) {
      void* result = TrySbrk(size, alignment);
      if (result != NULL) return result;
    }
#endif
    
#ifdef HAVE_MMAP    
    if (use_mmap && !mmap_failure) {
      void* result = TryMmap(size, alignment);
      if (result != NULL) return result;
    }
#endif
    
    // nothing worked - reset failure flags and try again
    devmem_failure = false;
    sbrk_failure = false;
    mmap_failure = false;
  }
  
  return NULL;
}
