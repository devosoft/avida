
AVIDA_SRC_DIR = ..

TEMPLATE    = app
TARGET      = primitive
DESTDIR	= ../../work
CONFIG      *= warn-on release
CONFIG      *= cpu event main trio tools
CONFIG	+= console
OBJECTS_DIR  = objs

include($$AVIDA_SRC_DIR/modules.pri)

SOURCES += primitive.cc
