#ifndef AVD_USER_MSG_VIEW_HH
#define AVD_USER_MSG_VIEW_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QLineEdit;
class QPushButton;
class QTextEdit;
class avdUserMsgView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<QTextEdit> m_text_edit;
public:
  avdUserMsgView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel
  );
  QTextEdit *textEdit(){ return m_text_edit; }
};

#endif

// arch-tag: header file for user debug-message display view
