#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QLINEEDIT_H
#include <qlineedit.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif

#ifndef AVD_USER_MSG_VIEW_HH
#include "avd_user_msg_view.hh"
#endif


avdUserMsgView::avdUserMsgView(QWidget *parent, const char *name, WFlags f)
:QMainWindow(parent, name, f){
  QWidget *central = new QWidget(this, "<avdUserMsgView(central)>");
  setCentralWidget(central);
  QVBoxLayout *layout = new QVBoxLayout(central, 10, -1, "<avdUserMsgView(layout)>");
  m_text_edit = new QTextEdit(central, "<avdUserMsgView::m_text_edit>");
  m_text_edit->setReadOnly(true);
  layout->addWidget(m_text_edit);
}

// arch-tag: implementation file for user debug-message display view
