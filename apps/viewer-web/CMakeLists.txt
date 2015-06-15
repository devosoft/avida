PROJECT(AVIDA_WEB)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(CMakeDependentOption)


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





# Locate Apto's Headers
SET(APTO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../libs/apto/include)

# Locate Empirical's Headers
SET(EMPIRICAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../libs/empirical)

# Locate Avida-Core's Headers
SET(AVIDA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../avida-core/include)


INCLUDE_DIRECTORIES(
   ${APTO_INCLUDE_DIR}
   ${EMPIRICAL_INCLUDE_DIR}
   ${AVIDA_INCLUDE_DIR}
   )


# Avida-HTML should only be built if the EMSCRIPTEN toolchain is active
# and the commandline version of Avida is not requested.
CMAKE_DEPENDENT_OPTION(AVD_HTML
   "Enable building JS/Emscripten/HTML version of Avida"
   ON
   "AVD_CMDLINE; NOT EMSCRIPTEN" OFF
)


# Setup the Avida-Web Target
IF(AVD_HTML)
   SET(AVIDA_HTML_DIR src/)
   SET(
      AVIDA_HTML_SOURCES 
      ${AVIDA_HTML_DIR}/viewer-web.cc
      )
   SOURCE_GROUP(target\\avida FILES ${AVIDA_CMDLINE_SOURCES})
  
   SET(CMAKE_EXE_LINKER_FLAGS 
        "${CMAKE_EXE_LINKER_FLAGS} 
        --embed-file ${EMS_AVIDA_EMBED_DIR}@/ -s ALLOW_MEMORY_GROWTH=1")
     ADD_EXECUTABLE(avida.html 
                    ${AVIDA_HTML_SOURCES}
                    $<TARGET_OBJECTS:apto_objs>
                    $<TARGET_OBJECTS:avida_core_objs>
                   )
   INSTALL_TARGETS(/work avida.html)
ENDIF(AVD_HTML)


# Install web-related resources (e.g. images, etc.)
INSTALL(DIRECTORY resources/ DESTINATION work)


