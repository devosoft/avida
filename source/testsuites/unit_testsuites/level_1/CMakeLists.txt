IF(AVD_UNIT_TESTS)
  INCLUDE_DIRECTORIES(
    ${ALL_INC_DIRS}
    ${THIRDPARTY_SRC_DIR}/yaktest
  )
  LINK_DIRECTORIES(
    ${ALL_LIB_DIRS}
    ${THIRDPARTY_BLD_DIR}/yaktest
  )
  SET(TESTSUITES
    code_label.t
    instruction.t
  ) 
  FOREACH(TESTSUITE ${TESTSUITES})
    ADD_EXECUTABLE(${TESTSUITE} ${TESTSUITE}.cc)
    TARGET_LINK_LIBRARIES(${TESTSUITE}
      main
      cpu
      event
      main
      tools
      yaktest
    )
    ADD_TEST(${TESTSUITE} ${TESTSUITE})
  ENDFOREACH(TESTSUITE)
ENDIF(AVD_UNIT_TESTS)
