#ifndef AVD_USER_MSG_PREFS_CTRL_HH
#define AVD_USER_MSG_PREFS_CTRL_HH

#ifndef QFONT_H
#include <qfont.h>
#endif
#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif
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



class avdMessageType;
class avdMsgModuleLVI : public QListViewItem, public cRetainable
{
public:
  avdMsgModuleLVI(
    QListView *parent,
    QString label                 // e.g. 'UsrMsgSys'
  );
};
class avdMsgTypeLVI : public QListViewItem, public cRetainable {
public:
  avdMsgTypeLVI(
    avdMsgModuleLVI *parent,
    const QString &parent_text,   // e.g. 'UsrMsgSys'
    const QString &text,          // e.g. 'Debug'
    avdMessageType &mt
  );
};
class avdMsgCnfgCLI : public QCheckListItem, public cRetainable {
protected:
  bool &m_flag;
  QString m_key;
protected:
  virtual void stateChange(bool state);
public:
  avdMsgCnfgCLI(
    avdMsgTypeLVI *parent,
    const QString &text,
    const QString &key,
    bool &flag
  );
};

class avdPrefsItemView;
class avdUserMsgCtrl;
class avdUserMsgPrefsView;
class avdUserMsgPrefsCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdUserMsgPrefsView> m_view;
  QFont m_console_font;
  int m_saved_lines;
public:
  void setView(avdUserMsgPrefsView *);
  void setConsoleFont(QFont console_font);
  void setSavedLines(int saved_lines){ m_saved_lines = saved_lines; }

  QWidget *getView();
  avdUserMsgPrefsView *getUserMsgPrefsView();
  QFont consoleFont() const { return m_console_font; }
  int savedLines(){ return m_saved_lines; }
public:
  avdUserMsgPrefsCtrl(QObject *parent = 0, const char *name = 0);
  ~avdUserMsgPrefsCtrl();
  void setup();
  void consoleFontFunctor(const avdUserMsgCtrl &user_msg_ctrl);
  void savedLinesFunctor(const avdUserMsgCtrl &user_msg_ctrl);
public slots:
  void requestUserMsgPrefsSlot(const avdUserMsgCtrl &);
  void fontBtnClickedSlot();
  void spinBoxValueChangedSlot(int value);
signals:
  void userMsgPrefsChangedSig(avdUserMsgCtrlFunct &funct);
};

#endif

// arch-tag: header file for user debug-message display gui preferences controller
