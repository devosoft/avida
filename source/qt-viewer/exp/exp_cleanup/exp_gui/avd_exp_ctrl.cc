#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef AVD_EXP_GUI_DBG_HH
#include "avd_exp_gui_dbg.hh"
#endif
#ifndef AVD_EXP_VIEW_HH
#include "avd_exp_view.hh"
#endif
#ifndef TEST_THING_HH
#include "exp_gui/test_thing.hh"
#endif

#ifndef AVD_EXP_CTRL_HH
#include "avd_exp_ctrl.hh"
#endif


void avdExpCtrl::setView(avdExpView *view){ SETretainable(m_view, view); }

avdExpCtrl::avdExpCtrl(QObject *parent, const char *name)
:QObject(parent, name) {
  Debug << "<avdExpCtrl> ctor; creating view.";
  avdExpView *view = new avdExpView(0, "<avdExpCtrl(m_view)>");
  setView(view); view->release();
  view->nameWatched("<avdExpCtrl::m_view>");
  getView()->nameWatched("<avdExpCtrl(m_view)>");
  connect(getView(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
  connect(getView()->scopeguardTestBtn(), SIGNAL(clicked()), this, SLOT(scopeguardTestBtnClickedSlot()));
  Debug << "<avdExpCtrl> created view; done.";
}
avdExpCtrl::~avdExpCtrl(){
  setView(0);
  Debug << "<avdExpCtrl> dtor.";
}
bool avdExpCtrl::setup(){
  Debug << "entered stub method.";
  emit(doResetMenuBar(getView()->menuBar()));
  getView()->show();
  return true;
}
void avdExpCtrl::scopeguardTestBtnClickedSlot(){
  Debug << "\"Test ScopeGuard\" button clicked.";
  cTestThing test_thing;
  Debug << "doing scopeguard test thing...";
  test_thing.doScopeGuardTest();
  Debug << "done.";
}

// arch-tag: implementation file for devel-experimentation gui controller
