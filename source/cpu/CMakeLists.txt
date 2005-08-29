SET(libcpu_a_SOURCES
  cCodeLabel.cc
  cCPUMemory.cc
  cCPUStack.cc
  cCPUTestInfo.cc
  cHardware4Stack.cc
  cHardware4Stack_Thread.cc
  cHardwareBase.cc
  cHardwareCPU.cc
  cHardwareCPU_Thread.cc
  cHardwareFactory.cc
  hardware_smt.cc
  hardware_smt_thread.cc
  hardware_status_printer.cc
  hardware_util.cc
  head_cpu.cc
  head_multi_mem.cc
  memory_flags.cc
  test_cpu.cc
  test_util.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(cpu ${libcpu_a_SOURCES})

