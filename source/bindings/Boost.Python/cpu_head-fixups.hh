#ifndef CPU_HEAD_FIXUPS_HH
#define CPU_HEAD_FIXUPS_HH

#ifndef HEAD_CPU_HH
#include "head_cpu.hh"
#endif

bool cHeadCPU_GetFlagCopied(cHeadCPU &);
bool cHeadCPU_GetFlagMutated(cHeadCPU &);
bool cHeadCPU_GetFlagExecuted(cHeadCPU &);
bool cHeadCPU_GetFlagBreakpoint(cHeadCPU &);
bool cHeadCPU_GetFlagPointMut(cHeadCPU &);
bool cHeadCPU_GetFlagCopyMut(cHeadCPU &);

void cHeadCPU_SetFlagCopied(cHeadCPU &, bool);
void cHeadCPU_SetFlagMutated(cHeadCPU &, bool);
void cHeadCPU_SetFlagExecuted(cHeadCPU &, bool);
void cHeadCPU_SetFlagBreakpoint(cHeadCPU &, bool);
void cHeadCPU_SetFlagPointMut(cHeadCPU &, bool);
void cHeadCPU_SetFlagCopyMut(cHeadCPU &, bool);

#endif
