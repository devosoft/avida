//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_MACROS_HH
#define MUTATION_MACROS_HH

namespace nMutation {
  enum {
    TRIGGER_NONE = 0,
    TRIGGER_UPDATE,
    TRIGGER_DIVIDE,
    TRIGGER_PARENT,
    TRIGGER_WRITE,
    TRIGGER_READ,
    TRIGGER_EXEC,
    NUM_TRIGGERS
  };

  enum {
    SCOPE_GENOME = 0,
    SCOPE_LOCAL,
    SCOPE_GLOBAL,
    SCOPE_PROP,
    SCOPE_SPREAD
  };

  enum {
    TYPE_POINT = 0,
    TYPE_INSERT,
    TYPE_DELETE,
    TYPE_HEAD_INC,
    TYPE_HEAD_DEC,
    TYPE_TEMP,
    TYPE_KILL
  };
}

#endif
