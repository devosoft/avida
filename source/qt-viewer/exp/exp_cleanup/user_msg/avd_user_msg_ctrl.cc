#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QEVENT_H
#include <qevent.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif

#ifndef AVD_USER_MSG_GUI_DBG_HH
#include "avd_user_msg_gui_dbg.hh"
#endif
#ifndef AVD_USER_MSG_VIEW_HH
#include "avd_user_msg_view.hh"
#endif

#ifndef AVD_USER_MSG_CTRL_HH
#include "avd_user_msg_ctrl.hh"
#endif


class avdUserMsgEvent : public QCustomEvent {
public:
  avdUserMsgEvent(const QString &msg):QCustomEvent(1002), m_msg(msg){}
  QString msg() const { return m_msg; }
protected:
  QString m_msg;
};

class avdUserMsgDply : public avdMessageDisplay, public cRetainable {
protected:
  avdUserMsgCtrl *m_msg_rcvr;
public:
  avdUserMsgDply(avdUserMsgCtrl *msg_rcvr):m_msg_rcvr(msg_rcvr)
  {
    setAvdInfoDisplay(*this);
    setAvdDebugDisplay(*this);
    setAvdErrorDisplay(*this);
    setAvdFatalDisplay(*this);
    setAvdPlainDisplay(*this);
  }
  ~avdUserMsgDply(){
    setAvdInfoDisplay(s_avd_info_msg_out);
    setAvdDebugDisplay(s_avd_debug_msg_out);
    setAvdErrorDisplay(s_avd_error_msg_out);
    setAvdFatalDisplay(s_avd_fatal_msg_out);
    setAvdPlainDisplay(s_avd_plain_msg_out);
  }
  void out(const cString &final_msg){
    avdUserMsgEvent *msg_ev = new avdUserMsgEvent(QString(final_msg));
    QApplication::postEvent(m_msg_rcvr, msg_ev);
  }
  /*
  XXX FIXME:  abort() below fails bcz of multithreading.  Should try to
  quit processing thread if possible, even though avida is now unstable.
  Easiest to try to signal to mediator, so this class should become a
  QObject, and be connected to UserMsgCtrl to pass an abort signal up.
  -- K
  */
  void abort(){ QApplication::exit(1); }
};

/*
Don't call any Message macros in the next two functions,
or you'll trigger an infinite loop.
*/
void avdUserMsgCtrl::customEvent(QCustomEvent *e){
  if(e->type() == 1002){
    avdUserMsgEvent *msg_ev = (avdUserMsgEvent *)e;
    getView()->textEdit()->append(msg_ev->msg());   // append to info window
    trimTextEdit();
    //std::cout << msg_ev->msg() << std::endl;        // and echo to stdout
  }
}
void avdUserMsgCtrl::trimTextEdit(){
  if(getView() && getView()->textEdit())
    for(int sl = savedLines(), p = getView()->textEdit()->paragraphs(); sl < p; p--)
      getView()->textEdit()->removeParagraph(0);
}

void avdUserMsgCtrl::setView(avdUserMsgView *view)
{ SETretainable(m_view, view); }
void avdUserMsgCtrl::setDply(avdUserMsgDply *dply)
{ SETretainable(m_dply, dply); }

avdUserMsgCtrl::avdUserMsgCtrl(QObject *parent, const char *name)
:QObject(parent, name), m_view(0), m_dply(0){
  Debug << "user message system controller constructor.";
  avdUserMsgView *view = new avdUserMsgView(0, "<avdUserMsgCtrl(m_view)>");
  setView(view); view->release();
  getView()->nameWatched("<avdUserMsgCtrl(m_view)>");
  connect(getView(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
  Debug << "Transferring message output from stdout/stderr to info console.";
  avdUserMsgDply *dply = new avdUserMsgDply(this);
  setDply(dply); dply->release();
  getDply()->nameWatched("<avdUserMsgCtrl(m_dply)>");
  Debug << "<avdUserMsgCtrl> setSavedLines(1000)...";
  setSavedLines(1000);
  Debug << "<avdUserMsgCtrl> ctor done.";
}
avdUserMsgCtrl::~avdUserMsgCtrl(){
  avdDebug << "user message system controller destructor.";
  setDply(0);
  setView(0);
  Debug << "Transferred message output from info console to stdout/stderr.";
  avdDebug << "user message system controller dtor done.";
}
bool avdUserMsgCtrl::setup(){
  Info << "entered message system setup.";
  emit(doResetMenuBar(getView()->menuBar()));
  emit(requestUserMsgPrefsSig(*this));
  Info << "message system setup done.";
  return true;
}

void avdUserMsgCtrl::userMsgPrefsChangedSlot(avdUserMsgCtrlFunct &funct){
  Debug << "entered userMsgPrefsChangedSlot(avdUserMsgCtrlFunct &).";
  funct(*this);
  Debug << "userMsgPrefsChangedSlot(avdUserMsgCtrlFunct &) done.";
}

void avdUserMsgCtrl::showUserMsgGUISlot(){
  Debug << "<showUserMsgGUISlot>";
  if(getView()){
    Debug << "<showUserMsgGUISlot> showing, raising...";
    getView()->show();
    getView()->raise();
  }
  Debug << "<showUserMsgGUISlot> done.";
}

// arch-tag: implementation file for user debug-message display controller
