
analyzer {
  HEADERS += \
    $$ANALYZER_HH/avd_n_analyzer_ctrl.hh \
    $$ANALYZER_HH/avd_n_analyzer_batchlist_item.hh \
    $$ANALYZER_HH/avd_n_analyzer_batchlist_item_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_data_entry.hh \
    $$ANALYZER_HH/avd_n_analyzer_data_entry_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_data_entry_command_list_item.hh \
    $$ANALYZER_HH/avd_n_analyzer_data_entry_command_list_item_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_dbg_msgs.hh \
    $$ANALYZER_HH/avd_n_analyzer_dbg_msgs_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_genotype_list_item.hh \
    $$ANALYZER_HH/avd_n_analyzer_genotype_list_item_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_genotype_sel_view.hh \
    $$ANALYZER_HH/avd_n_analyzer_mdl.hh \
    $$ANALYZER_HH/avd_n_analyzer_mdl_proto.hh \
    $$ANALYZER_HH/avd_n_analyzer_menubar_handler.hh \
    $$ANALYZER_HH/avd_n_analyzer_listview.hh \
    $$ANALYZER_HH/avd_n_analyzer_out_fmt_sel.hh \
    $$ANALYZER_HH/avd_n_analyzer_view.hh

  SOURCES += \
    $$ANALYZER_CC/avd_n_analyzer_ctrl.cc \
    $$ANALYZER_CC/avd_n_analyzer_data_entry_command_list_item.cc \
    $$ANALYZER_CC/avd_n_analyzer_genotype_sel_view.cc \
    $$ANALYZER_CC/avd_n_analyzer_gui.cc \
    $$ANALYZER_HH/avd_n_analyzer_mdl.cc \
    $$ANALYZER_CC/avd_n_analyzer_menubar_handler.cc \
    $$ANALYZER_CC/avd_n_analyzer_listview.cc \
    $$ANALYZER_CC/avd_n_analyzer_out_fmt_sel.cc \
    $$ANALYZER_CC/avd_n_analyzer_view.cc
}

# arch-tag: qmake include file for analyzer module
