
ANALYZER_CC           = $$EXP_AVIDA_SRC_DIR/analyzer
ANALYZER_O_CC         = $$EXP_AVIDA_SRC_DIR/analyzer_o
CONTAINERS_CC         = $$EXP_AVIDA_SRC_DIR/containers
DUMB_GUI_CC           = $$EXP_AVIDA_SRC_DIR/dumb_gui
EXP_GUI_CC            = $$EXP_AVIDA_SRC_DIR/exp_gui
EXP_MAIN_CC           = $$EXP_AVIDA_SRC_DIR/exp_main
GUI_FACTORY_CC        = $$EXP_AVIDA_SRC_DIR/gui_factory
LOKI_CC               = $$EXP_AVIDA_SRC_DIR/third-party/Loki
MAP_GUI_CC            = $$EXP_AVIDA_SRC_DIR/map_gui
MC_CC                 = $$EXP_AVIDA_SRC_DIR/mc
MEMORY_MGT_CC         = $$EXP_AVIDA_SRC_DIR/memory_mgt
MENUBAR_CC            = $$EXP_AVIDA_SRC_DIR/menubar
PREFERENCES_CC        = $$EXP_AVIDA_SRC_DIR/preferences
PREFS_GUI_FACTORY_CC  = $$EXP_AVIDA_SRC_DIR/prefs_gui_factory
THREADED_DRIVER_CC    = $$EXP_AVIDA_SRC_DIR/threaded_driver
USER_MSG_CC           = $$EXP_AVIDA_SRC_DIR/user_msg

CPU_CC    = $$AVIDA_SRC_DIR/cpu
EVENT_CC  = $$AVIDA_SRC_DIR/event
MAIN_CC   = $$AVIDA_SRC_DIR/main
TRIO_C    = $$AVIDA_SRC_DIR/third-party/trio-1.9
TOOLS_CC  = $$AVIDA_SRC_DIR/tools
SUPPORT_CC= $$AVIDA_SRC_DIR/support
DOC_CC	  = $$AVIDA_SRC_DIR/../doc

ANALYZER_HH           = $$ANALYZER_CC
ANALYZER_O_HH         = $$ANALYZER_O_CC
CONTAINERS_HH         = $$CONTAINERS_CC
DUMB_GUI_HH           = $$DUMB_GUI_CC
EXP_GUI_HH            = $$EXP_GUI_CC
GUI_FACTORY_HH        = $$GUI_FACTORY_CC
LOKI_HH               = $$LOKI_CC
MAP_GUI_HH            = $$MAP_GUI_CC
MC_HH                 = $$MC_CC
MEMORY_MGT_HH         = $$MEMORY_MGT_CC
MENUBAR_HH            = $$MENUBAR_CC
PREFERENCES_HH        = $$PREFERENCES_CC
PREFS_GUI_FACTORY_HH  = $$PREFS_GUI_FACTORY_CC
THREADED_DRIVER_HH    = $$THREADED_DRIVER_CC
USER_MSG_HH           = $$USER_MSG_CC

CPU_HH    = $$CPU_CC
EVENT_HH  = $$EVENT_CC
MAIN_HH   = $$MAIN_CC
TRIO_H    = $$TRIO_C
TOOLS_HH  = $$TOOLS_CC
SUPPORT_HH= $$SUPPORT_CC
DOC_HH 	  = $$DOC_CC

include($$ANALYZER_CC/analyzer.pri)
include($$ANALYZER_O_CC/analyzer_o.pri)
include($$CONTAINERS_CC/containers.pri)
include($$DUMB_GUI_CC/dumb_gui.pri)
include($$EXP_GUI_CC/exp_gui.pri)
include($$GUI_FACTORY_CC/gui_factory.pri)
include($$MAP_GUI_CC/map_gui.pri)
include($$MC_CC/mc.pri)
include($$MEMORY_MGT_CC/memory_mgt.pri)
include($$MENUBAR_CC/menubar.pri)
include($$PREFERENCES_CC/preferences.pri)
include($$PREFS_GUI_FACTORY_CC/prefs_gui_factory.pri)
include($$THREADED_DRIVER_CC/threaded_driver.pri)
include($$USER_MSG_CC/user_msg.pri)

include($$CPU_CC/cpu.pri)
include($$EVENT_CC/event.pri)
include($$LOKI_CC/Loki.pri)
include($$MAIN_CC/main_sub.pri)
include($$TRIO_C/trio.pri)
include($$TOOLS_CC/tools.pri)
include($$SUPPORT_CC/support.pri)
include($$DOC_CC/doc.pri)

DEPENDPATH += ;$$AVIDA_SRC_DIR

INCLUDEPATH += $$DEPENDPATH

# arch-tag: qmake include file include other qmake module include files
