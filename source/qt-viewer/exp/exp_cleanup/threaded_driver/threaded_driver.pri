
threaded_driver {
  HEADERS += \
    $$THREADED_DRIVER_HH/avd_avida_driver.hh \
    $$THREADED_DRIVER_HH/avd_avida_thread_drvr_dbg_proto.hh \
    $$THREADED_DRIVER_HH/avd_avida_thread_drvr_dbg.hh \
    $$THREADED_DRIVER_HH/avd_gui_msg_event.hh \
    $$THREADED_DRIVER_HH/avd_orig_driver_controller.hh

  SOURCES += \
    $$THREADED_DRIVER_CC/avd_avida_driver.cc \
    $$THREADED_DRIVER_CC/avd_driver_creator.cc \
    $$THREADED_DRIVER_CC/avd_orig_driver_controller.cc
}

# arch-tag: qmake include file for avida thread module
