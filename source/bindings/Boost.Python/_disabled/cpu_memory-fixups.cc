#include "cpu_memory-fixups.hh"

bool cCPUMemory_GetFlagCopied(const cCPUMemory &mem, int pos){
  return mem.FlagCopied(pos);
}
bool cCPUMemory_GetFlagMutated(const cCPUMemory &mem, int pos){
  return mem.FlagMutated(pos);
}
bool cCPUMemory_GetFlagExecuted(const cCPUMemory &mem, int pos){
  return mem.FlagExecuted(pos);
}
bool cCPUMemory_GetFlagBreakpoint(const cCPUMemory &mem, int pos){
  return mem.FlagBreakpoint(pos);
}
bool cCPUMemory_GetFlagPointMut(const cCPUMemory &mem, int pos){
  return mem.FlagPointMut(pos);
}
bool cCPUMemory_GetFlagCopyMut(const cCPUMemory &mem, int pos){
  return mem.FlagCopyMut(pos);
}
bool cCPUMemory_GetFlagInjected(const cCPUMemory &mem, int pos){
  return mem.FlagInjected(pos);
}

void cCPUMemory_SetFlagCopied(cCPUMemory &mem, int pos, bool flag){
  mem.FlagCopied(pos) = flag;
}
void cCPUMemory_SetFlagMutated(cCPUMemory &mem, int pos, bool flag){
  mem.FlagMutated(pos) = flag;
}
void cCPUMemory_SetFlagExecuted(cCPUMemory &mem, int pos, bool flag){
  mem.FlagExecuted(pos) = flag;
}
void cCPUMemory_SetFlagBreakpoint(cCPUMemory &mem, int pos, bool flag){
  mem.FlagBreakpoint(pos) = flag;
}
void cCPUMemory_SetFlagPointMut(cCPUMemory &mem, int pos, bool flag){
  mem.FlagPointMut(pos) = flag;
}
void cCPUMemory_SetFlagCopyMut(cCPUMemory &mem, int pos, bool flag){
  mem.FlagCopyMut(pos) = flag;
}
void cCPUMemory_SetFlagInjected(cCPUMemory &mem, int pos, bool flag){
  mem.FlagInjected(pos) = flag;
}


