
exp_gui {
  HEADERS += \
    $$EXP_GUI_HH/avd_exp_ctrl.hh \
    $$EXP_GUI_HH/avd_exp_gui_dbg_proto.hh \
    $$EXP_GUI_HH/avd_exp_gui_dbg.hh \
    $$EXP_GUI_HH/avd_exp_view.hh \
    $$EXP_GUI_HH/test_thing_proto.hh \
    $$EXP_GUI_HH/test_thing.hh \
    $$EXP_GUI_HH/testing_interface.hh

  SOURCES += \
    $$EXP_GUI_CC/avd_exp_ctrl.cc \
    $$EXP_GUI_CC/avd_exp_gui.cc \
    $$EXP_GUI_CC/avd_exp_view.cc \
    $$EXP_GUI_CC/test_thing.cc \
}

# arch-tag: qmake include file for experimental gui module
