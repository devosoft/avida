
gui_factory {
  HEADERS += \
    $$GUI_FACTORY_HH/avd_abstract_gui_proto.hh \
    $$GUI_FACTORY_HH/avd_abstract_gui.hh \
    $$GUI_FACTORY_HH/avd_gui_factory_error_policy.hh \
    $$GUI_FACTORY_HH/avd_gui_factory_proto.hh \
    $$GUI_FACTORY_HH/avd_gui_factory.hh \
    $$GUI_FACTORY_HH/tGUI_proto.hh \
    $$GUI_FACTORY_HH/tGUI.hh

  SOURCES += \
    $$GUI_FACTORY_CC/avd_gui_factory.cc
}

# arch-tag: qmake include file for generic gui factory module
