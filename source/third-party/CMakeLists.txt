


SUBDIRS(
  trio
  yaktest
)

IF(BUILD_AvidaSupportLibs)
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/AvidaSupportLibs/CMakeLists.txt)
    SUBDIRS(AvidaSupportLibs)
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/AvidaSupportLibs/CMakeLists.txt)
ENDIF(BUILD_AvidaSupportLibs)


