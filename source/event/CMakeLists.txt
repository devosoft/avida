SET(libevent_a_SOURCES
  cEventFactoryManager.cc
  cEventList.cc
  cEventListIterator.cc
  cEventTriggers.cc
  cPopulationEventFactory.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(event ${libevent_a_SOURCES})
