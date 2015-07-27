CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(CMakeDependentOption)


MESSAGE(STATUS "AVIDAED WEB")


# The following three variables must be after the PROJECT statement, otherwise
# newer versions of cmake will (correctly) use an empty value of
# PROJECT_BINARY_DIR, since the project didn't exist yet.

# Default location for installed software/configs/ docs is the build directory.
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



# This section defines default builtin compiler options
# ------------------------------------------------------------------------------
IF(UNIX)
  IF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    SET(COMPILER_WARNING_FLAGS "")
    SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -OPT:Olimit=0")
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      SET(COMPILER_WARNING_FLAGS "")
      SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -mp1 -pc64")
    ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      IF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
        SET(COMPILER_WARNING_FLAGS "-Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-trigraphs")
        SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -fvisibility-inlines-hidden")
      ELSE (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
        SET(COMPILER_WARNING_FLAGS "-Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-trigraphs")
        SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -ftree-vectorize -fvisibility-inlines-hidden")
      ENDIF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
    ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")

  INCLUDE(CheckCSourceCompiles)
  IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(HAVE_FUSED_MADD FALSE)
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    IF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
      SET(HAVE_FUSED_MADD FALSE)
    ELSE (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
      SET(CMAKE_REQUIRED_FLAGS "-mno-fused-madd")
      CHECK_C_SOURCE_COMPILES("int main() { return 0; }" HAVE_FUSED_MADD)
    ENDIF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  SET(CMAKE_REQUIRED_FLAGS "")

  IF(HAVE_FUSED_MADD)
    SET(COMPILER_OPTIMIZATION_FLAGS 
       "-mno-fused-madd ${COMPILER_OPTIMIZATION_FLAGS}")
  ENDIF(HAVE_FUSED_MADD)

  # Some emscripten warnings
   IF(EMSCRIPTEN)
      SET(COMPILER_WARNING_FLAGS 
         "${COMPILER_WARNING_FLAGS} -Wno-warn-absolute-paths")
   ENDIF(EMSCRIPTEN)

  # Four types of c++ compilations:
  # - debug (Debug)
  # - minimum-size release (MinSizeRel)
  # - release (Release)
  # - release with debug info (RelWithDebInfo)
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "-Os ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O2 -g ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE
    "-O2 ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
ENDIF(UNIX)


# Default build mode compiles c++ and c code with debug info and no
# optimizations.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE
    Release
    CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
ENDIF(NOT CMAKE_BUILD_TYPE)



# Locate Rapid JSON's headers
SET(RAPIDJSON_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/rapidjson/include)

# Locate apto's headers
SET(APTO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/apto/include)


INCLUDE_DIRECTORIES(
   ${RAPIDJSON_INCLUDE_DIR}
   ${AVIDA_INCLUDES}
   ${APTO_INCLUDE_DIR}
   )


SET(AVIDAED_WEB_DIR src/)
SET(
   AVIDAED_WEB_SOURCES 
   ${AVIDAED_WEB_DIR}/viewer-webed.cc
   )
SOURCE_GROUP(target\\avida FILES ${AVIDAED_WEB_SOURCES})


# Setup the configuration files to be loaded at start
# Keep in mind this path needs to be relative
# Unfortunately, it is relative to where ever we are in 
# the build system.  In this case we're a couple levels
# deep in the cbuild directory, so we need to go back up
# a few levels.
SET(EMS_AVIDA_EMBED_DIR ../../../apps/viewer-webed/config)

# A list of functions that should be exported
# The list is fragile.  It *must* be enclosed in quotes
# (") when fed to the shell.  Also, the list must include
# a space between entries.
SET(EMS_EXPORT_FUNCS
   "\"['_main', '_empCppCallback', '_StepDriver']\""
   )

# Flags specific to the web target
SET(WEB_FLAGS
   "-s DISABLE_EXCEPTION_CATCHING=1\
   -s COMPILER_ASSERTIONS=1\
   -s EXPORTED_FUNCTIONS=${EMS_EXPORT_FUNCS}\
   -s NO_EXIT_RUNTIME=1\
   -s DEMANGLE_SUPPORT=1\
   -DDEBUG "
   )

SET(OFLAGS_WEB 
   "-g4 -Wno-warn-absolute-paths -s TOTAL_MEMORY=67108864 ")
SET(CMAKE_CXX_FLAGS_RELEASE "")
SET(CMAKE_CXX_FLAGS "${OFLAGS_WEB} ${WEB_FLAGS} -std=c++11 -Wno-unused-function")
SET(CMAKE_EXE_LINKER_FLAGS 
     "${CMAKE_EXE_LINKER_FLAGS}\
     --embed-file ${EMS_AVIDA_EMBED_DIR}@/ -s ALLOW_MEMORY_GROWTH=1")


#Setup the Avida-Web Target
ADD_EXECUTABLE(avida
                 ${AVIDAED_WEB_SOURCES}
                 $<TARGET_OBJECTS:apto_objs>
                 $<TARGET_OBJECTS:avida_core_objs>
                 )
   

INSTALL_TARGETS(/work avida)

SET(BINARY_SUPPORT
   ${EXECUTABLE_OUTPUT_PATH}/avida.js.map
   ${EXECUTABLE_OUTPUT_PATH}/avida.js.mem
   )
INSTALL(DIRECTORY resources/ DESTINATION work)
INSTALL_FILES(/work FILES ${BINARY_SUPPORT})

