INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)

IF(AVD_GUI_NCURSES)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${NCURSES_INCLUDE_PATH}
  )

  SET(libviewer_SOURCES
    ansi.cc
    bar_screen.cc
    environment_screen.cc
    hist_screen.cc
    map_screen.cc
    menu.cc
    ncurses.cc
    options_screen.cc
    stats_screen.cc
    symbol_util.cc
    text_screen.cc
    view.cc
    zoom_screen.cc
  )
  ADD_LIBRARY(viewer ${libviewer_SOURCES})

  ADD_EXECUTABLE(AvidaConsole viewer.cc)
  IF(WIN32)
    TARGET_LINK_LIBRARIES(AvidaConsole
      viewer
      main
      cpu
      event
      main
      tools
      trio
      ${NCURSES_LIBRARY}
    )
  ELSE(WIN32)
    TARGET_LINK_LIBRARIES(AvidaConsole
      viewer
      main
      cpu
      event
      main
      tools
      trio
      ${NCURSES_LIBRARY}
      m
    )
  ENDIF(WIN32)
  LINK_DIRECTORIES(${ALL_LIB_DIRS} ${THIRDPARTY_BLD_DIR}/trio)

  INSTALL_TARGETS(/work AvidaConsole)
ENDIF(AVD_GUI_NCURSES)
