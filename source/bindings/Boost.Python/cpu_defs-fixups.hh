#ifndef CPU_DEFS_FIXUPS_HH
#define CPU_DEFS_FIXUPS_HH

struct cCPUDefs {
  static const int s_TEST_CPU_GENERATIONS;

  static const int s_MAX_TEST_DEPTH;

  static const double s_FITNESS_NEUTRAL_RANGE;
  static const double s_FITNESS_NEUTRAL_MIN;
  static const double s_FITNESS_NEUTRAL_MAX;

  static const int s_NUM_NOPS;
  static const int s_MAX_NOPS;
  static const int s_MAX_LABEL_SIZE;
  static const int s_REG_AX;
  static const int s_REG_BX;
  static const int s_REG_CX;
  static const int s_REG_DX;
  static const int s_NUM_REGISTERS;
  static const int s_NUM_REG_4STACK;

  static const int s_HEAD_IP;
  static const int s_HEAD_READ;
  static const int s_HEAD_WRITE;
  static const int s_HEAD_FLOW;
  static const int s_NUM_HEADS;

  static const int s_STACK_SIZE;
  static const int s_IO_SIZE;

  static const int s_GEN_RESOURCE;
  static const int s_NUM_RESOURCES;

  static const int s_FAULT_TYPE_WARNING;
  static const int s_FAULT_TYPE_ERROR;

  static const int s_FAULT_TYPE_FORK_TH;
  static const int s_FAULT_TYPE_KILL_TH;

  static const int s_FAULT_LOC_DEFAULT;
  static const int s_FAULT_LOC_INSTRUCTION;
  static const int s_FAULT_LOC_JUMP;
  static const int s_FAULT_LOC_MATH;
  static const int s_FAULT_LOC_INJECT;
  static const int s_FAULT_LOC_THREAD_FORK;
  static const int s_FAULT_LOC_THREAD_KILL;
  static const int s_FAULT_LOC_ALLOC;
  static const int s_FAULT_LOC_DIVIDE;
  static const int s_FAULT_LOC_BCOPY;
};

#endif
