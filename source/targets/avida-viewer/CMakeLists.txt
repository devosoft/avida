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

SET(avida_viewer_target_link_libs
  cursesviewer main classification cpu event analyze drivers analyze cpu tools trio actions ${NCURSES_LIBRARY}
)


IF(NOT MSVC)
  SET(avida_viewer_target_link_libs
    ${avida_viewer_target_link_libs} pthread
  )
ENDIF(NOT MSVC)

IF(AVD_ENABLE_TCMALLOC)
  SET(avida_viewer_target_link_libs
    ${avida_viewer_target_link_libs} tcmalloc
  )
ENDIF(AVD_ENABLE_TCMALLOC)

SET(avida_viewer_target_link_libs
  ${avida_viewer_target_link_libs} archive boost_serialization
)

SET(avida_viewer_target_link_libs
  ${avida_viewer_target_link_libs} main
)

TARGET_LINK_LIBRARIES(avida-viewer ${avida_viewer_target_link_libs})
LINK_DIRECTORIES(${ALL_LIB_DIRS} ${THIRDPARTY_BLD_DIR}/trio)
INSTALL_TARGETS(/work avida-viewer)
