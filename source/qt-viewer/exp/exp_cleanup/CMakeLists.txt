INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FixFindQt.cmake)

IF(_QT_EXPERIMENTAL_GRAPHIC_INTERFACE)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${THIRDPARTY_SRC_DIR}/trio
    ${THIRDPARTY_SRC_DIR}/Loki
    ${QT_INCLUDE_DIR}
    ${QT_INCLUDE_PATH}
    .
  )
  LINK_DIRECTORIES(
    ${ALL_LIB_DIRS}
    ${THIRDPARTY_BLD_DIR}/trio
    .
  )
  
  SET(libxqtviewer_a_SOURCES
#    analyzer/avd_n_analyzer_ctrl.cc
#    analyzer/avd_n_analyzer_data_entry_command_list_item.cc
#    analyzer/avd_n_analyzer_genotype_sel_view.cc
#    analyzer/avd_n_analyzer_gui.cc
#    analyzer/avd_n_analyzer_listview.cc
#    analyzer/avd_n_analyzer_mdl.cc
#    analyzer/avd_n_analyzer_menubar_handler.cc
#    analyzer/avd_n_analyzer_out_fmt_sel.cc
#    analyzer/avd_n_analyzer_view.cc
#    analyzer_o/avd_analyzer_controller.cc
#    analyzer_o/avd_analyzer_controller_data.cc
#    analyzer_o/avd_analyzer_creator.cc
#    analyzer_o/avd_analyzer_data_entry_command_list_item.cc
#    analyzer_o/avd_analyzer_dbg_msgs.cc
#    analyzer_o/avd_analyzer_listview.cc
#    analyzer_o/avd_analyzer_menubar_handler.cc
#    analyzer_o/avd_analyzer_model.cc
#    analyzer_o/avd_analyzer_output_fmt_selector.cc
#    analyzer_o/avd_analyzer_view.cc
#    dumb_gui/avd_dumb_ctrl.cc
#    dumb_gui/avd_dumb_gui.cc
#    dumb_gui/avd_dumb_view.cc
#    exp_gui/avd_exp_ctrl.cc
#    exp_gui/avd_exp_gui.cc
#    exp_gui/avd_exp_view.cc
#    exp_gui/test_thing.cc
#    gui_factory/avd_gui_factory.cc
#    map_gui/avd_map_canvas.cc
#    map_gui/avd_map_canvas_rectangle.cc
#    map_gui/avd_map_canvas_view.cc
#    map_gui/avd_map_ctrl.cc
#    map_gui/avd_map_gui.cc
#    map_gui/avd_map_layer.cc
#    map_gui/avd_map_view.cc
#    mc/avd_connections.cc
#    mc/avd_mc_dbg.cc
#    mc/avd_mc_mediator.cc
#    mc/avd_mission_control.cc
#    mc/setup_avida.cc
    memory_mgt/retainable.cc
    memory_mgt/watch_retains.cc
#    menubar/avd_menubar_handler.cc
#    menubar/avd_menubar_hdlr_creator.cc
#    preferences/avd_prefs_ctrl.cc
#    preferences/avd_prefs_dbg.cc
#    preferences/avd_prefs_gui.cc
#    preferences/avd_prefs_item.cc
#    preferences/avd_prefs_view.cc
#    prefs_gui_factory/avd_prefs_gui_factory.cc
#    threaded_driver/avd_avida_driver.cc
#    threaded_driver/avd_driver_creator.cc
#    threaded_driver/avd_orig_driver_controller.cc
#    user_msg/avd_message_display.cc
#    user_msg/avd_qstr_message_display.cc
#    user_msg/avd_user_msg_ctrl.cc
#    user_msg/avd_user_msg_gui.cc
#    user_msg/avd_user_msg_prefs_ctrl.cc
#    user_msg/avd_user_msg_prefs_gui.cc
#    user_msg/avd_user_msg_prefs_view.cc
#    user_msg/avd_user_msg_view.cc
#    user_msg/gui_message_display.cc
  )
  
  SET(libxqtviewer_moc_a_MOC_SOURCES
#    analyzer/avd_n_analyzer_batchlist_item.hh
#    analyzer/avd_n_analyzer_batchlist_item_proto.hh
#    analyzer/avd_n_analyzer_ctrl.hh
#    analyzer/avd_n_analyzer_data_entry.hh
#    analyzer/avd_n_analyzer_data_entry_command_list_item.hh
#    analyzer/avd_n_analyzer_data_entry_command_list_item_proto.hh
#    analyzer/avd_n_analyzer_data_entry_proto.hh
#    analyzer/avd_n_analyzer_dbg_msgs.hh
#    analyzer/avd_n_analyzer_dbg_msgs_proto.hh
#    analyzer/avd_n_analyzer_genotype_list_item.hh
#    analyzer/avd_n_analyzer_genotype_list_item_proto.hh
#    analyzer/avd_n_analyzer_genotype_sel_ctrl.hh
#    analyzer/avd_n_analyzer_genotype_sel_view.hh
#    analyzer/avd_n_analyzer_listview.hh
#    analyzer/avd_n_analyzer_mdl.hh
#    analyzer/avd_n_analyzer_mdl_proto.hh
#    analyzer/avd_n_analyzer_menubar_handler.hh
#    analyzer/avd_n_analyzer_out_fmt_sel.hh
#    analyzer/avd_n_analyzer_view.hh
#    analyzer_o/avd_analyzer_batchlist_item.hh
#    analyzer_o/avd_analyzer_batchlist_item_proto.hh
#    analyzer_o/avd_analyzer_controller.hh
#    analyzer_o/avd_analyzer_controller_data.hh
#    analyzer_o/avd_analyzer_creator.hh
#    analyzer_o/avd_analyzer_data_entry.hh
#    analyzer_o/avd_analyzer_data_entry_command_list_item.hh
#    analyzer_o/avd_analyzer_data_entry_command_list_item_proto.hh
#    analyzer_o/avd_analyzer_data_entry_proto.hh
#    analyzer_o/avd_analyzer_dbg_msgs.hh
#    analyzer_o/avd_analyzer_dbg_msgs_proto.hh
#    analyzer_o/avd_analyzer_genotype_list_item.hh
#    analyzer_o/avd_analyzer_genotype_list_item_proto.hh
#    analyzer_o/avd_analyzer_listview.hh
#    analyzer_o/avd_analyzer_listview_proto.hh
#    analyzer_o/avd_analyzer_menubar_handler.hh
#    analyzer_o/avd_analyzer_model.hh
#    analyzer_o/avd_analyzer_model_proto.hh
#    analyzer_o/avd_analyzer_output_fmt_selector.hh
#    analyzer_o/avd_analyzer_utils.hh
#    analyzer_o/avd_analyzer_view.hh
#    containers/tRetainable_list.hh
#    containers/tRetainable_list_proto.hh
#    containers/tRList.hh
#    containers/tRList_proto.hh
#    dumb_gui/avd_dumb_ctrl.hh
#    dumb_gui/avd_dumb_gui_dbg.hh
#    dumb_gui/avd_dumb_gui_dbg_proto.hh
#    dumb_gui/avd_dumb_view.hh
#    exp_gui/avd_exp_ctrl.hh
#    exp_gui/avd_exp_gui_dbg.hh
#    exp_gui/avd_exp_gui_dbg_proto.hh
#    exp_gui/avd_exp_view.hh
#    exp_gui/test_thing.hh
#    exp_gui/test_thing_proto.hh
#    exp_gui/testing_interface.hh
#    gui_factory/avd_abstract_gui.hh
#    gui_factory/avd_abstract_gui_proto.hh
#    gui_factory/avd_gui_factory.hh
#    gui_factory/avd_gui_factory_error_policy.hh
#    gui_factory/avd_gui_factory_proto.hh
#    gui_factory/tGUI.hh
#    gui_factory/tGUI_proto.hh
#    map_gui/avd_map_canvas.hh
#    map_gui/avd_map_canvas_rectangle.hh
#    map_gui/avd_map_canvas_rectangle_proto.hh
#    map_gui/avd_map_canvas_view.hh
#    map_gui/avd_map_ctrl.hh
#    map_gui/avd_map_gui_dbg.hh
#    map_gui/avd_map_gui_dbg_proto.hh
#    map_gui/avd_map_layer.hh
#    map_gui/avd_map_layer_proto.hh
#    map_gui/avd_map_view.hh
#    mc/attributes/requests_argv.hh
#    mc/avd_avida_state_mediator.hh
#    mc/avd_connections.hh
#    mc/avd_gui_factory_mediator.hh
#    mc/avd_mc_dbg.hh
#    mc/avd_mc_dbg_proto.hh
#    mc/avd_mc_mediator.hh
#    mc/avd_menubar_mediator.hh
#    mc/avd_mission_control.hh
#    mc/avd_settings_mediator.hh
#    mc/avd_windows_mediator.hh
#    mc/setup_avida.hh
    memory_mgt/activate_watcher.hh
    memory_mgt/activate_watcher_proto.hh
    memory_mgt/retainable.hh
    memory_mgt/retainable_dbg.hh
    memory_mgt/retainable_dbg_proto.hh
    memory_mgt/retainable_proto.hh
    memory_mgt/watch_retains.hh
    memory_mgt/watch_retains_proto.hh
#    menubar/avd_menubar_handler.hh
#    preferences/avd_prefs_ctrl.hh
#    preferences/avd_prefs_dbg.hh
#    preferences/avd_prefs_dbg_proto.hh
#    preferences/avd_prefs_item.hh
#    preferences/avd_prefs_item_proto.hh
#    preferences/avd_prefs_item_view.hh
#    preferences/avd_prefs_item_view_proto.hh
#    preferences/avd_prefs_view.hh
#    prefs_gui_factory/avd_abstract_prefs_gui.hh
#    prefs_gui_factory/avd_abstract_prefs_gui_proto.hh
#    prefs_gui_factory/avd_prefs_gui_factory.hh
#    prefs_gui_factory/avd_prefs_gui_factory_error_policy.hh
#    prefs_gui_factory/avd_prefs_gui_factory_proto.hh
#    prefs_gui_factory/tPrefsGUI.hh
#    prefs_gui_factory/tPrefsGUI_proto.hh
#    threaded_driver/avd_avida_driver.hh
#    threaded_driver/avd_avida_thread_drvr_dbg.hh
#    threaded_driver/avd_avida_thread_drvr_dbg_proto.hh
#    threaded_driver/avd_gui_msg_event.hh
#    threaded_driver/avd_orig_driver_controller.hh
#    user_msg/avd_message_display.hh
#    user_msg/avd_message_display_proto.hh
#    user_msg/avd_message_display_typetrack.hh
#    user_msg/avd_message_display_typetrack_proto.hh
#    user_msg/avd_user_msg_ctrl.hh
#    user_msg/avd_user_msg_ctrl_funct.hh
#    user_msg/avd_user_msg_ctrl_funct_proto.hh
#    user_msg/avd_user_msg_gui_dbg.hh
#    user_msg/avd_user_msg_gui_dbg_proto.hh
#    user_msg/avd_user_msg_prefs_ctrl.hh
#    user_msg/avd_user_msg_prefs_view.hh
#    user_msg/avd_user_msg_view.hh
  )
  
