#ifndef QMENUBAR_H
#include <qmenubar.h>
#endif
#ifndef QPOPUPMENU_H
#include <qpopupmenu.h>
#endif

#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif

#ifndef AVD_MENUBAR_HANDLER_HH
#include "avd_menubar_handler.hh"
#endif


void avdMenuBarCtrl::setupMenuBarSlot(QMenuBar *menubar){
  if(menubar){
      QPopupMenu *simulation_menu = new QPopupMenu(
        menubar, "<avdMenuBarCtrl::setupMenuBar(simulation_menu)");

      simulation_menu->insertItem(
        "Start", this, SIGNAL(doStartAvidaSig()), CTRL+SHIFT+Key_U);
      simulation_menu->insertItem(
        "Pause", this, SIGNAL(doPauseAvidaSig()), CTRL+SHIFT+Key_P);
      simulation_menu->insertItem(
        "Step", this, SIGNAL(doStepAvidaSig()), CTRL+SHIFT+Key_Comma);
      simulation_menu->insertItem(
        "Update", this, SIGNAL(doUpdateAvidaSig()), CTRL+SHIFT+Key_Period);

      simulation_menu->insertSeparator();

      if(avdControllerFactory::Instance().IsRegistered("avdDumbGUI"))
      { simulation_menu->insertItem(
          "Dumb Controls", this, SLOT(newDumbGUI()));
      }
      if(avdControllerFactory::Instance().IsRegistered("avdExpGUI"))
      { simulation_menu->insertItem(
          "Exp Controls", this, SLOT(newExpGUI()));
      }
      if(avdControllerFactory::Instance().IsRegistered("avdMapGUI"))
      { simulation_menu->insertItem(
          "Map", this, SLOT(newMapGUI()));
      }
      if(avdControllerFactory::Instance().IsRegistered("avdAnalyzerGUI"))
      { simulation_menu->insertItem(
          "New Analyzer", this, SLOT(newAnalyzerControllerSlot()));
      }
      if(avdControllerFactory::Instance().IsRegistered("avd_o_AnalyzerCtrl"))
      { simulation_menu->insertItem(
          "Original Analyzer", this, SLOT(new_o_AnalyzerControllerSlot()));
      }
      simulation_menu->insertSeparator();
      if(avdControllerFactory::Instance().IsRegistered("avdUserMsgGUI"))
      { simulation_menu->insertItem(
          "Log Window", this, SIGNAL(showUserMsgGUISig()));
      }
      if(avdControllerFactory::Instance().IsRegistered("avdPrefsGUI"))
      { simulation_menu->insertItem(
          "Settings", this, SIGNAL(showPrefsGUISig()));
      }

    menubar->insertItem("Simulation", simulation_menu);
} }

void avdMenuBarCtrl::newDumbGUI(void)
{ emit newGUIControllerSig("avdDumbGUI"); }
void avdMenuBarCtrl::newExpGUI(void)
{ emit newGUIControllerSig("avdExpGUI"); }
void avdMenuBarCtrl::newMapGUI(void)
{ emit newGUIControllerSig("avdMapGUI"); }
void avdMenuBarCtrl::newAnalyzerControllerSlot(void)
{ emit newGUIControllerSig("avdAnalyzerGUI"); }
void avdMenuBarCtrl::new_o_AnalyzerControllerSlot(void)
{ emit newGUIControllerSig("avd_o_AnalyzerCtrl"); }

// arch-tag: implementation file for menubar handler
