#ifndef QWIDGETSTACK_H
#include <qwidgetstack.h>
#endif

#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_PREFS_DBG_HH
#include "avd_prefs_dbg.hh"
#endif
#ifndef AVD_PREFS_GUI_FACTORY_HH
#include "prefs_gui_factory/avd_prefs_gui_factory.hh"
#endif
#ifndef AVD_PREFS_ITEM_VIEW_HH
#include "preferences/avd_prefs_item_view.hh"
#endif
#ifndef AVD_PREFS_ITEM_HH
#include "avd_prefs_item.hh"
#endif
#ifndef AVD_PREFS_VIEW_HH
#include "avd_prefs_view.hh"
#endif

#ifndef AVD_PREFS_CTRL_HH
#include "avd_prefs_ctrl.hh"
#endif


void avdPrefsCtrl::setMdtr(avdMCMediator *mdtr){ SETretainable(m_mdtr, mdtr); }
void avdPrefsCtrl::setView(avdPrefsView *view){ SETretainable(m_view, view); }
void avdPrefsCtrl::setPrefsItemsList(tRetainableList<avdPrefsItem> *itemlist)
{ SETretainable(m_prefs_items, itemlist); }

avdPrefsCtrl::avdPrefsCtrl(QObject *parent, const char *name)
: QObject(parent, name), m_view(0), m_prefs_items(0), m_mdtr(0) {
  avdPrefsView *view = new avdPrefsView(0, "<avdPrefsCtrl(m_view)>");
  setView(view);
  if(!getView()){ Error << "can't create avdPrefsView."; return; }
  view->release();
  getView()->nameWatched("<avdPrefsCtrl::m_view>");

  tRetainableList<avdPrefsItem> *prefs_items = new tRetainableList<avdPrefsItem>;
  setPrefsItemsList(prefs_items);
  if(!getPrefsItemsList()){ Error << "can't create tRetainableList."; return; }
  prefs_items->release();
  getPrefsItemsList()->nameWatched("<avdPrefsCtrl::m_prefs_items>");
}
avdPrefsCtrl::~avdPrefsCtrl(){
  setMdtr(0);
  /*
  The order of next two set(0)'s (i.e., deletions) is important.
  Deleting the PrefsItemsList results in deletion of its PrefsItems.
  This causes the QListBox in the View to forget about the PrefsItems,
  so it won't later try to double-free them.
  
  If the View is deleted first, its QListBox will delete the PrefsItems,
  but PrefsItemsList won't know that the PrefsItems have already been
  deleted, and consequently will double-free them.
  */
  setPrefsItemsList(0);
  setView(0);
}

bool avdPrefsCtrl::setup(){
  emit(doResetMenuBar(getView()->menuBar()));
  if(avdPrefsControllerFactory::Instance().IsRegistered("avdUserMsgPrefsGUI")){
    // Create a pref-pane gui
    avdAbstractPrefsCtrl *c =
      avdPrefsControllerFactory::Instance().CreateObject("avdUserMsgPrefsGUI");
    if(!c) { Error << "couldn't create avdUserMsgPrefsGUI."; }
    else {
      c->setup(getMdtr());
      // Create a pref-pane item
      avdPrefsItem *pi = new avdPrefsItem(getView()->listBox(), "Info Console");
      if(!pi){ Error << "couldn't create avdPrefsItem."; }
      else {
        //m_prefs_items.Push(pi);
        getPrefsItemsList()->Push(pi);
        pi->setPrefsGUI(c);
        pi->release();
      }
      c->release();
  } }

  //tRetainableListIter<avdPrefsItem> it(m_prefs_items);
  tRetainableListIter<avdPrefsItem> it(*getPrefsItemsList());
  it.Reset();
  while(it.Next())
  { getView()->widgetStack()->addWidget(it.Get()->getPrefsGUI()->getView()); }
  // XXX
  //if(m_prefs_items.GetSize() > 0)
  if(getPrefsItemsList()->GetSize() > 0)
  //{ getView()->widgetStack()->raiseWidget(m_prefs_items.GetFirst()->getPrefsGUI()->getView()); }
  { getView()->widgetStack()->raiseWidget(getPrefsItemsList()->GetFirst()->getPrefsGUI()->getView()); }
  return true;
}

void avdPrefsCtrl::showPrefsGUISlot(){
  Debug << "<showPrefsGUISlot>";
  if(getView()){
    Debug << "<showPrefsGUISlot> showing, raising...";
    getView()->show();
    getView()->raise();
  }
  Debug << "<showPrefsGUISlot> done.";
}

// arch-tag: implementation file for preferencess gui controller
