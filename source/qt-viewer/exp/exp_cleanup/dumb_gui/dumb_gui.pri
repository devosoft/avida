
dumb_gui {
  HEADERS += \
    $$DUMB_GUI_HH/avd_dumb_ctrl.hh \
    $$DUMB_GUI_HH/avd_dumb_gui_dbg_proto.hh \
    $$DUMB_GUI_HH/avd_dumb_gui_dbg.hh \
    $$DUMB_GUI_HH/avd_dumb_view.hh

  SOURCES += \
    $$DUMB_GUI_CC/avd_dumb_ctrl.cc \
    $$DUMB_GUI_CC/avd_dumb_gui.cc \
    $$DUMB_GUI_CC/avd_dumb_view.cc
}

# arch-tag: qmake include file for dumb gui module
