/*
 *  malloc_extension.cc
 *  Avida
 *
 *  Added by David on 10/14/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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
// Author: Sanjay Ghemawat <opensource@google.com>

#include "tcmalloc-platform.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined HAVE_STDINT_H
#include <stdint.h>
#elif defined HAVE_INTTYPES_H
#include <inttypes.h>
#else
#include <sys/types.h>
#endif
#include <string>
#include "sysinfo.h"    // for FillProcSelfMaps
#include "malloc_extension.h"

using std::string;

// Note: this routine is meant to be called before threads are spawned.
void MallocExtension::Initialize() {
  static bool initialize_called = false;

  if (initialize_called) return;
  initialize_called = true;

#ifdef __GLIBC__
  // GNU libc++ versions 3.3 and 3.4 obey the environment variables
  // GLIBCPP_FORCE_NEW and GLIBCXX_FORCE_NEW respectively.  Setting
  // one of these variables forces the STL default allocator to call
  // new() or delete() for each allocation or deletion.  Otherwise
  // the STL allocator tries to avoid the high cost of doing
  // allocations by pooling memory internally.  However, tcmalloc
  // does allocations really fast, especially for the types of small
  // items one sees in STL, so it's better off just using us.
  // TODO: control whether we do this via an environment variable?
  setenv("GLIBCPP_FORCE_NEW", "1", false /* no overwrite*/);
  setenv("GLIBCXX_FORCE_NEW", "1", false /* no overwrite*/);

  // Now we need to make the setenv 'stick', which it may not do since
  // the env is flakey before main() is called.  But luckily stl only
  // looks at this env var the first time it tries to do an alloc, and
  // caches what it finds.  So we just cause an stl alloc here.
  string dummy("I need to be allocated");
  dummy += "!";         // so the definition of dummy isn't optimized out
#endif  /* __GLIBC__ */
}

// Default implementation -- does nothing
MallocExtension::~MallocExtension() { }
bool MallocExtension::VerifyAllMemory() { return true; }
bool MallocExtension::VerifyNewMemory(void* p) { return true; }
bool MallocExtension::VerifyArrayNewMemory(void* p) { return true; }
bool MallocExtension::VerifyMallocMemory(void* p) { return true; }

bool MallocExtension::GetNumericProperty(const char* property, size_t* value) {
  return false;
}

bool MallocExtension::SetNumericProperty(const char* property, size_t value) {
  return false;
}

void MallocExtension::GetStats(char* buffer, int length) {
  assert(length > 0);
  buffer[0] = '\0';
}

bool MallocExtension::MallocMemoryStats(int* blocks, size_t* total,
                                       int histogram[kMallocHistogramSize]) {
  *blocks = 0;
  *total = 0;
  memset(histogram, 0, sizeof(histogram));
  return true;
}

void MallocExtension::MarkThreadIdle() {
  // Default implementation does nothing
}

void MallocExtension::MarkThreadBusy() {
  // Default implementation does nothing
}

void MallocExtension::ReleaseFreeMemory() {
  // Default implementation does nothing
}

void MallocExtension::SetMemoryReleaseRate(double rate) {
  // Default implementation does nothing
}

double MallocExtension::GetMemoryReleaseRate() {
  return -1.0;
}

size_t MallocExtension::GetEstimatedAllocatedSize(size_t size) {
  return size;
}

size_t MallocExtension::GetAllocatedSize(void* p) {
  return 0;
}

// The current malloc extension object.

static pthread_once_t module_init = PTHREAD_ONCE_INIT;
static MallocExtension* current_instance = NULL;

static void InitModule() {
  current_instance = new MallocExtension;
}

MallocExtension* MallocExtension::instance() {
  pthread_once(&module_init, InitModule);
  return current_instance;
}

static int GetRunningOnValgrind() {
  const char *running_on_valgrind_str = GetenvBeforeMain("RUNNING_ON_VALGRIND");
  if (running_on_valgrind_str) {
    return strcmp(running_on_valgrind_str, "0") != 0;
  }
  return 0;
}

