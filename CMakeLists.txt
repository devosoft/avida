
# This section introduces the Avida project to CMake.  In this file, it
# must be between the configuration options above (otherwise it
# overrides their defaults) and the configuration options, cmake module
# includes, and convenience variables below (otherwise some variables
# can't be properly defined because PROJECT_SOURCE_DIR and
# PROJECT_BUILD_DIR aren't yet defined).

PROJECT(AVIDA)

# The following three variables must be after the PROJECT statement, otherwise newer versions of cmake
# will (correctly) use an empty value of PROJECT_BINARY_DIR, since the project didn't exist yet.

# Default location for installed software / configs / docs is the build directory.
SET(CMAKE_INSTALL_PREFIX
  "${PROJECT_BINARY_DIR}"
  CACHE PATH
  "Install path prefix, prepended onto install directories."
  FORCE
)

# Final software is built directly into the work subdirectory.
SET(EXECUTABLE_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/bin"
  CACHE PATH
  "Single output directory for building all executables."
)

SET(LIBRARY_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/lib"
  CACHE PATH
  "Built libraries are placed here before installation."
)



# This section defines default builtin configure options.

IF(UNIX)

  IF(APPLE)
    SET(COMPILER_WARNING_FLAGS "-Wall -Wno-long-double -Wno-long-long")
  ELSE(APPLE)
    SET(COMPILER_WARNING_FLAGS "-Wall -Wno-long-long")
  ENDIF(APPLE)

  # Four types of c++ compilations:
  # - debug (Debug)
  # - minimum-size release (MinSizeRel)
  # - release (Release)
  # - release with debug info (RelWithDebInfo)
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "-Os -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O2 -ffast-math -g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -ffast-math -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)

  # Four types of c compilations:
  # - debug (Debug)
  # - minimum-size release (MinSizeRel)
  # - release (Release)
  # - release with debug info (RelWithDebInfo)
  SET(CMAKE_C_FLAGS_DEBUG
    "-g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_C_FLAGS_MINSIZEREL
    "-Os -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_C_FLAGS_RELEASE
    "-O3 -ffast-math -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO
    "-O2 -ffast-math -g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)

  # By default, show all build commands.
  SET(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL
    "If this value is on, makefiles will be generated without the .SILENT directive, and all commands will be echoed to the console during the make.  This is useful for debugging only.")
ENDIF(UNIX)

SET(CMAKE_SKIP_RPATH TRUE CACHE INTERNAL "Fix Library Path Issues?")

# Default build mode compiles c++ and c code with debug info and no
# optimizations.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE
    Release
    CACHE STRING
    "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
ENDIF(NOT CMAKE_BUILD_TYPE)


IF(UNIX)
  SET(AVD_ENABLE_TCMALLOC TRUE)
ELSE(UNIX)
  SET(AVD_ENABLE_TCMALLOC FALSE)
ENDIF(UNIX)

IF(EXISTS ${PROJECT_SOURCE_DIR}/source/archive)
  OPTION(AVD_SERIALIZATION
    "Enable boost serialization support"
    OFF
  )
ELSE(EXISTS ${PROJECT_SOURCE_DIR}/source/archive)
  SET(AVD_SERIALIZATION FALSE)
ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/source/archive)
IF(AVD_SERIALIZATION)
  ADD_DEFINITIONS(-DENABLE_SERIALIZATION=1)
ELSE(AVD_SERIALIZATION)
  REMOVE_DEFINITIONS(-DENABLE_SERIALIZATION=1)
ENDIF(AVD_SERIALIZATION)

# Experimental Boost.Python interface to avida is disabled by default.
OPTION(AVD_PY_BINDINGS
  "Enable Python interface to Avida. (EXPERIMENTAL)"
  OFF
)
IF(AVD_PY_BINDINGS)
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/avdFindPython.cmake)
  AVD_FIND_PYTHON(2.4 /Library/Frameworks)
  SET( BOOST_VERSION "1_32" CACHE STRING "Version of Boost Library to use")
  FIND_LIBRARY(
    BOOST_LIBRARY
    NAMES
      boost_python-${BOOST_VERSION}
      boost_python
    PATHS
      /usr/local/lib
    DOC "Path where the Boost Python Library can be found"
  )
  FIND_PATH(
    BOOST_INCLUDE_PATH
    boost/python/def.hpp
    /usr/local/include
    /usr/local/include/boost
    /usr/local/include/boost-${BOOST_VERSION}
    DOC "Path where the Boost Python header files can be found"
  )
  FIND_PROGRAM(
    GCCXML
    gccxml
    DOC "gccxml must be in the search path specified by your PATH environment variable in order for pyste to operate"
  )
  SET(UNIT_TESTS TRUE)
ENDIF(AVD_PY_BINDINGS)


# By default, build the console interface to Avida.
OPTION(AVD_GUI_NCURSES
  "Enable building Avida console interface."
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_NCURSES)
  # Locate the ncurses screen handling package (for Avida's console
  # interface) and the Qt graphics API (for Avida's graphic interface).
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)
  IF(NOT NCURSES_INCLUDE_PATH)
    MESSAGE(
      "Building Avida's console interface requires header files for the ncurses CRT screen handling package.  I can't find those header files.  If you know that the headers are installed, set the advanced variable NCURSES_INCLUDE_PATH to their include directory."
    )
  ENDIF(NOT NCURSES_INCLUDE_PATH)
  IF(NOT NCURSES_LIBRARY)
    MESSAGE(
      "Building Avida's console interface requires the ncurses library 'libncurses', which I can't find.  If you know that the library is installed, set the advanced variable NCURSES_LIBRARY to its location."
    )
  ENDIF(NOT NCURSES_LIBRARY)
