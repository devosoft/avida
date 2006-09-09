SET(libcpu_a_SOURCES
  cCodeLabel.cc
  cCPUMemory.cc
  cCPUStack.cc
  cCPUTestInfo.cc
  cHardwareBase.cc
  cHardwareCPU.cc
  cHardwareManager.cc
  cHardwareSMT.cc
  cHardwareStatusPrinter.cc
  cHardwareTransSMT.cc
  cHeadCPU.cc
  cTestCPU.cc
  cTestCPUInterface.cc
  cTestUtil.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(cpu ${libcpu_a_SOURCES})

