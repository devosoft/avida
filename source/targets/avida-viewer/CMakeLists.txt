INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)

INCLUDE_DIRECTORIES(
  ${ALL_INC_DIRS}
  ${NCURSES_INCLUDE_PATH}
)

SET(libviewer_SOURCES
  cBarScreen.cc
  cEnvironmentScreen.cc
  cHistScreen.cc
  cMapScreen.cc
  cMenuWindow.cc
  cOptionsScreen.cc
  cScreen.cc
  cStatsScreen.cc
  cSymbolUtil.cc
  cTextViewerDriver.cc
  cTextWindow.cc
  cView.cc
  cViewInfo.cc
  cZoomScreen.cc
)
ADD_LIBRARY(cursesviewer ${libviewer_SOURCES})

ADD_EXECUTABLE(avida-viewer viewer.cc)
IF(MSVC)
TARGET_LINK_LIBRARIES(avida-viewer
  cursesviewer
  main
  classification
  cpu
  event
  analyze
  drivers
  analyze
  cpu
  main
  tools
  trio
  ${NCURSES_LIBRARY}
)
ELSE(MSVC)
TARGET_LINK_LIBRARIES(avida-viewer
  cursesviewer
  main
  classification
  cpu
  event
  analyze
  drivers
  analyze
  cpu
  main
  tools
  trio
  ${NCURSES_LIBRARY}
  pthread
)
ENDIF(MSVC)

LINK_DIRECTORIES(${ALL_LIB_DIRS} ${THIRDPARTY_BLD_DIR}/trio)

INSTALL_TARGETS(/work avida-viewer)
