
cpu {
  HEADERS += \
             $$CPU_HH/cCodeLabel.h \
             $$CPU_HH/nHardware.h \
             $$CPU_HH/cCPUMemory.h \
             $$CPU_HH/cCPUStack.h \
             $$CPU_HH/sCPUStats.h \
             $$CPU_HH/cHardware4Stack.h \
             $$CPU_HH/cHardwareBase.h \
             $$CPU_HH/cHardwareCPU.h \
             $$CPU_HH/cHardwareFactory.h \
             $$CPU_HH/cHardwareUtil.h \
             $$CPU_HH/cTestCPU.h \
             $$CPU_HH/cTestUtil.h

  SOURCES += \
             $$CPU_CC/4stack_head.cc \
             $$CPU_CC/cCodeLabel.cc \
             $$CPU_CC/cpu_head.cc \
             $$CPU_CC/cCPUMemory.cc \
             $$CPU_CC/cCPUStack.cc \
             $$CPU_CC/cCPUTestInfo.cc \
             $$CPU_CC/cHardware4Stack.cc \
             $$CPU_CC/cHardware4Stack_Thread.cc \
             $$CPU_CC/cHardwareBase.cc \
             $$CPU_CC/cHardwareCPU.cc \
             $$CPU_CC/cHardwareCPU_Thread.cc \
             $$CPU_CC/cHardwareFactory.cc \
             $$CPU_CC/cHardwareUtil.cc \
             $$CPU_CC/cTestCPU.cc \
             $$CPU_CC/cTestUtil.cc
}
