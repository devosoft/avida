
TEMPLATE = app
TARGET = exp_cleanup
DESTDIR = ../../../../work

# These options configure Qt. We rely on Qt graphics and multithreading,
# so 'qt' and 'thread' are required.
#CONFIG *= qt thread debug
CONFIG *= qt thread
# Custom-options that are defined in this file.
#CONFIG *= gprof
# These are required modules from avida proper (the core source code).
CONFIG *= cpu event loki main trio tools
# These modules are needed by the user interface.
CONFIG *= user_msg containers gui_factory mc memory_mgt menubar
CONFIG *= threaded_driver preferences prefs_gui_factory
# These are optional user interface elements.
CONFIG *= analyzer analyzer_o dumb_gui exp_gui map_gui

# These must be conditional on platform type.  If they are active under
# MacOSX then the installation breaks.
win32 {
CONFIG	+= console
QMAKE_LFLAGS_CONSOLE += /FORCE:MULTIPLE
}

# The 'Loki' template library lets you use long-double data types, but
# some compilers warn that the size of long-double can change.  This
# disables the warning under MacOSX.
macx {
QMAKE_CXXFLAGS_THREAD += -Wno-long-double
}

# Asks compiler and linker to generate extra code to write profile
# information suitable for the analysis program "gprof".
gprof {
QMAKE_CXXFLAGS_THREAD += -pg
QMAKE_LFLAGS_THREAD += -pg
}

# This tells qmake where to store intermediate code (to keep your source
# tree clean).
MOC_DIR = moc
OBJECTS_DIR = objs
UI_DIR = ui

# These are paths to qmake-include files that tell qmake where to find
# source code for the various software modules.
#AVIDA_SRC_DIR = ../current/source
AVIDA_SRC_DIR = ../../..
EXP_AVIDA_SRC_DIR = .
include($$EXP_AVIDA_SRC_DIR/modules.pri)


HEADERS +=
INTERFACES +=
SOURCES += main.cc

# arch-tag: qmake file for qt-viewer
