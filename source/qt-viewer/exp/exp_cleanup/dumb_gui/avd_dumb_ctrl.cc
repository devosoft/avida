#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif

#ifndef AVD_DUMB_GUI_DBG_HH
#include "avd_dumb_gui_dbg.hh"
#endif
#ifndef AVD_DUMB_VIEW_HH
#include "avd_dumb_view.hh"
#endif

#ifndef AVD_DUMB_CTRL_HH
#include "avd_dumb_ctrl.hh"
#endif


void avdDumbCtrl::setView(avdDumbView *view){ SETretainable(m_view, view); }

avdDumbCtrl::avdDumbCtrl(QObject *parent, const char *name)
:QObject(parent, name){
  DumbGuiDebug << "constructor; creating view.";
  avdDumbView *view = new avdDumbView(0, "<avdDumbCtrl(m_view)>");
  setView(view); view->release();
  getView()->nameWatched("<avdDumbCtrl(m_view)>");
  connect(getView(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
  connect(
    getView()->startBtn(), SIGNAL(clicked()),
    this, SIGNAL(doStartAvidaSig()));
  connect(
    getView()->pauseBtn(), SIGNAL(clicked()),
    this, SIGNAL(doPauseAvidaSig()));
  connect(
    getView()->updateBtn(), SIGNAL(clicked()),
    this, SIGNAL(doUpdateAvidaSig()));

  connect(
    getView()->resetBtn(), SIGNAL(clicked()),
    this, SIGNAL(doResetAvidaSig()));
  connect(
    getView()->openBtn(), SIGNAL(clicked()),
    this, SIGNAL(doOpenAvidaSig()));
  connect(
    getView()->closeBtn(), SIGNAL(clicked()),
    this, SIGNAL(doCloseAvidaSig()));

  DumbGuiDebug << "created view; done.";
}
avdDumbCtrl::~avdDumbCtrl(){
  setView(0);
  DumbGuiDebug << "entered empty destructor.";
}
bool avdDumbCtrl::setup(){
  DumbGuiDebug << "entered stub method.";
  emit(doResetMenuBar(getView()->menuBar()));
  getView()->show();
  return true;
}
void avdDumbCtrl::avidaStateChangedSlot(cAvidaDriver_Population *pop){ }

// arch-tag: implementation file for dumb gui controller
