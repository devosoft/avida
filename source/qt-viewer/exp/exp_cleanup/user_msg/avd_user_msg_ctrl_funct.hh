#ifndef AVD_USER_MSG_CTRL_FUNCT_HH
#define AVD_USER_MSG_CTRL_FUNCT_HH

/* XXX disables the small-object allocator; it crashes under OSX10.2.6. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
/* XXX -- kaben */
#ifndef FUNCTOR_INC_
#include "third-party/Loki/Functor.h"
#endif

#ifndef AVD_USER_MSG_CTRL_FUNCT_PROTO_HH
#include "avd_user_msg_ctrl_funct_proto.hh"
#endif

#endif

// arch-tag: header file for user debug-message display controller functors
