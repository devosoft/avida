INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)

IF(AVD_GUI_NCURSES)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${NCURSES_INCLUDE_PATH}
  )

  SET(libviewer_SOURCES
    cAvidaDriver_TextPopViewer.cc
    cBarScreen.cc
    cEnvironmentScreen.cc
    cHistScreen.cc
    cMapScreen.cc
    cMenuWindow.cc
    cOptionsScreen.cc
    cScreen.cc
    cStatsScreen.cc
    cSymbolUtil.cc
    cTextWindow.cc
    cView.cc
    cViewInfo.cc
    cZoomScreen.cc
  )
  ADD_LIBRARY(viewer ${libviewer_SOURCES})

  ADD_EXECUTABLE(viewer viewer.cc)
  IF(WIN32)
    TARGET_LINK_LIBRARIES(viewer
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
    TARGET_LINK_LIBRARIES(viewer
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

  INSTALL_TARGETS(/work viewer)
ENDIF(AVD_GUI_NCURSES)
