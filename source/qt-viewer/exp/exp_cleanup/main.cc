#include <iostream>
#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef AVD_MISSION_CONTROL_HH
#include "mc/avd_mission_control.hh"
#endif
#ifndef WATCH_RETAINS_HH
#include "memory_mgt/watch_retains.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_HH
#include "user_msg/avd_message_display.hh"
#endif

class cRetainableTest : public cRetainable {};

int main( int argc, char **argv ){
  avdDebug("in.");
  cWatchRetains retain_watch;
  cRetainableTest *retainable_test = new cRetainableTest;
  if(retainable_test){
    retainable_test->nameWatched("(cRetainableTest) this memory leak is on purpose.");
  } else {
    avdError
    << "Couldn't allocate \"cRetainableTest\" object.\n"
    << "Normally this object, once created, is never destroyed; it\n"
    << "creates a small memory leak which, hopefully,\n"
    << "the memory management system will detect.\n";
  }
  retain_watch.nameWatched("<main(retain_watch)>");
  QApplication app( argc, argv );
  avdMissionControl *mc = new avdMissionControl(argc, argv, 0, "<main(mc)>");
  if(!mc)
    avdFatal
    << "Couldn't allocate \"avdMissionControl\" object.\n"
    << "Normally that object manages the construction of the remainder\n"
    << "of this program, including the graphic user interface and\n"
    << "the processing part of Avida.  Since the object couldn't be\n"
    << "created, this program has to abort.";
  mc->nameWatched("<main(mc)>");
  app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  int result = app.exec();
  mc->release();
  avdDebug("out.");
  return result;
}

// arch-tag: implementation file for main function
/* vim: set ts=2 ai et: */
