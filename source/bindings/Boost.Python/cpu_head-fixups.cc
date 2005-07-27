#include "cpu_head-fixups.hh"

bool cHeadCPU_GetFlagCopied(cHeadCPU &cpu_head){
  return cpu_head.FlagCopied();
}
bool cHeadCPU_GetFlagMutated(cHeadCPU &cpu_head){
  return cpu_head.FlagMutated();
}
bool cHeadCPU_GetFlagExecuted(cHeadCPU &cpu_head){
  return cpu_head.FlagExecuted();
}
bool cHeadCPU_GetFlagBreakpoint(cHeadCPU &cpu_head){
  return cpu_head.FlagBreakpoint();
}
bool cHeadCPU_GetFlagPointMut(cHeadCPU &cpu_head){
  return cpu_head.FlagPointMut();
}
bool cHeadCPU_GetFlagCopyMut(cHeadCPU &cpu_head){
  return cpu_head.FlagCopyMut();
}

void cHeadCPU_SetFlagCopied(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagCopied() = flag;
}
void cHeadCPU_SetFlagMutated(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagMutated() = flag;
}
void cHeadCPU_SetFlagExecuted(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagExecuted() = flag;
}
void cHeadCPU_SetFlagBreakpoint(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagBreakpoint() = flag;
}
void cHeadCPU_SetFlagPointMut(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagPointMut() = flag;
}
void cHeadCPU_SetFlagCopyMut(cHeadCPU &cpu_head, bool flag){
  cpu_head.FlagCopyMut() = flag;
}

