SET(libcpu_a_SOURCES
  code_label.cc
  cpu_memory.cc
  cpu_stack.cc
  cpu_test_info.cc
  hardware_4stack.cc
  hardware_4stack_thread.cc
  hardware_base.cc
  hardware_cpu.cc
  hardware_cpu_thread.cc
  hardware_factory.cc
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

