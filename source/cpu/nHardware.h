/*
 *  nHardware.h
 *  Avida
 *
 *  Created by David on 8/29/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef nHardware_h
#define nHardware_h

namespace nHardware {
  static const double FITNESS_NEUTRAL_RANGE = 0.0;
  static const double FITNESS_NEUTRAL_MIN = 1.0 - FITNESS_NEUTRAL_RANGE;
  static const double FITNESS_NEUTRAL_MAX = 1.0 + FITNESS_NEUTRAL_RANGE;
  
  static const int TEST_CPU_GENERATIONS = 3;  // Default generations tested for viability.
  
  static const int MAX_NOPS = 6;
  static const int MAX_LABEL_SIZE = 10;
  
  enum tHeads { HEAD_IP = 0, HEAD_READ, HEAD_WRITE, HEAD_FLOW, NUM_HEADS };
  
  static const int STACK_SIZE = 10;
  static const int IO_SIZE = 3;
}

enum tFaultTypes {
  FAULT_TYPE_WARNING = 0,
  FAULT_TYPE_FORK_TH = 0,
  FAULT_TYPE_KILL_TH = 0,
  FAULT_TYPE_ERROR
};

// Fault locations in CPU executions...
enum tFaultLocations {
  FAULT_LOC_DEFAULT = 0,
  FAULT_LOC_INSTRUCTION,
  FAULT_LOC_JUMP,
  FAULT_LOC_MATH,
  FAULT_LOC_INJECT,
  FAULT_LOC_THREAD_FORK,
  FAULT_LOC_THREAD_KILL,
  FAULT_LOC_ALLOC,
  FAULT_LOC_DIVIDE,
  FAULT_LOC_BCOPY
};

#endif
