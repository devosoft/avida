
SUBDIRS(third-party tools cpu event main viewers testsuites support utils/task_events)
IF(AVD_PY_BINDINGS)
  SUBDIRS(bindings)
ENDIF(AVD_PY_BINDINGS)
