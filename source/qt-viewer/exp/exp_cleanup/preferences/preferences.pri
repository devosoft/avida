
preferences {
  HEADERS += \
    $$PREFERENCES_HH/avd_prefs_dbg_proto.hh \
    $$PREFERENCES_HH/avd_prefs_dbg.hh \
    $$PREFERENCES_HH/avd_prefs_ctrl.hh \
    $$PREFERENCES_HH/avd_prefs_view.hh \
    $$PREFERENCES_HH/avd_prefs_item_proto.hh \
    $$PREFERENCES_HH/avd_prefs_item_view_proto.hh \
    $$PREFERENCES_HH/avd_prefs_item_view.hh \
    $$PREFERENCES_HH/avd_prefs_item.hh

  SOURCES += \
    $$PREFERENCES_CC/avd_prefs_ctrl.cc \
    $$PREFERENCES_CC/avd_prefs_dbg.cc \
    $$PREFERENCES_CC/avd_prefs_gui.cc \
    $$PREFERENCES_CC/avd_prefs_view.cc \
    $$PREFERENCES_CC/avd_prefs_item.cc
}

# arch-tag: qmake include file for preferences module
