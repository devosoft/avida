
AVIDA_SRC_DIR = ..

TEMPLATE    = app
TARGET      = qt-viewer
DESTDIR	= ../../work
CONFIG      *= qt thread
CONFIG      *= cpu event main trio tools

# These must be conditional on platform type.  If they are active under
# macx then the installation breaks.
win32 {
CONFIG	+= console
# This tells the Windows c++ compiler to treat .cc files as c++ files.
QMAKE_CXXFLAGS += -TP
# This permits multiple definitions of symbols during the final link.
QMAKE_LFLAGS_CONSOLE += /FORCE:MULTIPLE
}

OBJECTS_DIR = objs
MOC_DIR 	= moc

include($$AVIDA_SRC_DIR/modules.pri)

HEADERS +=  \
            avd_mission_control.hh \
            avd_driver_controller.hh \
            avd_avida_driver.hh \
            avd_help_controller.hh \
            avd_help_viewer.hh \
            avd_message_queue.hh \
            avd_message_queue_hdrs.hh \
            avd_plot_viewer_controller.hh \
            avd_main_window_controller.hh \
            avd_main_window.hh \
            map_view.hh \
            map_view_widget.hh \
            map_view_cell_entry.hh \
            color_scale_widget.hh \
            avd_instruction_viewer_controller.hh \
            n_orig_instruction_buttons_widget.hh \
            n_orig_instruction_cpu_widget.hh \
            n_orig_instruction_field_widget.hh \
            n_orig_instruction_line_pixmap_pair.hh \
            n_orig_instruction_line_widget.hh \
            n_orig_instruction_pixmapstructs.hh \
            n_orig_instruction_scrollview.hh \
            n_orig_instruction_scrollview_config.hh \
            n_orig_instruction_view_widget.hh \
            n_orig_instruction_viewer.hh \
            population_cell_wrapper.hh \
            population_cell_wrapped_accessors.hh \
            double_slider.hh \
            avd_gui_msg_event.hh \
            messaging_population_wrapper.hh \
            population_wrapper.hh \
	          avida_data.hh \
	          avida_plot_widget.hh \
	          plot_select_dia_impl.hh \
	          plot_widget.hh \
            avd_event_viewer_controller.hh \
            event_view_widget.hh \
            avd_godbox_controller.hh \
	          godbox_widget2.hh \
            godbox2.hh \
            apocalypse_widget.hh \
            avd_rate_validator2.hh \
            avd_lograte_widget.hh \
            avd_rate_slider2.hh \
            repopulate_widget.hh \
            \
            setup_avida.hh \
            setup_file.hh \
            setup_genesis.hh \
            setup_wizard.hh \
            setup_wizard2.hh \
            setup_wizard_config.hh \
            setup_wizard_constants.hh \
            setup_wizard_page.hh \
            setup_wizard_page2.hh \
            testwizard.hh

INTERFACES += \
            event_chooser.ui \
            event_list_editor.ui \
            plot_select_dia.ui \
            godbox.ui \
            \
            wizard.ui

SOURCES +=  \
            avd_mission_control.cc \
            avd_driver_controller.cc \
            avd_avida_driver.cc \
            avd_help_controller.cc \
            avd_help_viewer.cc \
            avd_plot_viewer_controller.cc \
            avd_main_window_controller.cc \
            avd_main_window.cc \
            map_view.cc \
            map_view_widget.cc \
            map_view_cell_entry.cc \
            color_scale_widget.cc \
            double_slider.cc \
            avd_instruction_viewer_controller.cc \
            n_orig_instruction_buttons_widget.cc \
            n_orig_instruction_cpu_widget.cc \
            n_orig_instruction_field_widget.cc \
            n_orig_instruction_line_pixmap_pair.cc \
            n_orig_instruction_line_widget.cc \
            n_orig_instruction_pixmapstructs.cc \
            n_orig_instruction_scrollview.cc \
            n_orig_instruction_scrollview_config.cc \
            n_orig_instruction_view_widget.cc \
            n_orig_instruction_viewer.cc \
            population_cell_wrapper.cc \
            population_cell_wrapped_accessors.cc \
            gui_message_display.cc \
            messaging_population_wrapper.cc \
	          avida_data.cc \
	          avida_plot_widget.cc \
	          plot_select_dia_impl.cc \
	          plot_widget.cc \
            avd_event_viewer_controller.cc \
            event_view_widget.cc \
            avd_godbox_controller.cc \
	          godbox_widget2.cc \
	          godbox2.cc \
            apocalypse_widget.cc \
            avd_rate_validator2.cc \
            avd_lograte_widget.cc \
            avd_rate_slider2.cc \
            repopulate_widget.cc \
	          main.cc \
            setup_avida.cc \
            \
            testwizard.cc \
            testwizard_loadgen.cc \
            testwizard_validators.cc \
            setup_file.cc \
            setup_genesis.cc \
            setup_wizard.cc \
            setup_wizard2.cc \
            setup_wizard_config.cc \
            setup_wizard_constants.cc \
            setup_wizard_page.cc \
            setup_wizard_page2.cc
