#ifndef AVD_ABSTRACT_PREFS_GUI_HH
#include "prefs_gui_factory/avd_abstract_prefs_gui.hh"
#endif
#ifndef AVD_PREFS_ITEM_VIEW_HH
#include "preferences/avd_prefs_item_view.hh"
#endif

#ifndef AVD_PREFS_ITEM_HH
#include "avd_prefs_item.hh"
#endif


void avdPrefsItem::setPrefsGUI(avdAbstractPrefsCtrl *prefs_gui)
{ SETretainable(m_prefs_gui, prefs_gui); }

// arch-tag: implementation file for preferences item object
