
cpu {
  HEADERS += \
             $$CPU_HH/code_label.hh \
             $$CPU_HH/cpu_defs.hh \
             $$CPU_HH/cpu_memory.hh \
             $$CPU_HH/cpu_stack.hh \
             $$CPU_HH/cpu_stats.hh \
             $$CPU_HH/hardware_4stack.hh \
             $$CPU_HH/hardware_base.hh \
             $$CPU_HH/hardware_cpu.hh \
             $$CPU_HH/hardware_factory.hh \
             $$CPU_HH/hardware_util.hh \
             $$CPU_HH/test_cpu.hh \
             $$CPU_HH/test_util.hh

  SOURCES += \
             $$CPU_CC/4stack_head.cc \
             $$CPU_CC/code_label.cc \
             $$CPU_CC/cpu_head.cc \
             $$CPU_CC/cpu_memory.cc \
             $$CPU_CC/cpu_stack.cc \
             $$CPU_CC/cpu_test_info.cc \
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
