
analyzer_o {
  HEADERS += \
    $$ANALYZER_O_HH/avd_analyzer_batchlist_item_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_batchlist_item.hh \
    $$ANALYZER_O_HH/avd_analyzer_controller_data.hh \
    $$ANALYZER_O_HH/avd_analyzer_controller.hh \
    $$ANALYZER_O_HH/avd_analyzer_data_entry_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_data_entry.hh \
    $$ANALYZER_O_HH/avd_analyzer_data_entry_command_list_item_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_data_entry_command_list_item.hh \
    $$ANALYZER_O_HH/avd_analyzer_dbg_msgs_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_dbg_msgs.hh \
    $$ANALYZER_O_HH/avd_analyzer_genotype_list_item_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_genotype_list_item.hh \
    $$ANALYZER_O_HH/avd_analyzer_listview_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_listview.hh \
    $$ANALYZER_O_HH/avd_analyzer_menubar_handler.hh \
    $$ANALYZER_O_HH/avd_analyzer_model_proto.hh \
    $$ANALYZER_O_HH/avd_analyzer_model.hh \
    $$ANALYZER_O_HH/avd_analyzer_output_fmt_selector.hh \
    $$ANALYZER_O_HH/avd_analyzer_utils.hh \
    $$ANALYZER_O_HH/avd_analyzer_view.hh

  SOURCES += \
    $$ANALYZER_O_CC/avd_analyzer_controller_data.cc \
    $$ANALYZER_O_CC/avd_analyzer_controller.cc \
    $$ANALYZER_O_CC/avd_analyzer_data_entry_command_list_item.cc \
    $$ANALYZER_O_CC/avd_analyzer_dbg_msgs.cc \
    $$ANALYZER_O_CC/avd_analyzer_listview.cc \
    $$ANALYZER_O_CC/avd_analyzer_menubar_handler.cc \
    $$ANALYZER_O_CC/avd_analyzer_model.cc \
    $$ANALYZER_O_CC/avd_analyzer_output_fmt_selector.cc \
    $$ANALYZER_O_CC/avd_analyzer_view.cc
}

# arch-tag: qmake include file for old analyzer module
