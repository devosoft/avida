#ifndef CPU_HEAD_FIXUPS_HH
#define CPU_HEAD_FIXUPS_HH

#ifndef CPU_HEAD_HH
#include "cpu_head.hh"
#endif

bool cCPUHead_GetFlagCopied(const cCPUHead &);
bool cCPUHead_GetFlagMutated(const cCPUHead &);
bool cCPUHead_GetFlagExecuted(const cCPUHead &);
bool cCPUHead_GetFlagBreakpoint(const cCPUHead &);
bool cCPUHead_GetFlagPointMut(const cCPUHead &);
bool cCPUHead_GetFlagCopyMut(const cCPUHead &);

void cCPUHead_SetFlagCopied(cCPUHead &, bool);
void cCPUHead_SetFlagMutated(cCPUHead &, bool);
void cCPUHead_SetFlagExecuted(cCPUHead &, bool);
void cCPUHead_SetFlagBreakpoint(cCPUHead &, bool);
void cCPUHead_SetFlagPointMut(cCPUHead &, bool);
void cCPUHead_SetFlagCopyMut(cCPUHead &, bool);

#endif
