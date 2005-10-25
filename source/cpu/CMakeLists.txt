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
  cHardwareManager.cc
  cHardwareSMT.cc
  cHardwareSMT_Thread.cc
  cHardwareStatusPrinter.cc
  cHeadCPU.cc
  cHeadMultiMem.cc
  cTestCPU.cc
  cTestUtil.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(cpu ${libcpu_a_SOURCES})

