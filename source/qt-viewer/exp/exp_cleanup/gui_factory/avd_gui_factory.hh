#ifndef AVD_GUI_FACTORY_HH
#define AVD_GUI_FACTORY_HH


#ifndef QSTRING_H
#include <qstring.h>
#endif

/* XXX disables the small-object allocator; it crashes under OSX10.2.6. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
/* XXX -- kaben */
#ifndef FUNCTOR_INC_
#include "third-party/Loki/Functor.h"
#endif
#ifndef FACTORY_INC_
#include "third-party/Loki/Factory.h"
#endif
#ifndef SINGLETON_INC_
#include "third-party/Loki/Singleton.h"
#endif

#ifndef AVD_ABSTRACT_GUI_HH
#include "avd_abstract_gui.hh"
#endif
#ifndef AVD_GUI_FACTORY_ERROR_POLICY_HH
#include "avd_gui_factory_error_policy.hh"
#endif

#ifndef AVD_GUI_FACTORY_PROTO_HH
#include "avd_gui_factory_proto.hh"
#endif

#endif

// arch-tag: header file for generic gui object factory
