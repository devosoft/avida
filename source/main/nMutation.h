/*
 *  nGeometry.h
 *  Avida
 *
 *  Created by David on 10/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef nMutation_h
#define nMutation_h

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
