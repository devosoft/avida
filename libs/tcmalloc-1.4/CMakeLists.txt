PROJECT(TCMALLOC CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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


IF(UNIX)
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "-Os -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O3 -g -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -DNDEBUG"
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


SET(TCMALLOC_SOURCES)

SET(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(SRC_SOURCES
  ${SRC_DIR}/atomicops-internals-x86.cc
  ${SRC_DIR}/central_freelist.cc
  ${SRC_DIR}/common.cc
  ${SRC_DIR}/internal_logging.cc
  ${SRC_DIR}/malloc_extension.cc
  ${SRC_DIR}/page_heap.cc
  ${SRC_DIR}/span.cc
  ${SRC_DIR}/spinlock.cc
  ${SRC_DIR}/static_vars.cc
  ${SRC_DIR}/sysinfo.cc
  ${SRC_DIR}/system-alloc.cc
  ${SRC_DIR}/tcmalloc.cc
  ${SRC_DIR}/thread_cache.cc
)
SOURCE_GROUP(src FILES ${SRC_SOURCES})
LIST(APPEND TCMALLOC_SOURCES ${SRC_SOURCES})

INCLUDE_DIRECTORIES(${SRC_DIR})

ADD_LIBRARY(tcmalloc-1.4 STATIC ${TCMALLOC_SOURCES})
IF(NOT MSVC)
  TARGET_LINK_LIBRARIES(tcmalloc-1.4 pthread)
ENDIF(NOT MSVC)
INSTALL(TARGETS tcmalloc-1.4 DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
SET(tcmalloc_BUILT 1 CACHE INTERNAL "")
