//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_MACROS_HH
#define MUTATION_MACROS_HH

#define MUTATION_TRIGGER_NONE     0
#define MUTATION_TRIGGER_UPDATE   1
#define MUTATION_TRIGGER_DIVIDE   2
#define MUTATION_TRIGGER_PARENT   3
#define MUTATION_TRIGGER_WRITE    4
#define MUTATION_TRIGGER_READ     5
#define MUTATION_TRIGGER_EXEC     6
#define NUM_MUTATION_TRIGGERS     7

#define MUTATION_SCOPE_GENOME     0
#define MUTATION_SCOPE_LOCAL      1
#define MUTATION_SCOPE_GLOBAL     2
#define MUTATION_SCOPE_PROP       3
#define MUTATION_SCOPE_SPREAD     4

#define MUTATION_TYPE_POINT       0
#define MUTATION_TYPE_INSERT      1
#define MUTATION_TYPE_DELETE      2
#define MUTATION_TYPE_HEAD_INC    3
#define MUTATION_TYPE_HEAD_DEC    4
#define MUTATION_TYPE_TEMP        5
#define MUTATION_TYPE_KILL        6

#endif
