#
# This module finds the Python executable. It isn't very smart, and
# might get confused on Mac OS X if more than one version of python is
# installed.
#

#
# This module finds if Python is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#
#  PYTHON_LIBRARIES       = the full path to the library found
#  PYTHON_INCLUDE_PATH    = the path to where Python.h can be found
#  PYTHON_DEBUG_LIBRARIES = the full path to the debug library found
#

INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/avdFindFrameworks.cmake)

MACRO(AVD_FIND_PYTHON version search_paths)
  IF(APPLE)

    # Search for Python frameworks.  This defines Python_FRAMEWORKS to a list of Python frameworks found
    # along search_paths. Python_FRAMEWORKS will be used to make a list of Python-framework include search paths.
    AVD_FIND_FRAMEWORKS(Python ${search_paths})

    SET(PYTHON_FRAMEWORK_FOUND FALSE)
    IF(PYTHON_FRAMEWORK_PATH)
      IF(EXISTS ${PYTHON_FRAMEWORK_PATH}/Python.framework/Versions/${version}/include/python${version}/Python.h)
        SET(PYTHON_FRAMEWORK_FOUND TRUE)
      ENDIF(EXISTS ${PYTHON_FRAMEWORK_PATH}/Python.framework/Versions/${version}/include/python${version}/Python.h)
    ELSE(PYTHON_FRAMEWORK_PATH)
      FOREACH(dir ${Python_FRAMEWORK_PATHS})
        # This chooses the first of the frameworks directories that contains a valid python framework.
        IF(NOT PYTHON_FRAMEWORK_PATH)
          # The a valid python framework hasn't been found yet, keep searching...
          IF(EXISTS ${dir}/Python.framework/Versions/${version}/include/python${version}/Python.h)
            SET(PYTHON_FRAMEWORK_PATH
              "${dir}"
              CACHE PATH
              "Apple-style Python Framework directory."
              FORCE
            )
          ENDIF(EXISTS ${dir}/Python.framework/Versions/${version}/include/python${version}/Python.h)
        ENDIF(NOT PYTHON_FRAMEWORK_PATH)
      ENDFOREACH(dir)
    ENDIF(PYTHON_FRAMEWORK_PATH)

    IF(EXISTS ${PYTHON_FRAMEWORK_PATH})
      SET(PYTHON_FRAMEWORK_FOUND TRUE)
      SET(PYTHON_EXE
        "${PYTHON_FRAMEWORK_PATH}/Python.framework/Versions/${version}/bin/python${version}"
        CACHE INTERNAL
        "Apple-style Python Framework executable."
        FORCE
      )
      SET(PYTHON_INCLUDE_PATH
        "${PYTHON_FRAMEWORK_PATH}/Python.framework/Versions/${version}/include/python${version}"
        CACHE INTERNAL
        "Apple-style Python Framework include directory."
        FORCE
      )
      SET(PYTHON_LIBRARY
        "-F\"${PYTHON_FRAMEWORK_PATH}\" -framework Python"
        CACHE INTERNAL
        "Apple-style Python Framework linking flags."
        FORCE
      )
      SET(PYTHON_DEBUG_LIBRARY
        "-F\"${PYTHON_FRAMEWORK_PATH}\" -framework Python"
        CACHE INTERNAL
        "Apple-style Python Framework linking flags."
        FORCE
      )
      #SET(PYTHON_INCLUDE_PATH
      #  ${PYTHON_INCLUDE_PATH}
      #  CACHE PATH
      #  "Apple-style Python Framework Include Path."
      #  FORCE
      #)
    ELSE(EXISTS ${PYTHON_FRAMEWORK_PATH})
      SET(PYTHON_EXE_SEARCH_PATHS
        /usr/bin
        /usr/local/bin
      )
      FIND_PROGRAM(PYTHON_EXE
        NAMES python python${version}
        PATHS ${PYTHON_EXE_SEARCH_PATHS}
      )
    ENDIF(EXISTS ${PYTHON_FRAMEWORK_PATH})
  ELSE(APPLE)

    # Collect search paths for Python libraries and Python includes; store search paths in variables
    # PYTHON_LIBRARY_SEARCH_PATHS and PYTHON_INCLUDE_SEARCH_PATHS.
    SET(STD_PYTHON_INCLUDE_SEARCH_PATHS
      /usr/include/python${version}
      /usr/include
      /usr/local/include/python${version}
      /usr/local/include
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${version}\\InstallPath]/include
    )
    SET(STD_PYTHON_LIBRARY_SEARCH_PATHS
      /usr/lib/python${version}/config
      /usr/lib
      /usr/local/lib/python${version}/config
      /usr/local/lib
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${version}\\InstallPath]/libs
    )
    SET(STD_PYTHON_EXE_SEARCH_PATHS
      /usr/bin
      /usr/local/bin
      # XXX This is probably not the correct path... @kgn
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${version}\\InstallPath]/bin
    )

    SET(PYTHON_INCLUDE_SEARCH_PATHS)
    SET(PYTHON_LIBRARY_SEARCH_PATHS)
    SET(PYTHON_EXE_SEARCH_PATHS)

    FOREACH(path ${search_paths})
      SET(PYTHON_INCLUDE_SEARCH_PATHS
        ${PYTHON_INCLUDE_SEARCH_PATHS}
        ${path}/include/python${version}
        ${path}/include
        ${path}/local/include/python${version}
        ${path}/local/include
      )
      SET(PYTHON_LIBRARY_SEARCH_PATHS
        ${PYTHON_LIBRARY_SEARCH_PATHS}
        ${path}/lib/python${version}/config
        ${path}/lib
        ${path}/local/lib/python${version}/config
        ${path}/local/lib
      )
      SET(PYTHON_EXE_SEARCH_PATHS
        ${PYTHON_EXE_SEARCH_PATHS}
        ${path}/bin
        ${path}/local/bin
      )
    ENDFOREACH(path)

    SET(PYTHON_INCLUDE_SEARCH_PATHS
      ${PYTHON_INCLUDE_SEARCH_PATHS}
      ${STD_PYTHON_INCLUDE_SEARCH_PATHS}
    )
    SET(PYTHON_LIBRARY_SEARCH_PATHS
      ${PYTHON_LIBRARY_SEARCH_PATHS}
      ${STD_PYTHON_LIBRARY_SEARCH_PATHS}
    )
    SET(PYTHON_EXE_SEARCH_PATHS
      ${PYTHON_EXE_SEARCH_PATHS}
      ${STD_PYTHON_EXE_SEARCH_PATHS}
    )
    
    # Search for Python includes and libraries.
    FIND_PATH(PYTHON_INCLUDE_PATH
      Python.h
      ${PYTHON_INCLUDE_SEARCH_PATHS}
    )
    FIND_LIBRARY(PYTHON_LIBRARY
      NAMES python python${version} python${version}.dll
      PATHS ${PYTHON_LIBRARY_SEARCH_PATHS}
    )
    FIND_PROGRAM(PYTHON_EXE
      NAMES python python.exe python${version} python${version}.exe
      PATHS ${PYTHON_EXE_SEARCH_PATHS}
    )

    IF(WIN32)
      FIND_LIBRARY(PYTHON_DEBUG_LIBRARY 
        NAMES python_d python python${version} python${version}.dll
        PATHS [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${version}\\InstallPath]/libs/Debug
      )
      MARK_AS_ADVANCED(
        PYTHON_DEBUG_LIBRARY 
        PYTHON_LIBRARY 
        PYTHON_INCLUDE_PATH
      )
    ENDIF(WIN32)

    # We use PYTHON_LIBRARY and PYTHON_DEBUG_LIBRARY for the cache entries
    # because they are meant to specify the location of a single library.
    # We now set the variables listed by the documentation for this
    # module.
    SET(PYTHON_LIBRARIES "${PYTHON_LIBRARY}")
    IF(WIN32)
      SET(PYTHON_DEBUG_LIBRARIES "${PYTHON_DEBUG_LIBRARY}")
    ENDIF(WIN32)
  ENDIF(APPLE)
ENDMACRO(AVD_FIND_PYTHON)

