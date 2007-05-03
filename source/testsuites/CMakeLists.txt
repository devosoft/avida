
INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

#
# This setup assumes that unit test suite source code has the same name as the
# unit test, e.g.:
# blah-unit-tests.cc and blah-unit-tests
#

SET(UNITTEST_SUITES
  full-unit-tests
  cInitFile-unit-tests
)   
FOREACH(TEST_SUITE ${UNITTEST_SUITES})
  ADD_EXECUTABLE(${TEST_SUITE} ${TEST_SUITE}.cc)
  IF(MSVC)
    TARGET_LINK_LIBRARIES(${TEST_SUITE} avidacore)
  ELSE(MSVC)
    TARGET_LINK_LIBRARIES(${TEST_SUITE} avidacore pthread)
  ENDIF(MSVC)
  INSTALL_TARGETS(/work ${TEST_SUITE})
  
  ADD_TEST("${TEST_SUITE}" "${EXECUTABLE_OUTPUT_PATH}/${TEST_SUITE}")
ENDFOREACH(TEST_SUITE)
