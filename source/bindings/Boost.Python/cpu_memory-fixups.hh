#ifndef CPU_MEMORY_FIXUPS_HH
#define CPU_MEMORY_FIXUPS_HH

#ifndef CPU_MEMORY_HH
#include "cpu_memory.hh"
#endif

bool cCPUMemory_GetFlagCopied(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagMutated(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagExecuted(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagBreakpoint(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagPointMut(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagCopyMut(const cCPUMemory &, int pos);
bool cCPUMemory_GetFlagInjected(const cCPUMemory &, int pos);

void cCPUMemory_SetFlagCopied(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagMutated(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagExecuted(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagBreakpoint(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagPointMut(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagCopyMut(cCPUMemory &, int pos, bool);
void cCPUMemory_SetFlagInjected(cCPUMemory &, int pos, bool);

#endif
