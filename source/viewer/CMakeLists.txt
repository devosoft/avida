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
  ADD_LIBRARY(cursesviewer ${libviewer_SOURCES})

  ADD_EXECUTABLE(viewer viewer.cc)
  TARGET_LINK_LIBRARIES(viewer
    cursesviewer
    main
    classification
    cpu
    event
    analyze
    cpu
    main
    tools
    trio
    ${NCURSES_LIBRARY}
  )
  LINK_DIRECTORIES(${ALL_LIB_DIRS} ${THIRDPARTY_BLD_DIR}/trio)

  INSTALL_TARGETS(/work viewer)
ENDIF(AVD_GUI_NCURSES)
