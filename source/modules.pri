
CPU_CC    = $$AVIDA_SRC_DIR/cpu
EVENT_CC  = $$AVIDA_SRC_DIR/event
MAIN_CC   = $$AVIDA_SRC_DIR/main
TRIO_C    = $$AVIDA_SRC_DIR/third-party/trio
TOOLS_CC  = $$AVIDA_SRC_DIR/tools
SUPPORT_CC= $$AVIDA_SRC_DIR/support
DOC_CC	  = $$AVIDA_SRC_DIR/../doc

CPU_HH    = $$CPU_CC
EVENT_HH  = $$EVENT_CC
MAIN_HH   = $$MAIN_CC
TRIO_H    = $$TRIO_C
TOOLS_HH  = $$TOOLS_CC
SUPPORT_HH= $$SUPPORT_CC
DOC_HH 	  = $$DOC_CC

DEPENDPATH += ;$$AVIDA_SRC_DIR
DEPENDPATH += ;$$CPU_HH
DEPENDPATH += ;$$EVENT_HH
DEPENDPATH += ;$$MAIN_HH
DEPENDPATH += ;$$TRIO_H
DEPENDPATH += ;$$TOOLS_HH
DEPENDPATH += ;$$SUPPORT_HH
DEPENDPATH += ;$$DOC_HH
DEPENDPATH += ;.

INCLUDEPATH += $$DEPENDPATH
QMAKE_CXXFLAGS += -I.

include($$CPU_CC/cpu.pri)
include($$EVENT_CC/event.pri)
include($$MAIN_CC/main_sub.pri)
include($$TRIO_C/trio.pri)
include($$TOOLS_CC/tools.pri)
#include($$SUPPORT_CC/support.pri)
#include($$DOC_CC/doc.pri)
