/*
 *  nHardwareSMT.h
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef nHardwareSMT_h
#define nHardwareSMT_h

namespace nHardwareSMT
{
  static const int NUM_LOCAL_STACKS = 3;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  
  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX };
  
  static const int NUM_NOPS = 4;
  static const int NOPX = 4;
  static const int MAX_MEMSPACE_LABEL = 3;
  
  // Performance Constants
  static const int MEM_LBLS_HASH_FACTOR = 4; // Sets hast table size to (NUM_NOPS^MAX_MEMSPACE_LABEL) / FACTOR
}

#endif
