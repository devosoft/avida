
cpu {
  HEADERS += \
             $$CPU_HH/cCodeLabel.h \
             $$CPU_HH/cpu_defs.hh \
             $$CPU_HH/cCPUMemory.h \
             $$CPU_HH/cCPUStack.h \
             $$CPU_HH/sCPUStats.h \
             $$CPU_HH/hardware_4stack.hh \
             $$CPU_HH/hardware_base.hh \
             $$CPU_HH/hardware_cpu.hh \
             $$CPU_HH/hardware_factory.hh \
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
             $$CPU_CC/hardware_4stack.cc \
             $$CPU_CC/hardware_4stack_thread.cc \
             $$CPU_CC/hardware_base.cc \
             $$CPU_CC/hardware_cpu.cc \
             $$CPU_CC/hardware_cpu_thread.cc \
             $$CPU_CC/hardware_factory.cc \
             $$CPU_CC/hardware_util.cc \
             $$CPU_CC/memory_flags.cc \
             $$CPU_CC/test_cpu.cc \
             $$CPU_CC/test_util.cc
}
