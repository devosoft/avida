
mc {
  HEADERS += \
    $$MC_HH/avd_avida_state_mediator.hh \
    $$MC_HH/avd_connections.hh \
    $$MC_HH/avd_gui_factory_mediator.hh \
    $$MC_HH/avd_menubar_mediator.hh \
    $$MC_HH/avd_settings_mediator.hh \
    $$MC_HH/avd_windows_mediator.hh \
    $$MC_HH/avd_mc_dbg_proto.hh \
    $$MC_HH/avd_mc_dbg.hh \
    $$MC_HH/avd_mc_mediator.hh \
    $$MC_HH/avd_mission_control.hh \
    $$MC_HH/setup_avida.hh \
    \
    $$MC_HH/attributes/requests_argv.hh

  SOURCES += \
    $$MC_CC/avd_connections.cc \
    $$MC_CC/avd_mc_dbg.cc \
    $$MC_CC/avd_mc_mediator.cc \
    $$MC_CC/avd_mission_control.cc \
    $$MC_CC/setup_avida.cc
}

# arch-tag: qmake include file for misson-control module
