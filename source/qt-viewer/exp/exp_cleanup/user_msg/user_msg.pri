
user_msg {
  HEADERS += \
    $$USER_MSG_HH/avd_message_display.hh \
    $$USER_MSG_HH/avd_message_display_proto.hh \
    $$USER_MSG_HH/avd_message_display_typetrack.hh \
    $$USER_MSG_HH/avd_message_display_typetrack_proto.hh \
    $$USER_MSG_HH/avd_user_msg_gui_dbg.hh \
    $$USER_MSG_HH/avd_user_msg_gui_dbg_proto.hh \
    $$USER_MSG_HH/avd_user_msg_ctrl_funct.hh \
    $$USER_MSG_HH/avd_user_msg_ctrl_funct_proto.hh \
    $$USER_MSG_HH/avd_user_msg_ctrl.hh \
    $$USER_MSG_HH/avd_user_msg_view.hh \
    $$USER_MSG_HH/avd_user_msg_prefs_ctrl.hh \
    $$USER_MSG_HH/avd_user_msg_prefs_view.hh

  SOURCES += \
    $$USER_MSG_CC/avd_message_display.cc \
    $$USER_MSG_CC/avd_qstr_message_display.cc \
    $$USER_MSG_CC/avd_user_msg_gui.cc \
    $$USER_MSG_CC/avd_user_msg_view.cc \
    $$USER_MSG_CC/avd_user_msg_ctrl.cc \
    $$USER_MSG_CC/gui_message_display.cc \
    $$USER_MSG_CC/avd_user_msg_prefs_gui.cc \
    $$USER_MSG_CC/avd_user_msg_prefs_view.cc \
    $$USER_MSG_CC/avd_user_msg_prefs_ctrl.cc
}

# arch-tag: qmake include file for user message display module
