INCLUDE_DIRECTORIES(${PY_BOOST_INCLUDE_PATH})

IF(UNIT_TESTS)
  SET(libyaktest_SOURCES
    cTestCase.cc
    cTestProblem.cc
    cTestResult.cc
    cTextTestInterpreter.cc
  )
  ADD_LIBRARY(yaktest ${libyaktest_SOURCES})
ENDIF(UNIT_TESTS)
