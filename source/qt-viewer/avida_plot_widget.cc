
#include "avida_plot_widget.hh"

#include "file.hh"
#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif
#include "population.hh"
#include "stats.hh"

#include <qmenubar.h>
#include <qpopupmenu.h>

//#include "messaging_population_wrapper.hh"
#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


AvidaPlotWidget::AvidaPlotWidget(
  avd_MissionControl *mission_control,
  AvidaData::eTypes type,
  QWidget *parent,
  const char * name, WFlags f
)
: PlotWidget( parent, name, f ),
  m_mission_control( mission_control ),
  m_type( type ),
  m_entry( AvidaData::findEntry( type ) )
{

  QPopupMenu *file_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(file_menu)>");
    //file_menu->insertItem( "Open Avida Session", this, SLOT(openAvidaSlot()), CTRL+Key_O);
    //file_menu->insertItem( "Close Avida Session", this, SLOT(closeAvidaSlot()));
    //file_menu->insertItem( "Reset Avida Session", this, SLOT(resetAvidaSlot()));
    //file_menu->insertItem( "Settings", m_mission_control, SIGNAL(userSettingsSig()));
    //file_menu->insertItem( "Quit", this, SIGNAL(quitSig()), CTRL+Key_Q);

    file_menu->insertItem( "Start Avida", m_mission_control, SIGNAL(startAvidaSig()));
    file_menu->insertItem( "Pause Avida", m_mission_control, SIGNAL(stopAvidaSig()));
    file_menu->insertItem( "Step Avida", m_mission_control, SIGNAL(stepAvidaSig()));
    file_menu->insertItem( "Update Avida", m_mission_control, SIGNAL(updateAvidaSig()));

    file_menu->insertSeparator();

    file_menu->insertItem( "Close Plot Viewer", this, SLOT(close()), CTRL+Key_W);
    file_menu->insertItem( "Quit", m_mission_control, SIGNAL(quitSig()), CTRL+Key_Q);
    
  menuBar()->insertItem( "File", file_menu);
    
  //QPopupMenu *control_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(control_menu)>");
  //  control_menu->insertItem( "Start Avida", this, SLOT(startAvidaSlot()));
  //  control_menu->insertItem( "Pause Avida", this, SLOT(stopAvidaSlot()));
  //  control_menu->insertItem( "Step Avida", this, SLOT(stepAvidaSlot()));
  //  control_menu->insertItem( "Update Avida", this, SLOT(updateAvidaSlot()));
  //menuBar()->insertItem( "Control", control_menu);
  
  QPopupMenu *view_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(view_menu)>");
    view_menu->insertItem( "New Map Viewer", m_mission_control, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", m_mission_control, SIGNAL(newInstructionViewerSig())); 
    view_menu->insertItem( "Event Editor", m_mission_control, SIGNAL(newEventViewerSig()));
    view_menu->insertItem( "Control Box", m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", m_mission_control, SIGNAL(newPlotViewerSig()));
  menuBar()->insertItem( "Viewers", view_menu);
  
  QPopupMenu *help_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(help_menu)>");
    help_menu->insertItem("Documentation Contents", this, SLOT(helpContents())); help_menu->insertSeparator(); 
    help_menu->insertItem("Setting up the Genesis File", this, SLOT(helpGenesis()));
    help_menu->insertItem("Setting up the Instruction Set", this, SLOT(helpInstructionSet()));
    help_menu->insertItem("Guidelines to setting Events", this, SLOT(helpEvents()));
    help_menu->insertItem("Guidelines to setting up Environments", this, SLOT(helpEnvironments()));
    help_menu->insertItem("Guidelines to Analyze Mode", this, SLOT(helpAnalyzeMode()));
  menuBar()->insertItem( "Help", help_menu);
  


  setCaption( m_entry.getDescription() );
  
  setYLabel( m_entry.getDescription() );
  setXLabel( "Time [updates]" );
  
  // load in those data points that have already been created while
  // avida was running...
  load( m_entry.getDataFilename(), 1, m_entry.getColumn() );
}


AvidaPlotWidget::~AvidaPlotWidget()
{
}


void
AvidaPlotWidget::load( const char* filename, int colx, int coly )
{
  assert( colx > 0 );
  assert( coly > 0 );

  clear( false );

  cInitFile file( filename );
  file.Load();
  file.Compress();
  cString line;
  cString valx;
  cString valy;

  if (file.GetNumLines() > 0) {
    line = file.GetLine();
    valx = line.GetWord( colx - 1 );
    valy = line.GetWord( coly - 1 );
    addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );
  }

  for (int line_id = 1; line_id < file.GetNumLines(); line_id++) {
    line = file.GetLine(line_id);
    valx = line.GetWord( colx - 1 );
    valy = line.GetWord( coly - 1 );
    addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );
  }
  addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );

  repaint( false );
}


void
AvidaPlotWidget::updateData()
{
  GenDebug("entered.");

  // if we don't have an active Avida object, do nothing.
  if ( m_mission_control == 0 || m_mission_control->getPopulation() == 0)
    return;


  //m_mission_control->lock();
  // the x value is the time;
  double x = m_mission_control->getPopulation()->GetStats().GetUpdate();
  // the y value depends on the data type chosen
  double y = AvidaData::getValue( m_type, m_mission_control->getPopulation() );
  //m_mission_control->unlock();

  addPoint( DataPoint( x, y ) );

  GenDebug("done.");
}


void
AvidaPlotWidget::avidaDestroyed()
{
  // we set the avida object to zero. The plot widget is essentially
  // frozen now.
  m_mission_control = 0;

}

void
AvidaPlotWidget::helpContents(){
  m_mission_control->emitHelpURLSig("index.html");
} 
void
AvidaPlotWidget::helpGenesis(){
  m_mission_control->emitHelpURLSig("genesis.html");
} 
void    
AvidaPlotWidget::helpInstructionSet(){
  m_mission_control->emitHelpURLSig("inst_set.html");
}   
void  
AvidaPlotWidget::helpEvents(){
  m_mission_control->emitHelpURLSig("events.html");
}   
void
AvidaPlotWidget::helpEnvironments(){
  m_mission_control->emitHelpURLSig("environment.html");
} 
void
AvidaPlotWidget::helpAnalyzeMode(){
  m_mission_control->emitHelpURLSig("analyze_mode.html"); 
}   


//#include "avida_plot_widget.moc"
