#SUBDIRS(unit_testsuites)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_EXECUTABLE(full-unit-tests full-unit-tests.cc)
IF(MSVC)
  TARGET_LINK_LIBRARIES(full-unit-tests main classification cpu event analyze drivers analyze cpu tools actions main archive boost_serialization)
ELSE(MSVC)
  TARGET_LINK_LIBRARIES(full-unit-tests main classification cpu event analyze drivers analyze cpu tools actions pthread main archive boost_serialization)
ENDIF(MSVC)
LINK_DIRECTORIES(${ALL_LIB_DIRS})
INSTALL_TARGETS(/work full-unit-tests)

ADD_TEST("full-unit-tests" "${EXECUTABLE_OUTPUT_PATH}/full-unit-tests")
