

#include <stdlib.h>
  
#ifndef AVD_MAIN_WINDOW_HH
#include "avd_main_window.hh"
#endif

//#include "main_window.hh"
//
//#include "defs.hh"
//#include "config.hh"
//
//#include "init_file_view.hh"
#include "map_view.hh"
//#include "creature_view.hh"
//#include "cell_view_widget.hh"
//#include "n_orig_instruction_viewer.hh"
//#include "event_view_widget.hh"
////#include "godbox_widget.hh"
//#include "godbox_widget2.hh"
//#include "avida_plot_widget.hh"
//#include "plot_select_dia_impl.hh"
//
//#include "messaging_population_wrapper.hh"
//
//#include <iostream>
//#include <stdlib.h>
//#include <qpopupmenu.h>
//#include <qmenubar.h>
//#include <qapplication.h>

//#ifndef HELP_VIEWER_HH
//#include "help_viewer.hh"
//#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
    
#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
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
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QSTATUSBAR_H
#include <qstatusbar.h>
#endif
#ifndef QVBOX_H
#include <qvbox.h>
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

                
avd_MainWindow::avd_MainWindow(
  avd_MissionControl &mission_control,
  QWidget *parent,
  const char *name,
  WFlags f
)
: QMainWindow(parent, name, f),
  m_mission_control(mission_control)
{
  GenDebug("entered.");

  m_central_widget = new QWidget(
    this,
    "<avd_MainWindow::avd_MainWindow(central_widget)>"
  );
  setCentralWidget(m_central_widget);

  /*
  lay out subwidgets of central widget
  */
  m_vlayout = new QVBoxLayout(
    m_central_widget,
    11,
    6,
    "<avd_MainWindow::avd_MainWindow(vlayout)>"
  );

  connect(
    this, SIGNAL(newMainWindowSig(void)),
    &m_mission_control, SIGNAL(newMainWindowSig(void))
  );
  connect(
    this, SIGNAL(userSettingsSig(void)),
    &m_mission_control, SIGNAL(userSettingsSig(void))
  );
  connect(
    this, SIGNAL(quitSig(void)),
    &m_mission_control, SIGNAL(quitSig(void))
  );

  connect(
    &m_mission_control, SIGNAL(avidaOpenedSig(void)),
    this, SLOT(avidaOpenedSlot(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaExitedSig(void)),
    this, SLOT(avidaClosedSlot(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaResetSig(void)),
    this, SLOT(avidaResetSlot(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaUpdatedSig(void)),
    this, SLOT(avidaUpdatedSlot(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(avidaSteppedSlot(int))
  );
  connect(
    &m_mission_control, SIGNAL(avidaBreakpointSig(int)),
    this, SLOT(avidaBreakpointSlot(int))
  );
  connect(
    &m_mission_control, SIGNAL(avidaStoppedSig(void)),
    this, SLOT(avidaStoppedSlot(void))
  );

  // now create the menu bar.
  setupMenuBar();
  
  // set some initial size
  resize( 550, 400);

  // if the avida population has been created, it's safe to create a map view.
  // otherwise wait until creation of population.
  m_mission_control.lock();
  if(m_mission_control.getPopulation()){
    m_mission_control.unlock();
    setupViewer();
    m_mission_control.lock();
  }
  m_mission_control.unlock();

  GenDebug("done.");
}


avd_MainWindow::~avd_MainWindow(){
  GenDebug("entered.");
  cleanupViewer();
  GenDebug("done.");
}

void
avd_MainWindow::setupViewer()
{
  GenDebug("entered.");

  m_map_view = new MapView(&m_mission_control, m_central_widget);
  m_vlayout->addWidget(m_map_view);

  connect(
    &m_mission_control, SIGNAL(avidaUpdatedSig(void)),
    m_map_view, SLOT(repaintMap(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaSteppedSig(int)),
    m_map_view, SLOT(repaintMap(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaBreakpointSig(int)),
    m_map_view, SLOT(repaintMap(void))
  );
  connect(
    &m_mission_control, SIGNAL(avidaStoppedSig(void)),
    m_map_view, SLOT(repaintMap(void))
  );
  connect(
    m_map_view, SIGNAL(cellSelected(int)),
	  this, SLOT(setSelectedPopulationCellSlot(int))
  );
  //m_map_view->repaintMap();
  m_map_view->show();

  GenDebug("done.");
}

void
avd_MainWindow::cleanupViewer()
{
  GenDebug("entered.");
  if(m_map_view) delete m_map_view;
  GenDebug("done.");
}

void
avd_MainWindow::setupMenuBar()
{
  GenDebug("entered.");
  /*
  add menu items
  */
  int tmp_menu_id;
  QPopupMenu *file_menu = new QPopupMenu( this, "<avd_MainWindow::avd_MainWindow(file_menu)>");

    file_menu->insertItem( "Start Avida", this, SLOT(startAvidaSlot()));
    file_menu->insertItem( "Pause Avida", this, SLOT(stopAvidaSlot()));
    file_menu->insertItem( "Step Avida", this, SLOT(stepAvidaSlot()));
    file_menu->insertItem( "Update Avida", this, SLOT(updateAvidaSlot()));

    file_menu->insertSeparator();

    //tmp_menu_id = file_menu->insertItem( "Open Avida Session", this, SLOT(openAvidaSlot()), CTRL+Key_O);
    //file_menu->setItemEnabled(tmp_menu_id, false);

    //tmp_menu_id = file_menu->insertItem( "Close Avida Session", this, SLOT(closeAvidaSlot()));
    //file_menu->setItemEnabled(tmp_menu_id, false);

    //tmp_menu_id = file_menu->insertItem( "Reset Avida Session", this, SLOT(resetAvidaSlot()));
    //file_menu->setItemEnabled(tmp_menu_id, false);

    file_menu->insertItem( "Close Map Window", this, SLOT(closeSlot()), CTRL+Key_W);

    //tmp_menu_id = file_menu->insertItem( "Settings", &m_mission_control, SIGNAL(userSettingsSig()));
    //file_menu->setItemEnabled(tmp_menu_id, false);

    tmp_menu_id = file_menu->insertItem( "Quit", this, SIGNAL(quitSig()), CTRL+Key_Q);
  menuBar()->insertItem( "File", file_menu);

  //QPopupMenu *control_menu = new QPopupMenu( this, "<avd_MainWindow::avd_MainWindow(control_menu)>");
  //  control_menu->insertItem( "Start Avida", this, SLOT(startAvidaSlot()));
  //  control_menu->insertItem( "Pause Avida", this, SLOT(stopAvidaSlot()));
  //  control_menu->insertItem( "Step Avida", this, SLOT(stepAvidaSlot()));
  //  control_menu->insertItem( "Update Avida", this, SLOT(updateAvidaSlot()));
  //menuBar()->insertItem( "Control", control_menu);

  QPopupMenu *view_menu = new QPopupMenu( this, "<avd_MainWindow::avd_MainWindow(view_menu)>");
    view_menu->insertItem( "New Map Viewer", this, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", &m_mission_control, SIGNAL(newInstructionViewerSig()));
    view_menu->insertItem( "Event Editor", &m_mission_control, SIGNAL(newEventViewerSig()));
    //view_menu->insertItem( "Environment Control Box", &m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", &m_mission_control, SIGNAL(newPlotViewerSig()));
  menuBar()->insertItem( "Viewers", view_menu);

  QPopupMenu *help_menu = new QPopupMenu( this, "<avd_MainWindow::avd_MainWindow(help_menu)>");
    //help_menu->insertItem( "About Avida", this, SLOT(aboutAvidaHelpSlot()), CTRL+Key_H);
    //help_menu->insertItem( "Help Contents", this, SLOT(help1Slot()), CTRL+Key_H);
    //help_menu->insertItem( "Help2", this, SLOT(help2Slot()));

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
  add status bar
  */
  statusBar()->message("ready.");

  //QPopupMenu* file = new QPopupMenu( this );
  //file->insertItem("&Start", this, SLOT( startAvida() ) );
  //file->insertItem("Sto&p", this, SLOT( stopAvida() ) );
  ////file->insertItem("&Reset", this, SLOT( resetAvida() ) );
  //file->insertItem("E&xit", this, SLOT( exitProgram() ) );
  //menuBar()->insertItem( "&File", file );

  //QPopupMenu* view = new QPopupMenu( this );
  ////view->insertItem("&Creature View", this, SLOT( creatureView() ) );
  ///**
  // ** XXX:  test code by kaben to test cell viewer.
  // **/
  ////view->insertItem("Cell View", this, SLOT( cellView() ) );
  ///**/
  //view->insertItem("Instruction View", this, SLOT( N_Instruction_View() ) );
  //view->insertItem("Event View", this, SLOT( eventView() ) );
  ////view->insertItem("God Box", this, SLOT( godBox() ) );
  //view->insertItem("God Box 2", this, SLOT( godBox2() ) );
  ////view->insertItem("View &Genesis File", this, SLOT( genesisView() ) );
  ////view->insertItem("View &Event List", this, SLOT( eventListView() ) );
  ////view->insertItem("View E&nvironment", this, SLOT( environmentView() ) );
  ////view->insertItem("View &Instruction Set", this, SLOT( instSetView() ) );
  //menuBar()->insertItem( "&View", view );

  //QPopupMenu* plot = new QPopupMenu( this );
  //plot->insertItem("&Plot Graph", this, SLOT( createPlotView() ) );
  //menuBar()->insertItem( "&Plot", plot );

  GenDebug("done.");
}

void
avd_MainWindow::closeSlot(){
  GenDebug("entered.");

  emit closeEventSig(this);

  GenDebug("done.");
}


void
avd_MainWindow::aboutAvidaHelpSlot(){
  GenDebug("entered.");

  statusBar()->message("Opening help viewer...");
  m_mission_control.emitHelpURLSig("avida_home.html");
  statusBar()->message("Ready.");

  GenDebug("done.");
}

void
avd_MainWindow::help1Slot(){
  GenDebug("entered.");

  statusBar()->message("Opening help viewer...");
  m_mission_control.emitHelpURLSig("index.html");
  statusBar()->message("Ready.");

  GenDebug("done.");
}

void
avd_MainWindow::help2Slot(){
  GenDebug("entered.");

  statusBar()->message("Opening help viewer...");
  m_mission_control.emitHelpURLSig("help2.html");
  statusBar()->message("Ready.");

  GenDebug("done.");
}

void
avd_MainWindow::helpContents(){
  m_mission_control.emitHelpURLSig("index.html");
}
void
avd_MainWindow::helpGenesis(){
  m_mission_control.emitHelpURLSig("genesis.html");
}
void
avd_MainWindow::helpInstructionSet(){
  m_mission_control.emitHelpURLSig("inst_set.html");
}
void
avd_MainWindow::helpEvents(){
  m_mission_control.emitHelpURLSig("events.html");
}
void
avd_MainWindow::helpEnvironments(){
  m_mission_control.emitHelpURLSig("environment.html");
}
void
avd_MainWindow::helpAnalyzeMode(){
  m_mission_control.emitHelpURLSig("analyze_mode.html");
}


void
avd_MainWindow::openAvidaSlot(){
  GenDebug("entered.");


  statusBar()->message("Opening new Avida session...");
  m_mission_control.emitOpenAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::closeAvidaSlot(){
  GenDebug("entered.");


  statusBar()->message("Closing Avida session...");
  m_mission_control.emitCloseAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::resetAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Resetting Avida session...");
  m_mission_control.emitResetAvidaSig();

  GenDebug("done.");
}



void
avd_MainWindow::startAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Starting Avida...");
  m_mission_control.emitStartAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::updateAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Updating Avida...");
  m_mission_control.emitUpdateAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::stepAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Stepping Avida...");
  m_mission_control.emitStepAvidaSig(
    m_mission_control.getSelectedPopulationCellID()
  );

  GenDebug("done.");
}

void
avd_MainWindow::stopAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Pausing Avida...");
  m_mission_control.emitStopAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::exitAvidaSlot(){
  GenDebug("entered.");

  statusBar()->message("Exiting Avida...");
  m_mission_control.emitExitAvidaSig();

  GenDebug("done.");
}

void
avd_MainWindow::setSelectedPopulationCellSlot(int cell_id){
  GenDebug("entered.");

  m_mission_control.setSelectedPopulationCellID(cell_id);
  m_mission_control.emitPopulationCellSelectedSig(cell_id);
  statusBar()->message(QString("Population cell %1 selected.").arg(cell_id));

  GenDebug("done.");
}


void
avd_MainWindow::avidaOpenedSlot(void){
  GenDebug("entered.");
  // viewer to watch avida.
  setupViewer();
  statusBar()->message("New Avida session opened.");
  GenDebug("done.");
}
void
avd_MainWindow::avidaClosedSlot(void){
  GenDebug("entered.");
  cleanupViewer();
  statusBar()->message("Avida session closed.");
  GenDebug("done.");
}
void
avd_MainWindow::avidaResetSlot(void){
  GenDebug("entered.");
  cleanupViewer();
  setupViewer();
  statusBar()->message("Avida session reset.");
  GenDebug("done.");
}
void
avd_MainWindow::avidaUpdatedSlot(void){
  GenDebug("entered.");
  statusBar()->message("Avida is running...");
  GenDebug("done.");
}
void
avd_MainWindow::avidaSteppedSlot(int cell_id){
  GenDebug("entered.");
  statusBar()->message(QString("Stepping through organism %1's instructions.").arg(cell_id));
  GenDebug("done.");
}
void
avd_MainWindow::avidaBreakpointSlot(int cell_id){
  GenDebug("entered.");
  statusBar()->message(QString("Breakpoint encountered in organism %1.").arg(cell_id));
  GenDebug("done.");
}
void
avd_MainWindow::avidaStoppedSlot(void){
  GenDebug("entered.");
  statusBar()->message("Avida paused.");
  GenDebug("done.");
}




void
avd_MainWindow::closeEvent(QCloseEvent *e){
  GenDebug("entered.");

  emit closeEventSig(this);

  /* XXX:  can't reference anything after this point, because this widget may have been deleted.
   * I'm not sure, but I think that all that's left is the stack frame.
   */

  GenDebug("done.");
}


/* vim: set ts=2 ai et: */




// claus


//
//// porting to gcc 3.1 -- k
//
//#include "main_window.hh"
//
//#include "defs.hh"
//#include "config.hh"
//
//#include "init_file_view.hh"
//#include "map_view.hh"
//#include "creature_view.hh"
//#include "cell_view_widget.hh"
//#include "n_orig_instruction_viewer.hh"
//#include "event_view_widget.hh"
////#include "godbox_widget.hh"
//#include "godbox_widget2.hh"
//#include "avida_plot_widget.hh"
//#include "plot_select_dia_impl.hh"
//
//#include "messaging_population_wrapper.hh"
//
//#include <iostream>
//#include <stdlib.h>
//#include <qpopupmenu.h>
//#include <qmenubar.h>
//#include <qapplication.h>
//
//
//MainWindow::MainWindow(
//  MessagingPopulationWrapper *pop_wrap,
//  QWidget *parent,
//  int argc,
//  const char * const argv[]
//)
//  : QMainWindow( parent ),
//    m_argc( argc ), m_argv( argv ),
//    m_pop_wrap( pop_wrap )
//{
//  // connect to driver via m_pop_wrap
//  connect(
//    this, SIGNAL(startAvidaSig()),
//    m_pop_wrap, SLOT(startAvidaSlot())
//  );
//  connect(
//    this, SIGNAL(stopAvidaSig()),
//    m_pop_wrap, SLOT(stopAvidaSlot())
//  );
//  connect(
//    this, SIGNAL(resetAvidaSig()),
//    m_pop_wrap, SLOT(resetAvidaSlot())
//  );
//  connect(
//    this, SIGNAL(exitAvidaSig()),
//    m_pop_wrap, SLOT(exitAvidaSlot())
//  );
//
//  connect(
//    m_pop_wrap, SIGNAL(avidaUpdatedSig()),
//    this, SIGNAL(stateChanged())
//  );
//  connect(
//    m_pop_wrap, SIGNAL(avidaSteppedSig(int)),
//    this, SIGNAL(stateChanged())
//  );
//  
//  // and the viewer to watch it.
//  setupViewer();
//  // now create the menu bar.
//  setupMenuBar();
//  
//  // set some initial size
//  resize( 550, 400);
//
//}
//
//
//MainWindow::~MainWindow()
//{
//  cleanupViewer();
//  // m_timer is automatically destroyed.
//}
//
//
//void
//MainWindow::setupViewer()
//{
//  /**
//   ** XXX:  test code by kaben to test cell viewer.
//   **       this gives Qt a path to pixmaps used in some buttons.  this
//   **       kludge will change at some point.
//   **/
//  QMimeSourceFactory::defaultFactory()->addFilePath(".");
//  /**/
//
//  m_map_view = new MapView( m_pop_wrap, this );
//  setCentralWidget( m_map_view );
//
//  connect( this, SIGNAL( stateChanged() ),
//	   m_map_view, SLOT( repaintMap() ) );
//}
//
//void
//MainWindow::cleanupViewer()
//{
//  // delete all windows that have something to do with the Avida world.
//  // everything else remain open.
//  delete m_map_view;
//  delete m_creature_view;
//}
//
//void
//MainWindow::setupMenuBar()
//{
//  QPopupMenu* file = new QPopupMenu( this );
//  file->insertItem("&Start", this, SLOT( startAvida() ) );
//  file->insertItem("Sto&p", this, SLOT( stopAvida() ) );
//  //file->insertItem("&Reset", this, SLOT( resetAvida() ) );
//  file->insertItem("E&xit", this, SLOT( exitProgram() ) );
//  menuBar()->insertItem( "&File", file );
//
//  QPopupMenu* view = new QPopupMenu( this );
//  //view->insertItem("&Creature View", this, SLOT( creatureView() ) );
//  /**
//   ** XXX:  test code by kaben to test cell viewer.
//   **/
//  //view->insertItem("Cell View", this, SLOT( cellView() ) );
//  /**/
//  view->insertItem("Instruction View", this, SLOT( N_Instruction_View() ) );
//  view->insertItem("Event View", this, SLOT( eventView() ) );
//  //view->insertItem("God Box", this, SLOT( godBox() ) );
//  view->insertItem("God Box 2", this, SLOT( godBox2() ) );
//  //view->insertItem("View &Genesis File", this, SLOT( genesisView() ) );
//  //view->insertItem("View &Event List", this, SLOT( eventListView() ) );
//  //view->insertItem("View E&nvironment", this, SLOT( environmentView() ) );
//  //view->insertItem("View &Instruction Set", this, SLOT( instSetView() ) );
//  menuBar()->insertItem( "&View", view );
//
//  QPopupMenu* plot = new QPopupMenu( this );
//  plot->insertItem("&Plot Graph", this, SLOT( createPlotView() ) );
//  menuBar()->insertItem( "&Plot", plot );
//
//}
//
//
//void
//MainWindow::N_Instruction_View(void){
//  if ( !m_n_orig_instruction_viewer ){ // create the view if it doesn't exist
//    int cell = 0;
//    if ( m_map_view ) // do we have a map view already?
//      cell = m_map_view->GetSelectedCell();
//    m_n_orig_instruction_viewer = new N_Instruction_Viewer(
//      0,
//      0,
//      /*
//      XXX: destr-close of N_Instruction_Viewer fails without this.
//      I'm default-setting the flag in the constructor declaration, but
//      it doesn't seem to hold, don't know why...  -- K.
//      */
//      WDestructiveClose
//    );
//    m_n_orig_instruction_viewer->setPopulationWrapper(m_pop_wrap);
//    m_n_orig_instruction_viewer->setPopulationCell(cell);
//
//    connect(
//      m_map_view,
//      SIGNAL(cellSelected(int)),
//	    m_n_orig_instruction_viewer,
//      SLOT(setPopulationCell(int))
//    );
//  }
//  m_n_orig_instruction_viewer->show();
//  m_n_orig_instruction_viewer->raise();
//}
//
//
//void
//MainWindow::eventView(void){
//  if ( !m_event_view_widget ){ // create the view if it doesn't exist
//    int cell = 0;
//    if ( m_map_view ) // do we have a map view already?
//    m_event_view_widget = new EventViewWidget(
//      0,
//      0,
//      WDestructiveClose
//    );
//    m_event_view_widget->setPopulationWrapper(m_pop_wrap);
//    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
//  }
//  m_event_view_widget->show();
//  m_event_view_widget->raise();
//}
//
//
///*
//void
//MainWindow::godBox(void){
//  if ( !m_god_box_widget ){ // create the view if it doesn't exist
//    m_god_box_widget = new GodBoxWidget(
//      0,
//      0
//    );
//    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
//  }
//  m_god_box_widget->setPopulationWrapper(m_pop_wrap);
//  m_god_box_widget->setup();
//  m_god_box_widget->show();
//  m_god_box_widget->raise();
//}
//*/
//
//
//void
//MainWindow::godBox2(void){
//  if ( !m_god_box_widget2 ){ // create the view if it doesn't exist
//    m_god_box_widget2 = new GodBoxWidget2(
//      0,
//      0
//    );
//    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
//    m_god_box_widget2->setPopulationWrapper(m_pop_wrap);
//    m_god_box_widget2->setup();
//  }
//  m_god_box_widget2->show();
//  m_god_box_widget2->raise();
//}
//
//
////void
////MainWindow::creatureView()
////{
////  if ( !m_creature_view ){ // create the view if it doesn't exist
////    int cell = 0;
////    if ( m_map_view ) // do we have a map view already?
////      cell = m_map_view->GetSelectedCell();
////    m_creature_view = new CreatureView(
////      0, m_pop_wrap, cell );
////    connect( m_map_view, SIGNAL( cellSelected( int ) ),
////	     m_creature_view, SLOT( setCell( int ) ) );
////    connect( this, SIGNAL( stateChanged() ),
////	     m_creature_view, SLOT( drawView() ) );
////  }
////  m_creature_view->show();
////  m_creature_view->raise();
////}
//
//
//void
//MainWindow::cellView()
//{
//  if ( !m_cell_view_widget ){ // create the view if it doesn't exist
//    int cell = 0;
//    if ( m_map_view ) // do we have a map view already?
//      cell = m_map_view->GetSelectedCell();
//    m_cell_view_widget = new CellViewWidget();
//    m_cell_view_widget->setPopulationWrapper(m_pop_wrap);
//    m_cell_view_widget->setPopulationCell(cell);
//
//    connect(
//      m_map_view,
//      SIGNAL(cellSelected(int)),
//	    m_cell_view_widget,
//      SLOT(setPopulationCell(int))
//    );
//    connect(
//      this,
//      SIGNAL(stateChanged()),
//	    m_cell_view_widget,
//      SLOT(updateState())
//    );
//    connect(
//      m_pop_wrap, SIGNAL(avidaBreakpointSig(int)),
//      m_cell_view_widget, SLOT(setPopulationCell(int))
//    );
//  }
//  m_cell_view_widget->show();
//  m_cell_view_widget->raise();
//}
//
//void
//MainWindow::createPlotView()
//{
//  PlotSelectDiaImpl dia;
//
//  if ( dia.exec() ){
//    // The Avida Plot widgets are created with the flag Qt::WDestructiveClose,
//    // hence we don't have to worry about memory leaks here.
//    AvidaPlotWidget *w = new AvidaPlotWidget( m_pop_wrap, dia.result() );
//    connect( this, SIGNAL( stateChanged() ), w, SLOT( updateData() ) );
//    connect( this, SIGNAL( avidaDestroyed() ), w , SLOT( avidaDestroyed() ) );
//    w->show();
//  }
//}
//
//
//void
//MainWindow::genesisView()
//{
//  if ( !m_genesis_view ){ // create the view if it doesn't exist
//    m_genesis_view = new InitFileView(
//      QString( cConfig::GetGenesisFilename() () ), 0 );
//  }
//  else
//    m_genesis_view->setFile( ( cConfig::GetGenesisFilename() () ) );
//  m_genesis_view->show();
//  m_genesis_view->raise();
//}
//
//void
//MainWindow::eventListView()
//{
//  if ( !m_event_file_view ){ // create the view if it doesn't exist
//    m_event_file_view = new InitFileView(
//      QString( cConfig::GetEventFilename() () ), 0 );
//  }
//  else
//    m_event_file_view->setFile( ( cConfig::GetEventFilename() () ) );
//  m_event_file_view->show();
//  m_event_file_view->raise();
//}
//
//void
//MainWindow::environmentView()
//{
//  if ( !m_environment_view ){ // create the view if it doesn't exist
//    m_environment_view = new InitFileView(
//      QString( cConfig::GetEnvironmentFilename() () ), 0 );
//  }
//  else
//    m_environment_view->setFile( ( cConfig::GetEnvironmentFilename() () ) );
//  m_environment_view->show();
//  m_environment_view->raise();
//}
//
//
//void
//MainWindow::instSetView()
//{
//  if ( !m_inst_set_view ){ // create the view if it doesn't exist
//    m_inst_set_view = new InitFileView(
//      QString( cConfig::GetInstFilename() () ), 0 );
//  }
//  else
//    m_inst_set_view->setFile( ( cConfig::GetInstFilename() () ) );
//  m_inst_set_view->show();
//  m_inst_set_view->raise();
//}
//
//
//void
//MainWindow::startAvida()
//{
//  emit startAvidaSig();
//}
//
//void
//MainWindow::stopAvida()
//{
//  emit stopAvidaSig();
//}
//
//void
//MainWindow::resetAvida()
//{
//  // stop the current run
//  stopAvida();
//  // delete everything...
//  cleanupViewer();
//
//  // here, Avida should be reseted. That's not yet implemented...
//  emit resetAvidaSig();
//
//  // ... and set up anew
//  setupViewer();
//}
//
//void
//MainWindow::exitProgram()
//{
//  emit exitAvidaSig();
//}
//
////#include "main_window.moc"
//
//