ENDIF(AVD_GUI_NCURSES)

# By default, build the primitive version of Avida.
OPTION(AVD_PRIMITIVE
  "Enable building interfaceless Avida (fastest version)."
  ON
)

OPTION(AVD_GUI_PYQT
	"Enable the PyQt GUI interface. (EXPERIMENTAL)"
	OFF
)
IF(AVD_GUI_PYQT)
	SET(AVD_PY_BINDINGS ON CACHE BOOL "Avida Python Bindings MUST be built for the PyQt GUI." FORCE)
  FIND_PROGRAM(PYUIC pyuic DOC "Path to pyuic.  Used to compile python files from .ui files.")
ENDIF(AVD_GUI_PYQT)

OPTION(AVD_TASK_EVENT_GEN
  "Enable building the task_event_gen utility"
  OFF
)


# By default, compile all unit tests of primitive Avida classes.  Tests
# are run via 'make test' under unix.
OPTION(AVD_UNIT_TESTS
  "Enable building primitive unit test suites."
  OFF
)
IF(AVD_UNIT_TESTS)
  SET(UNIT_TESTS TRUE)
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  ADD_DEFINITIONS(-DENABLE_UNIT_TESTS)
  ADD_DEFINITIONS(-DUSE_tMemTrack=1)
  ENABLE_TESTING()
ELSE(AVD_UNIT_TESTS)
  REMOVE_DEFINITIONS(-DENABLE_UNIT_TESTS)
  REMOVE_DEFINITIONS(-DUSE_tMemTrack=1)
ENDIF(AVD_UNIT_TESTS)

IF(MSVC)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/source/platform/win32-pthread)
ENDIF(MSVC)

# This section defines convenience variables.  They're available to
# CMakeList.txt files in all subdirectories beneath the directory
# containing this file..

# Shortcuts to the source subdirectories.
SET(THIRDPARTY_SRC_DIR ${PROJECT_SOURCE_DIR}/source/third-party)
SET(TOOLS_SRC_DIR ${PROJECT_SOURCE_DIR}/source/tools)
SET(ACTIONS_SRC_DIR ${PROJECT_SOURCE_DIR}/source/actions)
SET(ANALYZE_SRC_DIR ${PROJECT_SOURCE_DIR}/source/analyze)
SET(ARCHIVE_SRC_DIR ${PROJECT_SOURCE_DIR}/source/archive)
SET(BOOST_SRC_DIR ${PROJECT_SOURCE_DIR}/source/third-party/boost)
SET(CLASSIFICATION_SRC_DIR ${PROJECT_SOURCE_DIR}/source/classification)
SET(CPU_SRC_DIR ${PROJECT_SOURCE_DIR}/source/cpu)
SET(DRIVERS_SRC_DIR ${PROJECT_SOURCE_DIR}/source/drivers)
SET(EVENT_SRC_DIR ${PROJECT_SOURCE_DIR}/source/event)
SET(MAIN_SRC_DIR ${PROJECT_SOURCE_DIR}/source/main)

# Shortcuts to the build subdirectories.
SET(THIRDPARTY_BLD_DIR ${PROJECT_BINARY_DIR}/source/third-party)
SET(TOOLS_BLD_DIR ${PROJECT_BINARY_DIR}/source/tools)
SET(ACTIONS_BLD_DIR ${PROJECT_BINARY_DIR}/source/actions)
SET(ANALYZE_BLD_DIR ${PROJECT_BINARY_DIR}/source/analyze)
SET(ARCHIVE_BLD_DIR ${PROJECT_BINARY_DIR}/source/archive)
SET(BOOST_SERIALIZATION_BLD_DIR ${PROJECT_BINARY_DIR}/source/third-party/boost/serialization)
SET(CLASSSIFICATION_BLD_DIR ${PROJECT_BINARY_DIR}/source/classification)
SET(CPU_BLD_DIR ${PROJECT_BINARY_DIR}/source/cpu)
SET(DRIVERS_BLD_DIR ${PROJECT_BINARY_DIR}/source/drivers)
SET(EVENT_BLD_DIR ${PROJECT_BINARY_DIR}/source/event)
SET(MAIN_BLD_DIR ${PROJECT_BINARY_DIR}/source/main)

# A list of all source subdirectories (convenient for naming
# include-directories).
SET(ALL_INC_DIRS
  ${TOOLS_SRC_DIR}
  ${ACTIONS_SRC_DIR}
  ${ANALYZE_SRC_DIR}
  ${ARCHIVE_SRC_DIR}
  ${BOOST_SRC_DIR}
  ${CLASSIFICATION_SRC_DIR}
  ${CPU_SRC_DIR}
  ${DRIVERS_SRC_DIR}
  ${EVENT_SRC_DIR}
  ${EVENT_BLD_DIR}
  ${MAIN_SRC_DIR}
  ${PROJECT_SOURCE_DIR}/source
)

# A list of all build subdirectories (convenient for naming
# link-subdirecties).
SET(ALL_LIB_DIRS
  ${TOOLS_BLD_DIR}
  ${ACTIONS_BLD_DIR}
  ${ANALYZE_BLD_DIR}
  ${ARCHIVE_BLD_DIR}
  ${BOOST_SERIALIZATION_BLD_DIR}
  ${CLASSIFICATION_BLD_DIR}
  ${CPU_BLD_DIR}
  ${DRIVERS_BLD_DIR}
  ${EVENT_BLD_DIR}
  ${MAIN_BLD_DIR}
  ${LIBRARY_OUTPUT_PATH}
)

SUBDIRS(source support)
