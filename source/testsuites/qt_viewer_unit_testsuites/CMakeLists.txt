INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FixFindQt.cmake)

SET(EXECUTABLE_OUTPUT_PATH "")

ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)
IF(_UNIT_TEST_QT_GUI AND _QT_GRAPHIC_INTERFACE)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${GUI_SRC_DIR}
    ${THIRDPARTY_SRC_DIR}/yaktest
    ${QT_INCLUDE_DIR}
    ${QT_INCLUDE_PATH}
    .
  )
  LINK_DIRECTORIES(
    ${ALL_LIB_DIRS}
    ${GUI_BLD_DIR}
    ${THIRDPARTY_BLD_DIR}/yaktest
    .
  )

  SET(TESTSUITES
  	template.t
  ) 
  STRING(REGEX REPLACE [.]t .t.hh\; TESTSUITE_MOCSRCS ${TESTSUITES})
  SET(libqtunittest_a_SOURCES
    qt_test_case.cc
  )
  SET(libqtunittest_moc_a_MOC_SOURCES
    qt_test_case.hh
    ${TESTSUITE_MOCSRCS}
  )
  QT_WRAP_CPP(qtunittest_moc
    libqtunittest_moc_a_SOURCES
    libqtunittest_moc_a_MOC_SOURCES
  )
  ADD_LIBRARY(qtunittest_moc ${libqtunittest_moc_a_SOURCES})
  ADD_LIBRARY(qtunittest ${libqtunittest_a_SOURCES})

  FOREACH(TESTSUITE ${TESTSUITES})
    ADD_EXECUTABLE(${TESTSUITE} ${TESTSUITE}.cc)
    TARGET_LINK_LIBRARIES(${TESTSUITE}
      qtviewer
      main
      cpu
      event
      main
      tools
      yaktest
      qtviewer_moc
      qtviewer_ui
      qtunittest
      qtunittest_moc
      ${QT_LIBRARIES}
    )
    ADD_TEST(${TESTSUITE} ${TESTSUITE})
  ENDFOREACH(TESTSUITE)
ENDIF(_UNIT_TEST_QT_GUI AND _QT_GRAPHIC_INTERFACE)

