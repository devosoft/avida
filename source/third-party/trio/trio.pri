
trio {
  HEADERS += $$TRIO_H/strio.h \
             $$TRIO_H/trio.h \
             $$TRIO_H/triodef.h \
             $$TRIO_H/trionan.h \
             $$TRIO_H/triop.h \
             $$TRIO_H/triostr.h

  SOURCES += $$TRIO_C/trio.c \
             $$TRIO_C/trionan.c \
             $$TRIO_C/triostr.c
}
