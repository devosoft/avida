

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif


int main( int argc, char **argv ){
  qDebug("<main>");
  
  int result;
  QApplication a( argc, argv );
  
  avd_MissionControl *mc = new avd_MissionControl(
    argc,
    argv,
    0,
    "<main(mc)>"
  );
  
  a.connect(
    &a, SIGNAL(lastWindowClosed()),
    &a, SLOT(quit())
  );
  
  result = a.exec();
  
  qDebug("<main> done.");
  return result;
}


/* vim: set ts=2 ai et: */

