
IF(AVD_GUI_PYQT)
  FILE(GLOB_RECURSE PYUI_FILES "*.ui")
  FOREACH(PYUI_FILE ${PYUI_FILES})
    GET_FILENAME_COMPONENT(PYUI_PATH ${PYUI_FILE} PATH)
    GET_FILENAME_COMPONENT(PYUI_MODULE ${PYUI_FILE} NAME_WE)
    SET(ABS_PYUI_MODULE ${PYUI_PATH}/${PYUI_MODULE})
    ADD_CUSTOM_TARGET(
      ${ABS_PYUI_MODULE}.py
      ALL
      ${PYUIC}
      -p 2 -o
      ${ABS_PYUI_MODULE}.py
      ${ABS_PYUI_MODULE}.ui
      DEPENDS
      ${ABS_PYUI_MODULE}.ui
    )
    SET_DIRECTORY_PROPERTIES(PROPERTIES
      ADDITIONAL_MAKE_CLEAN_FILES "${ADDITIONAL_MAKE_CLEAN_FILES};${ABS_PYUI_MODULE}.py")
  ENDFOREACH(PYUI_FILE ${PYUI_FILES})

  IF(UNIX)
    IF(APPLE)
      CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/AvidaEd-osx.py ${CMAKE_CURRENT_BINARY_DIR}/AvidaEd.py COPYONLY)
      CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/setup_osx.py.in ${CMAKE_CURRENT_BINARY_DIR}/setup.py)

      # CMake custom targets often produce strange and idiosyncratic
      # output in Makefiles, and this has become a real pain. I would
      # like a different build system again.
      # @kgn
      ADD_CUSTOM_TARGET(BUILD_AvidaEd ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/AvidaEd.app)
      #ADD_CUSTOM_TARGET(
      #  ${CMAKE_CURRENT_BINARY_DIR}/build
      #  ALL
      #  DEPENDS ${EXECUTABLE_OUTPUT_PATH}/AvidaEd.app/Contents/MacOS/AvidaEd
      #)
      ADD_CUSTOM_COMMAND(
        OUTPUT ${EXECUTABLE_OUTPUT_PATH}/AvidaEd.app/Contents/MacOS/AvidaEd
        COMMAND ${PYTHON_EXE}
        DEPENDS ${LIBRARY_OUTPUT_PATH}/libPyAvidaRepairHacks.dylib
        ARGS setup.py py2app
      )

      # 'clean' build target
      # Grr...
      # The cmake developers adamantly refuse to give access to the
      # 'clean' target. So I add my own, and make gnumake complain...
      # @kgn
      ADD_CUSTOM_TARGET(CLEAN_AvidaEd ${PYTHON_EXE} setup.py clean --all)
      ADD_CUSTOM_TARGET(clean DEPENDS CLEAN_AvidaEd)
    ELSE(APPLE)
      # FIXME # need unix case
      # @kgn
    ENDIF(APPLE)
  ENDIF(UNIX)
  # FIXME # need windows case
  # @kgn
ENDIF(AVD_GUI_PYQT)


