#SET(EXECUTABLE_OUTPUT_PATH "")
INCLUDE_DIRECTORIES(
  ${ALL_INC_DIRS}
  ${THIRDPARTY_SRC_DIR}
)

# Each of the test-names below must match the prefix of the source file
# containing it (e.g., UnitTest_recursive_tests must be contained in the
# file UnitTest_recursive_tests.cc) and that file must have a function
# of the same name to launch the tests (e.g.,
# UnitTest_recursive_tests.cc must define a function named
# UnitTest_recursive_tests).
#
# (I think that these are silly requirements, but CMake's test-driver
# system requires them.)
SET(TESTSUITES
  UnitTest_recursive_tests
)

ADD_DEFINITIONS(-DUSING_TESTDRIVER)

FOREACH(TESTSUITE ${TESTSUITES})
  ADD_TEST(${TESTSUITE} ${EXECUTABLE_OUTPUT_PATH}/Tools_UnitTestDriver ${TESTSUITE})
ENDFOREACH(TESTSUITE)

CREATE_TEST_SOURCELIST(UNIT_TEST_DRIVER_SRCS test_driver.cc ${TESTSUITES})
ADD_EXECUTABLE(Tools_UnitTestDriver test_driver.cc UnitTest_recursive_tests.cc)
TARGET_LINK_LIBRARIES(Tools_UnitTestDriver tools yaktest)
