#ifndef TEST_CPU_FIXUPS_HH
#define TEST_CPU_FIXUPS_HH

#ifndef TEST_CPU_HH
#include "cTestCPU.h"
#endif

bool cTestCPU_GetUseResources(cTestCPU &test_cpu);
void cTestCPU_SetUseResources(cTestCPU &test_cpu, bool value);

#endif
