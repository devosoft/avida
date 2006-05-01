INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_EXECUTABLE(task_event_gen task_event_gen.cc)
TARGET_LINK_LIBRARIES(task_event_gen tools archive boost_serialization)
LINK_DIRECTORIES(${ALL_LIB_DIRS})
INSTALL_TARGETS(/work task_event_gen)