// When running under valgrind, this function will be intercepted
// and a non-zero value will be returned.
// Some valgrind-based tools (e.g. callgrind) do not intercept functions,
// so we also read environment variable.
extern "C" int RunningOnValgrind() {
  static int running_on_valgrind = GetRunningOnValgrind();
  return running_on_valgrind;
}

void MallocExtension::Register(MallocExtension* implementation) {
  pthread_once(&module_init, InitModule);
  // When running under valgrind, our custom malloc is replaced with
  // valgrind's one and malloc extensions will not work.
  if (!RunningOnValgrind()) {
    current_instance = implementation;
  }
}

// -----------------------------------------------------------------------
// Heap sampling support
// -----------------------------------------------------------------------

namespace {

// Accessors
uintptr_t Count(void** entry) {
  return reinterpret_cast<uintptr_t>(entry[0]);
}
uintptr_t Size(void** entry) {
  return reinterpret_cast<uintptr_t>(entry[1]);
}
uintptr_t Depth(void** entry) {
  return reinterpret_cast<uintptr_t>(entry[2]);
}
void* PC(void** entry, int i) {
  return entry[3+i];
}

void PrintCountAndSize(MallocExtensionWriter* writer,
                       uintptr_t count, uintptr_t size) {
  char buf[100];
  snprintf(buf, sizeof(buf),
           "%6lld: %8lld [%6lld: %8lld] @",
           static_cast<long long>(count),
           static_cast<long long>(size),
           static_cast<long long>(count),
           static_cast<long long>(size));
  writer->append(buf, strlen(buf));
}

void PrintHeader(MallocExtensionWriter* writer,
                 const char* label, void** entries) {
  // Compute the total count and total size
  uintptr_t total_count = 0;
  uintptr_t total_size = 0;
  for (void** entry = entries; Count(entry) != 0; entry += 3 + Depth(entry)) {
    total_count += Count(entry);
    total_size += Size(entry);
  }

  const char* const kTitle = "heap profile: ";
  writer->append(kTitle, strlen(kTitle));
  PrintCountAndSize(writer, total_count, total_size);
  writer->append(" ", 1);
  writer->append(label, strlen(label));
  writer->append("\n", 1);
}

void PrintStackEntry(MallocExtensionWriter* writer, void** entry) {
  PrintCountAndSize(writer, Count(entry), Size(entry));

  for (unsigned int i = 0; i < Depth(entry); i++) {
    char buf[32];
    snprintf(buf, sizeof(buf), " %p", PC(entry, i));
    writer->append(buf, strlen(buf));
  }
  writer->append("\n", 1);
}

}


// These are C shims that work on the current instance.

#define C_SHIM(fn, retval, paramlist, arglist)          \
  extern "C" PERFTOOLS_DLL_DECL retval MallocExtension_##fn paramlist {    \
    return MallocExtension::instance()->fn arglist;     \
  }

C_SHIM(VerifyAllMemory, bool, (), ());
C_SHIM(VerifyNewMemory, bool, (void* p), (p));
C_SHIM(VerifyArrayNewMemory, bool, (void* p), (p));
C_SHIM(VerifyMallocMemory, bool, (void* p), (p));
C_SHIM(MallocMemoryStats, bool,
       (int* blocks, size_t* total, int histogram[kMallocHistogramSize]),
       (blocks, total, histogram));

C_SHIM(GetStats, void,
       (char* buffer, int buffer_length), (buffer, buffer_length));
C_SHIM(GetNumericProperty, bool,
       (const char* property, size_t* value), (property, value));
C_SHIM(SetNumericProperty, bool,
       (const char* property, size_t value), (property, value));

C_SHIM(MarkThreadIdle, void, (), ());
C_SHIM(MarkThreadBusy, void, (), ());
C_SHIM(ReleaseFreeMemory, void, (), ());
C_SHIM(GetEstimatedAllocatedSize, size_t, (size_t size), (size));
C_SHIM(GetAllocatedSize, size_t, (void* p), (p));
