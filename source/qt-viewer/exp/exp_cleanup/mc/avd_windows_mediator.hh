#ifndef AVD_WINDOWS_MEDIATOR_HH
#define AVD_WINDOWS_MEDIATOR_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avdWindowsMediator : public QObject {
  Q_OBJECT
public:
  avdWindowsMediator(
    QObject *parent = 0, const char *name = "(avdWindowsMediator)")
  :QObject(parent, name){}
signals:
  void showUserMsgGUISig();
  void showPrefsGUISig();
};

#endif

// arch-tag: header file for windows mediator
