SUBDIRS(boost/serialization)

IF(AVD_GUI_NCURSES)
  SUBDIRS(trio)
ENDIF(AVD_GUI_NCURSES)

IF(BUILD_AvidaSupportLibs)
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/AvidaSupportLibs/CMakeLists.txt)
    SUBDIRS(AvidaSupportLibs)
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/AvidaSupportLibs/CMakeLists.txt)
ENDIF(BUILD_AvidaSupportLibs)


