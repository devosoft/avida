#include "py_hardware_tracer.hh"

#include "cHardware4Stack.h"
#include "cHardwareCPU.h"
#include "cHardwareTracer.h"
#include "cHardwareTracer_4Stack.h"
#include "cHardwareTracer_CPU.h"
#include "cHardwareTracer_TestCPU.h"

#include <iostream>


void pyHardwareTracerBase::TraceHardware_CPU(cHardwareCPU &hardware){
  std::cout
  << "pyHardwareTracerBase::TraceHardware_CPU not implemented."
  << std::endl;
}
void pyHardwareTracerBase::TraceHardware_4Stack(cHardware4Stack &hardware){
  std::cout
  << "pyHardwareTracerBase::TraceHardware_4Stack not implemented."
  << std::endl;
}
void pyHardwareTracerBase::TraceHardware_CPUBonus(cHardwareCPU &hardware){
  std::cout
  << "pyHardwareTracerBase::TraceHardware_CPUBonus not implemented."
  << std::endl;
}
void pyHardwareTracerBase::TraceHardware_4StackBonus(cHardware4Stack &hardware){
  std::cout
  << "pyHardwareTracerBase::TraceHardware_4StackBonus not implemented."
  << std::endl;
}
void pyHardwareTracerBase::TraceHardware_TestCPU(
  int time_used,
  int time_allocated,
  int size,
  const cString &final_memory,
  const cString &child_memory
){
  std::cout
  << "pyHardwareTracerBase::TraceHardware_TestCPU not implemented."
  << std::endl;
}

