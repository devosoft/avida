#ifndef AVD_USER_MSG_CTRL_HH
#define AVD_USER_MSG_CTRL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif
#ifndef AVD_USER_MSG_CTRL_FUNCT_HH
#include "avd_user_msg_ctrl_funct.hh"
#endif


class QCustomEvent;
class QMenuBar;
class avdAvidaThreadDrvr;
class avdUserMsgDply;
class avdUserMsgView;
class avdUserMsgCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdUserMsgView> m_view;
  avdUserMsgDply *m_dply;
  int m_saved_lines;
public:
  void setView(avdUserMsgView *view);
  void setDply(avdUserMsgDply *dply);
  void setSavedLines(int saved_lines){ m_saved_lines = saved_lines; trimTextEdit(); }

  avdUserMsgView *getView(){ return m_view; }
  avdUserMsgDply *getDply(){ return m_dply; }
  int savedLines(){ return m_saved_lines; }
public:
  avdUserMsgCtrl(QObject *parent = 0, const char *name = 0);
  ~avdUserMsgCtrl();
  bool setup();
  void customEvent(QCustomEvent *e);
  void trimTextEdit();
public slots:
  void userMsgPrefsChangedSlot(avdUserMsgCtrlFunct &funct);
  void showUserMsgGUISlot();
signals:
  void requestUserMsgPrefsSig(const avdUserMsgCtrl &ctrl);
  void doResetMenuBar(QMenuBar *);
};

#endif

// arch-tag: header file for user debug-message display controller
