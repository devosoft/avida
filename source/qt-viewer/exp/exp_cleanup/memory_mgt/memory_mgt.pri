
memory_mgt {
  HEADERS += \
    $$MEMORY_MGT_HH/activate_watcher_proto.hh \
    $$MEMORY_MGT_HH/activate_watcher.hh \
    $$MEMORY_MGT_HH/retainable_dbg_proto.hh \
    $$MEMORY_MGT_HH/retainable_dbg.hh \
    $$MEMORY_MGT_HH/retainable_proto.hh \
    $$MEMORY_MGT_HH/retainable.hh \
    $$MEMORY_MGT_HH/ScopeGuard.h \
    $$MEMORY_MGT_HH/watch_retains_proto.hh \
    $$MEMORY_MGT_HH/watch_retains.hh

  SOURCES += \
    $$MEMORY_MGT_CC/retainable.cc \
    $$MEMORY_MGT_CC/watch_retains.cc
}

# arch-tag: qmake include file for memory-management module