#  SET(libxqtviewer_ui_a_UI_SOURCES
#  )
  
  QT_WRAP_CPP(xqtviewer_moc
    libxqtviewer_moc_a_SOURCES
    libxqtviewer_moc_a_MOC_SOURCES
  )
#  QT_WRAP_UI(xqtviewer_ui
#    libxqtviewer_ui_a_HEADERS
#    libxqtviewer_ui_a_SOURCES
#    libxqtviewer_ui_a_UI_SOURCES
#  )
  ADD_LIBRARY(xqtviewer_moc ${libxqtviewer_moc_a_SOURCES})
#  ADD_LIBRARY(xqtviewer_ui ${libxqtviewer_ui_a_SOURCES})
  ADD_LIBRARY(xqtviewer ${libxqtviewer_a_SOURCES})
  
  SET(Avida_SOURCES main.cc)
  SET(Avida_LIBRARIES
    xqtviewer
    main
    cpu
    event
    main
    tools
    trio
    xqtviewer_moc
#    xqtviewer_ui
    ${QT_LIBRARIES}
  )
#  CREATE_QT_EXECUTABLE(Avida Avida_SOURCES Avida_LIBRARIES /)
ENDIF(_QT_EXPERIMENTAL_GRAPHIC_INTERFACE)

SET(EXECUTABLE_OUTPUT_PATH "")

