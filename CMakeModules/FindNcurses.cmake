#
# Find the ncurses include file and library
#

IF(WIN32)
  FIND_PATH(NCURSES_INCLUDE_PATH curses.h
    ${PROJECT_SOURCE_DIR}/source/third-party/pdcurses
  )
  FIND_LIBRARY(NCURSES_LIBRARY
    NAMES pdcurses
    PATHS ${PROJECT_SOURCE_DIR}/source/third-party/pdcurses
  )
ELSE(WIN32)
  FIND_PATH(NCURSES_INCLUDE_PATH ncurses.h
    /usr/local/include /usr/include
  )
  FIND_LIBRARY(NCURSES_LIBRARY
    NAMES ncurses
    PATHS /usr/local/lib /usr/lib /lib
  )
ENDIF(WIN32)

SET(FOO_NCURSES_INCLUDE_PATH ${NCURSES_INCLUDE_PATH})
SET(FOO_NCURSES_LIBRARY ${NCURSES_LIBRARY})

MARK_AS_ADVANCED(
  NCURSES_INCLUDE_PATH
  NCURSES_LIBRARY
)
