
AVIDA_SRC_DIR = ..

TEMPLATE    = app
TARGET      = avida
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

SOURCES += main.cc
LIBS += -L. -lqtviewer
