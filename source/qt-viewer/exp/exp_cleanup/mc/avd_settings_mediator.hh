#ifndef AVD_SETTINGS_MEDIATOR_HH
#define AVD_SETTINGS_MEDIATOR_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_USER_MSG_CTRL_FUNCT_HH
#include "user_msg/avd_user_msg_ctrl_funct.hh"
#endif


class aReqArgcArgv;
class cTestInterface;
class avdUserMsgCtrl;

class avdSettingsMediator : public QObject {
  Q_OBJECT
public:
  avdSettingsMediator(
    QObject *parent = 0, const char *name = "(avdSettingsMediator)")
  :QObject(parent, name){}
signals:
  void requestArgvSig(aReqArgcArgv &receiver);
  void settingsChangedSig();
  void userMsgPrefsChangedSig();
  void userMsgPrefsChangedSig(avdUserMsgCtrlFunct &funct);
  void requestUserMsgPrefsSig(const avdUserMsgCtrl &ctrl);

  void doSendSig(cTestInterface *test_ifc);
};

#endif

// arch-tag: header file for settings mediator
