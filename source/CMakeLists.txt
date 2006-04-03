
SUBDIRS(analyze classification cpu drivers event main targets third-party tools support utils/task_events)
IF(AVD_PY_BINDINGS)
  SUBDIRS(bindings python)
ENDIF(AVD_PY_BINDINGS)
IF(AVD_UNIT_TESTS)
  #SUBDIRS(testsuites)
ENDIF(AVD_UNIT_TESTS)
