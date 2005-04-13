#include "cpu_head-fixups.hh"

bool cCPUHead_GetFlagCopied(const cCPUHead &cpu_head){
  return cpu_head.FlagCopied();
}
bool cCPUHead_GetFlagMutated(const cCPUHead &cpu_head){
  return cpu_head.FlagMutated();
}
bool cCPUHead_GetFlagExecuted(const cCPUHead &cpu_head){
  return cpu_head.FlagExecuted();
}
bool cCPUHead_GetFlagBreakpoint(const cCPUHead &cpu_head){
  return cpu_head.FlagBreakpoint();
}
bool cCPUHead_GetFlagPointMut(const cCPUHead &cpu_head){
  return cpu_head.FlagPointMut();
}
bool cCPUHead_GetFlagCopyMut(const cCPUHead &cpu_head){
  return cpu_head.FlagCopyMut();
}

void cCPUHead_SetFlagCopied(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagCopied() = flag;
}
void cCPUHead_SetFlagMutated(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagMutated() = flag;
}
void cCPUHead_SetFlagExecuted(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagExecuted() = flag;
}
void cCPUHead_SetFlagBreakpoint(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagBreakpoint() = flag;
}
void cCPUHead_SetFlagPointMut(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagPointMut() = flag;
}
void cCPUHead_SetFlagCopyMut(cCPUHead &cpu_head, bool flag){
  cpu_head.FlagCopyMut() = flag;
}

