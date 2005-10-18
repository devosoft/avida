#ifndef HARDWARE_CPU_THREAD_FIXUPS_HH
#define HARDWARE_CPU_THREAD_FIXUPS_HH

#ifndef HARDWARE_CPU_THREAD_HH
#include "cHardwareCPU_Thread.h"
#endif

#ifndef HEAD_CPU_HH
#include "cHeadCPU.h"
#endif

const int cHardwareCPU_Thread_GetRegister(
  cHardwareCPU_Thread & thread,
  int index
);
const cHeadCPU &cHardwareCPU_Thread_GetHead(
  cHardwareCPU_Thread & thread,
  int index
);

#endif
