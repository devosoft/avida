#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#define AVD_MESSAGE_DISPLAY_TYPETRACK_HH

/* XXX disables the small-object allocator; it crashes under OSX10.2.6. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
/* XXX -- kaben */
#ifndef ASSOCVECTOR_INC_
#include "third-party/Loki/AssocVector.h"
#endif
#ifndef SINGLETON_INC_
#include "third-party/Loki/Singleton.h"
#endif

#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif

#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_PROTO_HH
#include "avd_message_display_typetrack_proto.hh"
#endif

#endif

// arch-tag: header file for debug-message display-type tracking
