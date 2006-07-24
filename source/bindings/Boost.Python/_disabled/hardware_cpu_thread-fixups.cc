#include "cHeadCPU.h"
#include "hardware_cpu_thread-fixups.hh"

const int cHardwareCPU_Thread_GetRegister(cHardwareCPU_Thread & thread, int index){
  return thread.reg[index];
}
const cHeadCPU &cHardwareCPU_Thread_GetHead(cHardwareCPU_Thread & thread, int index){
  return thread.heads[index];
}
