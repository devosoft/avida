SET(libevent_a_SOURCES
  event_factory_manager.cc
  event_list.cc
  event_list_iterator.cc
  event_triggers.cc
  population_event_factory.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(event ${libevent_a_SOURCES})
