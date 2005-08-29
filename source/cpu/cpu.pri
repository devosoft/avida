
cpu {
  HEADERS += \
             $$CPU_HH/cCodeLabel.h \
             $$CPU_HH/cpu_defs.hh \
             $$CPU_HH/cCPUMemory.h \
             $$CPU_HH/cCPUStack.h \
             $$CPU_HH/sCPUStats.h \
             $$CPU_HH/cHardware4Stack.h \
             $$CPU_HH/cHardwareBase.h \
             $$CPU_HH/cHardwareCPU.h \
             $$CPU_HH/cHardwareFactory.h \
             $$CPU_HH/hardware_util.hh \
             $$CPU_HH/test_cpu.hh \
             $$CPU_HH/test_util.hh

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
             $$CPU_CC/hardware_util.cc \
             $$CPU_CC/memory_flags.cc \
             $$CPU_CC/test_cpu.cc \
             $$CPU_CC/test_util.cc
}
