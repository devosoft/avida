#ifndef QLINEEDIT_H
#include <qlineedit.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif

#ifndef AVD_EXP_VIEW_HH
#include "avd_exp_view.hh"
#endif


avdExpView::avdExpView( QWidget *parent, const char *name, WFlags f)
:QMainWindow(parent, name, f){
  QWidget *central = new QWidget(
    this, "<avdExpView(central)>");
  setCentralWidget(central);
  QVBoxLayout *layout = new QVBoxLayout(
    central, 10, -1, "<avdExpView(layout)>");
  m_scopeguard_test_btn = new QPushButton(
    "Test ScopeGuard", central, "<avdExpView::m_scopeguard_test_btn>");
  layout->addWidget(m_scopeguard_test_btn);
}

// arch-tag: implementation file for devel-experimentation gui view
