PROJECT(avida-main)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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


ADD_SUBDIRECTORY(libs/apto)
IF(NOT WIN32)
  ADD_SUBDIRECTORY(libs/tcmalloc-1.4)
ENDIF(NOT WIN32)
ADD_SUBDIRECTORY(avida-core)

# Default location for installed software/configs/ docs is the build directory.
SET(CMAKE_INSTALL_PREFIX
  "${PROJECT_BINARY_DIR}"
  CACHE PATH
  "Install path prefix, prepended onto install directories."
  FORCE
)

