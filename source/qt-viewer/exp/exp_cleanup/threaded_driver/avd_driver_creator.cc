#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_ORIG_DRIVER_CONTROLLER_HH
#include "avd_orig_driver_controller.hh"
#endif
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif
#ifndef AVD_AVIDA_THREAD_DRVR_DBG_HH
#include "avd_avida_thread_drvr_dbg.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif


avdMessageType  Info_THRD_DRVR_avdMsg("ThreadHdlr", avdMCInfo);
avdMessageType Debug_THRD_DRVR_avdMsg("ThreadHdlr", avdMCDebug);
avdMessageType Error_THRD_DRVR_avdMsg("ThreadHdlr", avdMCError);
avdMessageType Fatal_THRD_DRVR_avdMsg("ThreadHdlr", avdMCFatal);


template <class avdAvidaThreadCtrl> class pAvidaThreadDrvrConnections {
protected:
  void setupConnections(avdMCMediator *mdtr, avdAvidaThreadCtrl *hdlr){
    Debug << "in .";
    avdConnections cnct(mdtr->getAvidaStateMediator(), hdlr);
    cnct.add(SIGNAL(doOpenAvidaSig()), SLOT(openAvidaSlot()));
    cnct.add(SIGNAL(doCloseAvidaSig()), SLOT(closeAvidaSlot()));
    //cnct.add(SIGNAL(doResetAvidaSig()), SLOT(resetAvidaSlot()));

    cnct.add(SIGNAL(doStartAvidaSig()), SIGNAL(doStartAvidaSig()));
    cnct.add(SIGNAL(doPauseAvidaSig()), SIGNAL(doPauseAvidaSig()));
    cnct.add(SIGNAL(doUpdateAvidaSig()), SIGNAL(doUpdateAvidaSig()));
    cnct.add(SIGNAL(doStepAvidaSig(int)), SIGNAL(doStepAvidaSig(int)));
    
    avdConnections cnct2(hdlr, mdtr->getAvidaStateMediator());
    cnct2.add(SIGNAL(avidaUpdatedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaUpdatedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaUpdatedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaSteppedSig(cAvidaDriver_Population *, int)),
      SIGNAL(avidaSteppedSig(cAvidaDriver_Population *, int)));
    cnct2.add(SIGNAL(avidaSteppedSig(cAvidaDriver_Population *, int)),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaBreakSig(cAvidaDriver_Population *, int)),
      SIGNAL(avidaBreakSig(cAvidaDriver_Population *, int)));
    cnct2.add(SIGNAL(avidaBreakSig(cAvidaDriver_Population *, int)),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaOpenedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaOpenedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaOpenedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaClosedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaClosedSig(cAvidaDriver_Population *)));
    cnct2.add(SIGNAL(avidaClosedSig(cAvidaDriver_Population *)),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)));

    QObject::connect(hdlr, SIGNAL(requestArgvSig(aReqArgcArgv &)),
      mdtr->getSettingsMediator(), SIGNAL(requestArgvSig(aReqArgcArgv &)));
} };
typedef tGUI<avdAvidaThreadCtrl, pAvidaThreadDrvrConnections> avdAvidaThreadHdlr;

namespace {
  avdAbstractCtrl *createDriverHdlr(void){
    avdAvidaThreadHdlr *m = new avdAvidaThreadHdlr(0, "<avdAvidaThreadHdlr>");
    m->nameWatched("<avdAvidaThreadHdlr>");
    m->getCtrl()->nameWatched("<avdAvidaThreadCtrl>");
    return m; }
  QString s_key("avdAvidaThreadHdlr");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createDriverHdlr);

  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_THRD_DRVR_avdMsg);
        m_msg_types->Register("Debug", &Debug_THRD_DRVR_avdMsg);  
        m_msg_types->Register("Error", &Error_THRD_DRVR_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_THRD_DRVR_avdMsg);
        avdMessageSetTracker::Instance().Register("ThreadHdlr", m_msg_types);
    } } 
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("ThreadHdlr");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}

// arch-tag: implementation file for avida processing thread factory and debug messages
