

#ifndef AVD_HELP_VIEWER_HH
#include "avd_help_viewer.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QMENUBAR_H
#include <qmenubar.h>
#endif
#ifndef QPOPUPMENU_H
#include <qpopupmenu.h>
#endif
#ifndef QSTATUSBAR_H
#include <qstatusbar.h>
#endif
#ifndef QSTRINGLIST_H
#include <qstringlist.h>
#endif
#ifndef QTEXTBROWSER_H
#include <qtextbrowser.h>
#endif
#ifndef QTOOLBUTTON_H
#include <qtoolbutton.h>
#endif
#ifndef QWIDGET_H
#include <qwidget.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


avd_HelpViewer::avd_HelpViewer(
  avd_MissionControl &mission_control,
  QWidget *parent,
  const char *name,
  WFlags f
)
: QMainWindow(parent, name, f),
  m_mission_control(mission_control)
{
  GenDebug("entered.");

  QWidget *central_widget = new QWidget(
    this,
    "<avd_HelpViewer::avd_HelpViewer(central_widget)>"
  );
  setCentralWidget(central_widget);


  /*
  lay out subwidgets of central widget
  */
  QVBoxLayout *vlayout = new QVBoxLayout(
    central_widget,
    11,
    6,
    "<avd_HelpViewer::avd_HelpViewer(vlayout)>"
  );

  m_text_browser = new QTextBrowser(
    central_widget,
    "<avd_HelpViewer::m_text_browser>"
  );
  vlayout->addWidget(m_text_browser);
  

  /*
  add menu items
  */
  QPopupMenu *file_menu = new QPopupMenu( this, "<avd_HelpViewer::avd_HelpViewer(file_menu)>");
    file_menu->insertItem( "Start Avida", &m_mission_control, SIGNAL(startAvidaSig()));
    file_menu->insertItem( "Pause Avida", &m_mission_control, SIGNAL(stopAvidaSig()));
    file_menu->insertItem( "Step Avida", &m_mission_control, SIGNAL(stepAvidaSig()));
    file_menu->insertItem( "Update Avida", &m_mission_control, SIGNAL(updateAvidaSig()));

    file_menu->insertSeparator();

    file_menu->insertItem( "Close Help Viewer", this, SLOT(close()), CTRL+Key_W);
    file_menu->insertItem( "Quit", this, SIGNAL(quitSig()), CTRL+Key_Q);
  menuBar()->insertItem( "File", file_menu);

  QPopupMenu *nav_menu = new QPopupMenu( this, "<avd_HelpViewer::avd_HelpViewer(nav_menu)>");
    nav_menu->insertItem( "Back", m_text_browser, SLOT(backward()));
    nav_menu->insertItem( "Next", m_text_browser, SLOT(forward()));
    nav_menu->insertItem( "Home", m_text_browser, SLOT(home()));
    nav_menu->insertItem( "Reload", m_text_browser, SLOT(reload()));
  menuBar()->insertItem( "Navigate", nav_menu);

  QPopupMenu *view_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(view_menu)>");
    view_menu->insertItem( "New Map Viewer", &m_mission_control, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", &m_mission_control, SIGNAL(newInstructionViewerSig()));
    view_menu->insertItem( "Event Viewer", &m_mission_control, SIGNAL(newEventViewerSig()));
    //view_menu->insertItem( "Control Box", &m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", &m_mission_control, SIGNAL(newPlotViewerSig()));
  menuBar()->insertItem( "Viewers", view_menu);

  QPopupMenu *help_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(help_menu)>");
    help_menu->insertItem("Documentation Contents", this,
      SLOT(helpContents()));
    help_menu->insertSeparator();
    help_menu->insertItem("Setting up the Genesis File", this,
      SLOT(helpGenesis()));
    help_menu->insertItem("Setting up the Instruction Set", this,
      SLOT(helpInstructionSet()));
    help_menu->insertItem("Guidelines to setting Events", this,
      SLOT(helpEvents()));
    help_menu->insertItem("Guidelines to setting up Environments", this,
      SLOT(helpEnvironments()));
    help_menu->insertItem("Guidelines to Analyze Mode", this,
      SLOT(helpAnalyzeMode()));

  menuBar()->insertItem( "Help", help_menu);

  /*
  connects
  */
  connect(
    m_text_browser, SIGNAL(highlighted(const QString &)),
    statusBar(), SLOT(message(const QString &))
  );
  connect(
    this, SIGNAL(userSettingsSig(void)),
    &m_mission_control, SIGNAL(userSettingsSig(void))
  );
  connect(
    this, SIGNAL(quitSig(void)),
    &m_mission_control, SIGNAL(quitSig(void))
  );

  GenDebug("done.");
}

avd_HelpViewer::~avd_HelpViewer(){
  GenDebug("entered.");
  GenDebug("done.");
}


QStringList
avd_HelpViewer::getPath(void){
  GenDebug("entered.");
  QString result(m_text_browser->mimeSourceFactory()->filePath().front());
  GenDebug("done.");
  return result;
}

void
avd_HelpViewer::setPath(const QStringList &path){
  GenDebug("entered.");

  m_text_browser->mimeSourceFactory()->setFilePath(QStringList(path));

  GenDebug("done.");
}

void
avd_HelpViewer::setSource(const QString &source){
  GenDebug("entered.");

  m_text_browser->setSource(source);

  GenDebug("done.");
}


void
avd_HelpViewer::helpContents(){
  m_mission_control.emitHelpURLSig("index.html");
}
void
avd_HelpViewer::helpGenesis(){
  m_mission_control.emitHelpURLSig("genesis.html");
}
void
avd_HelpViewer::helpInstructionSet(){
  m_mission_control.emitHelpURLSig("inst_set.html");
}
void
avd_HelpViewer::helpEvents(){
  m_mission_control.emitHelpURLSig("events.html");
}
void
avd_HelpViewer::helpEnvironments(){
  m_mission_control.emitHelpURLSig("environment.html");
}
void
avd_HelpViewer::helpAnalyzeMode(){
  m_mission_control.emitHelpURLSig("analyze_mode.html");
}




/* vim: set ts=2 ai et: */

