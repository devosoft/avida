SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL
"If this value is on, makefiles will be generated without the .SILENT directive, and all commands will be echoed to the console during the make.  This is useful for debugging only."
)

PROJECT(AvidaConsistencyChecks)

#SET(AvidaExePath "/Users/kaben/Projects/Software/Avida/branch.kaben/current--HEAD--cvs/build.cmake.dbg/bin/primitive")
#SET(PythonExePath "/sw/bin/python")
FIND_PROGRAM(_AvidaExePath
  NAMES avida
  PATHS ${PROJECT_BINARY_DIR}
  DOC "Where can the Avida executable 'avida' be found ?"
)
IF(_AvidaExePath)
  SET(AvidaExePath_FOUND TRUE)
ENDIF(_AvidaExePath)

FIND_PROGRAM(_PythonExePath
  NAMES python python2.3 python2.4
  DOC "Where can the Python executable be found ?"
)
IF(_PythonExePath)
  SET(PythonExePath_FOUND TRUE)
ENDIF(_PythonExePath)

OPTION(AVD_CONSISTENCY_TEST_PRIMITIVE "Enable consistency check test suites." OFF)
VARIABLE_REQUIRES(AVD_CONSISTENCY_TEST_PRIMITIVE ChecksExes_FOUND AvidaExePath_FOUND PythonExePath_FOUND)

IF(AVD_CONSISTENCY_TEST_PRIMITIVE)
  ENABLE_TESTING()
  #INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(AVD_CONSISTENCY_TEST_PRIMITIVE)

SET(AvidaConsistencyChecksPythonSiteDir "${PROJECT_SOURCE_DIR}/Support/python")

SUBDIRS(Tests)

# Vim modeline to tell Vim that this is a configuration script.
# vim: set ft=config:
