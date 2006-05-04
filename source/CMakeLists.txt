
SUBDIRS(actions analyze archive classification cpu drivers event main platform targets third-party tools utils/task_events)
IF(AVD_PY_BINDINGS)
  SUBDIRS(bindings python)
ENDIF(AVD_PY_BINDINGS)
IF(AVD_UNIT_TESTS)
  SUBDIRS(testsuites)
ENDIF(AVD_UNIT_TESTS)
