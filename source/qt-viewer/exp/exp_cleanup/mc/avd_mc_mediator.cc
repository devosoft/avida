#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif

#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
#endif
#ifndef AVD_GUI_FACTORY_MEDIATOR_HH
#include "mc/avd_gui_factory_mediator.hh"
#endif
#ifndef AVD_MENUBAR_MEDIATOR_HH
#include "mc/avd_menubar_mediator.hh"
#endif
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef AVD_WINDOWS_MEDIATOR_HH
#include "mc/avd_windows_mediator.hh"
#endif

#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif

class avdMCMediator_prv : public cRetainable {
public:
  avdGUIFactoryMediator m_gui_factory_mediator;
  avdAvidaStateMediator m_avida_state_mediator;
  avdMenuBarMediator m_menubar_mediator;
  avdSettingsMediator m_settings_mediator;
  avdWindowsMediator m_windows_mediator;
};

avdMCMediator::avdMCMediator(QObject *parent, const char *name)
: QObject(parent, name), m_d(0)
{ avdMCMediator_prv *d = new avdMCMediator_prv; setD(d)->release(); }
avdMCMediator::~avdMCMediator()
{ setD(0); }
avdMCMediator_prv *avdMCMediator::setD(avdMCMediator_prv *d)
{ SETretainable(m_d, d); return D(); }

avdAvidaStateMediator *avdMCMediator::getAvidaStateMediator()
{ return &D()->m_avida_state_mediator; }
avdGUIFactoryMediator *avdMCMediator::getGUIFactoryMediator()
{ return &D()->m_gui_factory_mediator; }
avdMenuBarMediator *avdMCMediator::getMenuBarMediator()
{ return &D()->m_menubar_mediator; }
avdSettingsMediator *avdMCMediator::getSettingsMediator()
{ return &D()->m_settings_mediator; }
avdWindowsMediator *avdMCMediator::getWindowsMediator()
{ return &D()->m_windows_mediator; }

// arch-tag: implementation file for mission-control mediator
