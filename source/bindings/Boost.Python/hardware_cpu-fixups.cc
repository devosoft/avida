#ifndef HARDWARE_CPU_FIXUPS_HH
#include "hardware_cpu-fixups.hh"
#endif

int cHardwareCPU_GetRegister(cHardwareCPU &hardware_cpu, int reg_id){
  return hardware_cpu.Register(reg_id);
}
void cHardwareCPU_SetRegister(cHardwareCPU &hardware_cpu, int reg_id, int value){
  hardware_cpu.Register(reg_id) = value;
}
