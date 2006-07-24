#ifndef HARDWARE_CPU_FIXUPS_HH
#define HARDWARE_CPU_FIXUPS_HH

#ifndef cHardwareCPU_h
#include "cHardwareCPU.h"
#endif

int cHardwareCPU_GetRegister(cHardwareCPU &hardware_cpu, int reg_id);
void cHardwareCPU_SetRegister(cHardwareCPU &hardware_cpu, int reg_id, int value);

#endif
