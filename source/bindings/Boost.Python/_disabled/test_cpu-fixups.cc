
#include "test_cpu-fixups.hh"

bool cTestCPU_GetUseResources(cTestCPU &test_cpu){
  return test_cpu.UseResources();
}
void cTestCPU_SetUseResources(cTestCPU &test_cpu, bool value){
  test_cpu.UseResources() = value;
}
