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
    count_tracker.t
    cpu_stats.t
    double_sum.t
    event_triggers.t
    fixed_block.t
    genotype_test_data.t
    int_sum.t
    memory_flags.t
    merit.t
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
