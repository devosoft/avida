#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef AVD_MAP_CTRL_HH
#include "map_gui/avd_map_ctrl.hh"
#endif
#ifndef AVD_MAP_GUI_DBG_HH
#include "map_gui/avd_map_gui_dbg.hh"
#endif
#ifndef AVD_MAP_VIEW_HH
#include "map_gui/avd_map_view.hh"
#endif
#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_MENUBAR_MEDIATOR_HH
#include "mc/avd_menubar_mediator.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif

/* Definitions of debugging messages. */
avdMessageType  Info_MAP_GUI_avdMsg("MapGUI", avdMCInfo);
avdMessageType Debug_MAP_GUI_avdMsg("MapGUI", avdMCDebug);
avdMessageType Error_MAP_GUI_avdMsg("MapGUI", avdMCError);
avdMessageType Fatal_MAP_GUI_avdMsg("MapGUI", avdMCFatal);

template <class avdMapCtrl> class pMapConnections {
protected:
  void setupConnections(avdMCMediator *med, avdMapCtrl *ctrl){
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
    QObject::connect(
      med->getAvidaStateMediator(),
      SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)),
      ctrl, SLOT(avidaStateChangedSlot(cAvidaDriver_Population *)));
} };
typedef tGUI<avdMapCtrl, pMapConnections> avdMapGUI;

namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_MAP_GUI_avdMsg);
        m_msg_types->Register("Debug", &Debug_MAP_GUI_avdMsg);
        m_msg_types->Register("Error", &Error_MAP_GUI_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_MAP_GUI_avdMsg);
        avdMessageSetTracker::Instance().Register("MapGUI", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("MapGUI");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;

  /* Registers avdMapGUI creator with GUIFactory. */
  avdAbstractCtrl *createMapGUI(void){
    avdMapGUI *g = new avdMapGUI(0, "<(avdMapGUI)>");
    g->getCtrl()->nameWatched("<createMapGUI(avdMapCtrl)>");
    return g;
  }
  QString s_key("avdMapGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createMapGUI);
}

// arch-tag: implementation file for map gui factory and debug messages
