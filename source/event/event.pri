
event {
  HEADERS += $$EVENT_HH/event.hh \
             $$EVENT_HH/event_factory.hh \
             $$EVENT_HH/event_factory_manager.hh \
             $$EVENT_HH/event_list.hh \
             $$EVENT_HH/population_event_factory.hh

  SOURCES += $$EVENT_CC/event.cc \
             $$EVENT_CC/event_factory.cc \
             $$EVENT_CC/event_factory_manager.cc \
             $$EVENT_CC/event_list.cc \
             $$EVENT_CC/event_list_entry.cc \
             $$EVENT_CC/event_list_iterator.cc \
             $$EVENT_CC/event_triggers.cc \
             $$EVENT_CC/population_event.cc \
             $$EVENT_CC/population_event_factory.cc
}
