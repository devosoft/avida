# Uses PYTHON_EXE PYTHON_LIBRARY PYTHON_INCLUDE_PATH PY_BOOST_INCLUDE_PATH PY_BOOST_LIBRARIES PY_BOOST_LIBNAME AvidaPysteScript GCCXML_EXE_PATH
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH} ${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${ALL_INC_DIRS} ${PROJECT_SOURCE_DIR}/source/third-party/yaktest)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

IF(APPLE)
  SET(BOOST_PYTHON_COMPILE_FLAGS "-no-cpp-precomp -ftemplate-depth-120 -fno-inline -fPIC -Wno-long-double -Wno-long-long -DBOOST_PYTHON_DYNAMIC_LIB")
ELSE(APPLE)
  SET(BOOST_PYTHON_COMPILE_FLAGS "-Wall -ftemplate-depth-100  -DBOOST_PYTHON_DYNAMIC_LIB  -fno-inline -fPIC")
ENDIF(APPLE)

SET(AvidaPysteScript "${PROJECT_SOURCE_DIR}/source/bindings/Boost.Python/AvidaPyste.py")


#
# 
#
MACRO(ADD_PYSTE_MODULE
  ModuleName
  PysteBases
  Includes
  Defines
  ExtraDepends
  ExtraCppFiles
)
  #
  # A list of cpp source files to generate from pyste source files.
  #
  SET(${ModuleName}_CppFiles)
  #XXX
  FOREACH(Entry ${${PysteBases}})
    STRING(REGEX REPLACE "(.*)::(.*)" "\\1" PysteBase ${Entry})
    SET(${ModuleName}_CppFiles ${${ModuleName}_CppFiles} ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_${PysteBase}.cpp)
  ENDFOREACH(Entry ${${PysteBases}})
  SET(${ModuleName}_CppFiles ${${ModuleName}_CppFiles} ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_main.cpp)

  #
  # Mark those source files as generated (to prevent cmake errors when listing library depends upon files that don't yet
  # exist).
  #
  SET_SOURCE_FILES_PROPERTIES(${${ModuleName}_CppFiles}
    PROPERTIES
      GENERATED TRUE
      COMPILE_FLAGS ${BOOST_PYTHON_COMPILE_FLAGS})

  #
  # Command defining the python module build target.
  #
  ADD_LIBRARY(${ModuleName} MODULE ${${ModuleName}_CppFiles} ${${ExtraCppFiles}})
  SET_TARGET_PROPERTIES(${ModuleName} PROPERTIES PREFIX "")
  IF(${ExtraDepends})
    ADD_DEPENDENCIES(${ModuleName} ${${ExtraDepends}})
  ENDIF(${ExtraDepends})

  #
  # Make a list of pyste source files to parse, and for each such file, add the parsing command that produces the
  # corresponding cpp source file.
  #
  SET(${ModuleName}_PysteFiles)
  SET(${ModuleName}_PysteCacheFiles)
  #XXX
  FOREACH(Entry ${${PysteBases}})
    STRING(REGEX REPLACE "(.*)::(.*)" "\\1" PysteBase ${Entry})
    STRING(REGEX REPLACE "(.*)::(.*)" "\\2" Args ${Entry})
    IF(Args)
      STRING(REGEX REPLACE ":" ";" DepList ${Args})
    ELSE(Args)
      SET(DepList)
    ENDIF(Args)

    SET(${ModuleName}_PysteFiles ${${ModuleName}_PysteFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste)
    SET(${ModuleName}_PysteCacheFiles ${${ModuleName}_PysteCacheFiles}
      ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec)
    FILE(GLOB HeaderFile "${PROJECT_SOURCE_DIR}/source/*/${PysteBase}.hh")
    FILE(GLOB ThirdPartyHeaderFile "${PROJECT_SOURCE_DIR}/source/*/*/${PysteBase}.hh")
    SET(DepList ${HeaderFile} ${ThirdPartyHeaderFile} ${DepList})

    ADD_CUSTOM_COMMAND(COMMENT "${PysteBase} pyste cache file..."
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
      DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste
        # Ugly dependency tracking coercion below... If ${PysteBase}.hh
        # changes, wherever it lives, .pystec files depending on it will
        # be rebuilt.
        #${CMAKE_CURRENT_BINARY_DIR}/dummy_${PysteBase}.o
        #dummy_${PysteBase}.o
        #${${PysteBase}_HeaderFile}
        ${DepList}
      COMMAND ARGS
        export PATH=${GCCXML_EXE_PATH}:$$PATH
        \;\\\n\t
        rm -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
        \;\\\n\t
        ${PYTHON_EXE}
          ${AvidaPysteScript} ${${Includes}} ${${Defines}}
          --multiple
          --module=${ModuleName}
          --cache-dir=${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}
          --only-create-cache
          ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste
        \;\\\n\t
        test -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
        && touch ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
    )
    ADD_CUSTOM_COMMAND(COMMENT "${PysteBase} cpp source file..."
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_${PysteBase}.cpp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
        #${CMAKE_CURRENT_BINARY_DIR}/dummy_${PysteBase}.o
        #dummy_${PysteBase}.o
      COMMAND ARGS
        export PATH=${GCCXML_EXE_PATH}:$$PATH
        \;\\\n\t
        rm -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_${PysteBase}.cpp
        \;\\\n\t
        ${PYTHON_EXE}
          ${AvidaPysteScript} ${${Includes}} ${${Defines}}
          --multiple
          --module=${ModuleName}
          --cache-dir=${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}
          ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste
        \;\\\n\t
        test -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_${PysteBase}.cpp
        && touch ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_${PysteBase}.cpp
    )
  ENDFOREACH(Entry ${${PysteBases}})

  ADD_CUSTOM_COMMAND(COMMENT "${ModuleName} main cpp source file..."
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_main.cpp
    DEPENDS ${${ModuleName}_PysteCacheFiles}
    COMMAND ARGS
      export PATH=${GCCXML_EXE_PATH}:$$PATH
      \;\\\n\t
      rm -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_main.cpp
      \;\\\n\t
      ${PYTHON_EXE}
        ${AvidaPysteScript} ${${Includes}} ${${Defines}}
        --multiple
        --module=${ModuleName}
        --generate-main
        ${${ModuleName}_PysteFiles}
      \;\\\n\t
      test -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_main.cpp
      && touch ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/_main.cpp
  )
ENDMACRO(ADD_PYSTE_MODULE)

#
# 
#
MACRO(ADD_PYSTE_PACKAGE
  ModuleName
  PysteBases
  Includes
  Defines
  ExtraDepends
  ExtraCppFiles
  LinkLibraries
  PackageLocation
)
  FILE(WRITE ${LIBRARY_OUTPUT_PATH}/${ModuleName}/__init__.py
    "\"\"\"\n"
    "${ModuleName} base package init. Imports submodules comprising this package.\n"
    "Autogenerated by cmake. Changes to this file will be lost if Avida when rebuilt.\n"
    "\"\"\""
    "\n\n")

  FOREACH(Entry ${${PysteBases}})
    STRING(REGEX REPLACE "(.*)::(.*)" "\\1" PysteBase ${Entry})
    STRING(REGEX REPLACE "(.*)::(.*)" "\\2" Args ${Entry})
    IF(Args)
      STRING(REGEX REPLACE ":" ";" DepList ${Args})
    ELSE(Args)
      SET(DepList)
    ENDIF(Args)

    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp
      PROPERTIES
        GENERATED TRUE
        COMPILE_FLAGS ${BOOST_PYTHON_COMPILE_FLAGS})

    FILE(GLOB HeaderFile "${PROJECT_SOURCE_DIR}/source/*/${PysteBase}.hh")
    FILE(GLOB ThirdPartyHeaderFile "${PROJECT_SOURCE_DIR}/source/*/*/${PysteBase}.hh")
    SET(DepList ${HeaderFile} ${ThirdPartyHeaderFile} ${DepList})

    ADD_CUSTOM_COMMAND(COMMENT "${PysteBase} pyste cache file..."
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.pystec
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste ${DepList}
      COMMAND ARGS
        export PATH=${GCCXML_EXE_PATH}:$$PATH
        \;\\\n\t
        rm -f ${CMAKE_CURRENT_BINARY_DIR}/${ModuleName}/${PysteBase}.pystec
        \;\\\n\t
        ${PYTHON_EXE}
          ${AvidaPysteScript} ${${Includes}} ${${Defines}}
          --module=${PysteBase}
          --cache-dir=${CMAKE_CURRENT_BINARY_DIR}
          --only-create-cache
          ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste
        \;\\\n\t
        test -f ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.pystec
        && touch ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.pystec
    )
    ADD_CUSTOM_COMMAND(COMMENT "${PysteBase} cpp source file..."
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.pystec
      COMMAND ARGS
        export PATH=${GCCXML_EXE_PATH}:$$PATH
        \;\\\n\t
        rm -f ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp
        \;\\\n\t
        ${PYTHON_EXE}
          ${AvidaPysteScript} ${${Includes}} ${${Defines}}
          --module=${PysteBase}
          --cache-dir=${CMAKE_CURRENT_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/${PysteBase}.pyste
        \;\\\n\t
        test -f ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp
        && touch ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp
    )
    FILE(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${ModuleName})
    ADD_LIBRARY(${ModuleName}/${PysteBase} MODULE ${CMAKE_CURRENT_BINARY_DIR}/${PysteBase}.cpp)
    SET_TARGET_PROPERTIES(${ModuleName}/${PysteBase} PROPERTIES PREFIX "")
    TARGET_LINK_LIBRARIES(${ModuleName}/${PysteBase} ${${LinkLibraries}})
    INSTALL_TARGETS(${PackageLocation}/${ModuleName} $${ModuleName}/${PysteBase})
    FILE(APPEND ${LIBRARY_OUTPUT_PATH}/${ModuleName}/__init__.py "from ${PysteBase} import *\n")
  ENDFOREACH(Entry ${${PysteBases}})
ENDMACRO(ADD_PYSTE_PACKAGE)
