
INCLUDE_DIRECTORIES(${ALL_INC_DIRS})
LINK_DIRECTORIES(${ALL_LIB_DIRS})

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
    TARGET_LINK_LIBRARIES(${TEST_SUITE} main classification cpu analyze drivers analyze cpu tools actions main archive boost_serialization)
  ELSE(MSVC)
    TARGET_LINK_LIBRARIES(${TEST_SUITE} main classification cpu analyze drivers analyze cpu tools actions pthread main archive boost_serialization)
  ENDIF(MSVC)
  INSTALL_TARGETS(/work ${TEST_SUITE})
  
  ADD_TEST("${TEST_SUITE}" "${EXECUTABLE_OUTPUT_PATH}/${TEST_SUITE}")
ENDFOREACH(TEST_SUITE)
