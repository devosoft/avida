SET(libevent_a_SOURCES
  cEventManager.cc
  cEventList.cc
  cEventTriggers.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(event ${libevent_a_SOURCES})
