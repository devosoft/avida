
event {
  HEADERS += $$EVENT_HH/cEvent.h \
             $$EVENT_HH/cEventFactoryManager.h \
             $$EVENT_HH/cEventList.h \
             $$EVENT_HH/cPopulationEventFactory.h

  SOURCES += $$EVENT_CC/event.cc \
             $$EVENT_CC/cEventFactoryManager.cc \
             $$EVENT_CC/cEventList.cc \
             $$EVENT_CC/event_list_entry.cc \
             $$EVENT_CC/cEventListIterator.cc \
             $$EVENT_CC/cEventTriggers.cc \
             $$EVENT_CC/population_event.cc \
             $$EVENT_CC/cPopulationEventFactory.cc
}