IF(_UNIT_TEST_QT_EXP_GUI AND _QT_EXPERIMENTAL_GRAPHIC_INTERFACE)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${GUI_SRC_DIR}
    ${THIRDPARTY_SRC_DIR}/Loki
    ${THIRDPARTY_SRC_DIR}/trio
    ${THIRDPARTY_SRC_DIR}/yaktest
    ${QT_INCLUDE_DIR}
    ${QT_INCLUDE_PATH}
    .
  )
  LINK_DIRECTORIES(
    ${ALL_LIB_DIRS}
    ${THIRDPARTY_BLD_DIR}/trio
    ${THIRDPARTY_BLD_DIR}/yaktest
    .
  )

  SET(TESTSUITES
  	template_t
  	template2_t
  ) 
  STRING(REGEX REPLACE _t _t.hh\; TESTSUITE_MOCSRCS ${TESTSUITES})
  SET(libxqtunittest_a_SOURCES
    xqt_test_case.cc
  )
  SET(libxqtunittest_moc_a_MOC_SOURCES
    xqt_test_case.hh
    ${TESTSUITE_MOCSRCS}
  )
  QT_WRAP_CPP(xqtunittest_moc
    libxqtunittest_moc_a_SOURCES
    libxqtunittest_moc_a_MOC_SOURCES
  )
  ADD_LIBRARY(xqtunittest_moc ${libxqtunittest_moc_a_SOURCES})
  ADD_LIBRARY(xqtunittest ${libxqtunittest_a_SOURCES})

  FOREACH(TESTSUITE ${TESTSUITES})
    ADD_EXECUTABLE(${TESTSUITE} ${TESTSUITE}.cc)
    TARGET_LINK_LIBRARIES(${TESTSUITE}
      xqtviewer
      main
      cpu
      event
      main
      tools
      yaktest
      xqtviewer_moc
#      xqtviewer_ui
      xqtunittest
      xqtunittest_moc
      ${QT_LIBRARIES}
    )
    ADD_TEST(${TESTSUITE} ${TESTSUITE})
  ENDFOREACH(TESTSUITE)
ENDIF(_UNIT_TEST_QT_EXP_GUI AND _QT_EXPERIMENTAL_GRAPHIC_INTERFACE)

