//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////



#ifndef EVENT_VIEW_WIDGET_HH
#include "event_view_widget.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif


#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef INIT_FILE_HH
#include "file.hh"
#endif

#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif

#include "event_list.hh"
//#include <sstream>
#include <strstream>
#include <qstringlist.h>
#include <qregexp.h>

#include <qlineedit.h>
#include <qlabel.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qtextview.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qevent.h>

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>

//event descriptions
#include "cPopulation_descr.hi"

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


EventItem::EventItem( QListView * parent )
  : QListViewItem( parent )
{
  m_ordering = listView()->childCount();
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::EventItem( QListView * parent, EventItem *after )
  : QListViewItem( parent, after )
{
  if ( after )
    m_ordering = after->getOrder() + 1;
  else m_ordering = listView()->childCount();
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::EventItem( QListView * parent, int ordering )
  : m_ordering(ordering), QListViewItem( parent )
{
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::~EventItem()
{
  GenDebug << "closing.";
}


QString
EventItem::text( int column ) const
{
  switch ( column ){
  case 0:
    return m_trigg;
  case 1:
    return m_start;
  case 2:
    return m_step;
  case 3:
    return m_stop;
  case 4:
    return cEventDescrs::entries[m_event].GetName();
  case 5:
    return m_param;
  default:
    return QString::null;
  }
}


QString
EventItem::fileFormat() const
{
  QString result = m_trigg;

  if ( !m_start.isEmpty() )
    result += " " + m_start;
  if ( !m_step.isEmpty() )
    result += ":" + m_step;
  if ( !m_stop.isEmpty() )
    result += ":" + m_stop;

  result += " " + QString( cEventDescrs::entries[m_event].GetName() ) + " " + m_param + "\n";
  return result;
}


/*
 *  Constructs a EventViewWidget which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
EventViewWidget::EventViewWidget( QWidget* parent,  const char* name, WFlags fl )
  : EventListEditor( parent, name, fl )
{
  m_events_lv->setAllColumnsShowFocus( true );
  m_events_lv->setSorting(-1);
  m_document_changed = false;

  m_event_name_lookup = new QDict<int>( 107 );
  m_event_name_lookup->setAutoDelete( true );

  QString event;
  for ( int i=0; i<cEventDescrs::num_of_events; i++ ){
    event = cEventDescrs::entries[i].GetName();
    m_event_cb->insertItem( event );
    // no memory leak here, because we have AutoDelete...
    m_event_name_lookup->insert( event, new int(i) );
  }
  updateEventDescr( 0 );

  connect(
    m_help_b, SIGNAL(clicked()),
    this, SLOT(helpButtonPushed())
  );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EventViewWidget::~EventViewWidget()
{
  // no need to delete child widgets, Qt does it all for us
  GenDebug << "closing.";
}

//void
//EventViewWidget::avidaStatusSlot(bool is_running){
//  m_mission_control->emitStopAvidaSig(); 
//}

void
EventViewWidget::setMissionControl(
  avd_MissionControl *mission_control
){
  m_mission_control = mission_control;
  GenDebug << "m_mission_control set.";

  /*
  pause avida.
  */
  //connect(
  //  mission_control, SIGNAL(avidaStatusSig(bool)),
  //  this, SLOT(avidaStatusSlot(bool))
  //);
  //mission_control->emitIsAvidaRunningSig();

  /*
  add menu items
  */
  QPopupMenu *file_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(file_menu)>");

    file_menu->insertItem( "Close Event Editor", this, SLOT(close()), CTRL+Key_W);
    file_menu->insertItem( "Quit", m_mission_control, SIGNAL(quitSig()), CTRL+Key_Q);

  menuBar()->insertItem( "File", file_menu);

  QPopupMenu *control_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(control_menu)>");
    control_menu->insertItem( "Start Avida", m_mission_control, SIGNAL(startAvidaSig()));
    control_menu->insertItem( "Pause Avida", m_mission_control, SIGNAL(stopAvidaSig()));
    control_menu->insertItem( "Step Avida", m_mission_control, SIGNAL(stepAvidaSig()));
    control_menu->insertItem( "Update Avida", m_mission_control, SIGNAL(updateAvidaSig()));
  menuBar()->insertItem( "Control", control_menu);

  QPopupMenu *view_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(view_menu)>");
    view_menu->insertItem( "New Map Viewer", m_mission_control, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", m_mission_control, SIGNAL(newInstructionViewerSig()));
    view_menu->insertItem( "Event Viewer", m_mission_control, SIGNAL(newEventViewerSig()));
    view_menu->insertItem( "Control Box", m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", m_mission_control, SIGNAL(newPlotViewerSig()));
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

  loadFromMemoryClicked();
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventItem(EventItem *item)
{
  if ( !item )
    return;

  //m_start_cb->blockSignals( true );
  //m_step_cb->blockSignals( true );
  //m_stop_cb->blockSignals( true );
  switch( m_trigger_group->id( m_trigger_group->selected() ) ){
  case 0:
    item->setTrigger( "immediate" );
    m_start_cb->lineEdit()->setText( "" );
    m_step_cb->lineEdit()->setText( "" );
    m_stop_cb->lineEdit()->setText( "" );
    m_interval_group->setEnabled(false);
    break;
  case 1:
    item->setTrigger( "update" );
    m_interval_group->setEnabled(true);
    break;
  case 2:
    item->setTrigger( "generation" );
    m_interval_group->setEnabled(true);
    break;
  //case 3:
  //  item->setTrigger( "r" );
  //  break;
  //case 4:
  //  item->setTrigger( "e" );
  //  break;
  }

  item->setStart( m_start_cb->currentText() );
  item->setStep( m_step_cb->currentText() );
  item->setStop( m_stop_cb->currentText() );
  item->setParameters( m_parameters_inp->text() );
  item->setEvent( m_event_cb->currentItem() );

  item->repaint();

  // the document has changed now.
  m_document_changed = true;
  m_file_name = QString::null;
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventLV()
{
  EventItem *item = static_cast<EventItem *>( m_events_lv->currentItem() );
  GenDebug << "item " << item << '.';
  updateEventItem(item);
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventDescr( int item )
{
  GenDebug << "item " << item << '.';
  if(m_events_lv->currentItem() == 0) return;
  m_event_descr_tv->setText( cEventDescrs::entries[item].GetDescription() );
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventInp()
{
  
  
  QListViewItem *item = m_events_lv->currentItem();
  if ( !item )
    return;

  m_trigger_group->blockSignals( true );
  QString trig = item->text( 0 );
  if ( trig == "i" || trig == "immediate" )
    m_imm_rb->setChecked( true );
  else if ( trig == "u" || trig == "update" )
    m_upd_rb->setChecked( true );
  else if ( trig == "g" || trig == "generation" )
    m_gen_rb->setChecked( true );
  //else if ( trig == "r" )
  //  m_realt_rb->setChecked( true );
  //else if ( trig == "e" )
  //  m_exec_rb->setChecked( true );
  m_trigger_group->blockSignals( false );

  //m_start_cb->blockSignals( true );
  //m_step_cb->blockSignals( true );
  //m_stop_cb->blockSignals( true );
  if(m_imm_rb->isOn()){
    m_start_cb->lineEdit()->setText( "" );
    m_step_cb->lineEdit()->setText( "" );
    m_stop_cb->lineEdit()->setText( "" );
    m_interval_group->setEnabled(false);
  } else {
    m_interval_group->setEnabled(true);
    m_start_cb->lineEdit()->setText( item->text( 1 ) );
    m_step_cb->lineEdit()->setText( item->text( 2 ) );
    m_stop_cb->lineEdit()->setText( item->text( 3 ) );
  }
  //m_start_cb->blockSignals( false );
  //m_step_cb->blockSignals( false );
  //m_stop_cb->blockSignals( false );

  m_start_cb->lineEdit()->setText( item->text( 1 ) );
  m_step_cb->lineEdit()->setText( item->text( 2 ) );
  m_stop_cb->lineEdit()->setText( item->text( 3 ) );

  m_parameters_inp->blockSignals( true );
  m_parameters_inp->setText( item->text( 5 ) );
  m_parameters_inp->blockSignals( false );

  m_event_cb->blockSignals( true );
  int i = static_cast<EventItem *>( item )->getEvent();
  m_event_cb->setCurrentItem( i );
  m_event_cb->blockSignals( false );
  updateEventDescr( i );
}

/*
 * protected slot
 */
void
EventViewWidget::deleteEvent()
{
  QListViewItem *item = m_events_lv->selectedItem();
  if ( !item )
    return;

  QListViewItem *prev_item = item->itemAbove();
  QListViewItem *next_item = item->itemBelow();

  /*
  reorder (by decreasing its ordering by one) each item below the one
  we're about to delete.
  */
  QListViewItemIterator it( item );
  it++;
  while ( it.current() ){
    EventItem *event_item = (EventItem *)(it.current());
    event_item->setOrder(
      event_item->getOrder() - 1
    );
    it++;
  }
  
  delete item;

  /*
  if there was an item below the one we just deleted, select it;
  otherwise, if there was an item above the one we deleted, select it;
  otherwise do nothing.
  */
  if( next_item ){
    m_events_lv->setCurrentItem( next_item );
    m_events_lv->setSelected( next_item, true );
  } else if( prev_item ){
    m_events_lv->setCurrentItem( prev_item );
    m_events_lv->setSelected( prev_item, true );
  }

  // the document has changed now.
  m_document_changed = true;
}

/*
 * protected slot
 */
void
EventViewWidget::lowerEvent()
{
  EventItem *item = (EventItem *)m_events_lv->currentItem();
  if ( !item )
    return;

  QListViewItemIterator it( item );
  QListViewItem *parent = item->parent();
  it++;
  while ( it.current() ) {
    if ( it.current()->parent() == parent )
      break;
    it++;
  }

  if ( !it.current() )
    return;
  EventItem *other = (EventItem *)it.current();

  item->moveItem( other );

  int orderswap = item->getOrder();
  item->setOrder(other->getOrder());
  other->setOrder(orderswap);

  // the document has changed now.
  m_document_changed = true;

}
/*
 * protected slot
 */
void
EventViewWidget::newEvent()
{
  EventItem *current_item = (EventItem *)m_events_lv->selectedItem();
  EventItem *prev_item = 0;
  if (current_item) {
    prev_item = (EventItem *)current_item->itemAbove();
  }

  //QListViewItemIterator it( item );

  /*
  if an item was already selected, try to place the new item immediately
  before it; otherwise place the new item at the bottom.
  */
  GenDebug << "inserting...";
  EventItem *item;
  if( prev_item ){
    item = new EventItem( m_events_lv, prev_item );
  } else if (current_item) {
    item = new EventItem( m_events_lv, current_item->getOrder() );
  } else {
    item = new EventItem( m_events_lv );
  }
  GenDebug << "done inserting...";

  /*
  unless the new item was placed at the bottom of the list, there are
  items below it whose orderings need to be incremented by one.
  */
  GenDebug << "reordering...";
  if (current_item){
    QListViewItemIterator it( item );
    it++;
    while ( it.current() ){
      EventItem *event_item = (EventItem *)(it.current());
      event_item->setOrder( event_item->getOrder() + 1);
      it++;
    }
  }
  GenDebug << "done reordering...";

  /*
  select the new item.
  */
  GenDebug << "selecting...";
  item->setText( 0, "Item" );
  m_events_lv->setCurrentItem( item );
  m_events_lv->setSelected( item, TRUE );
  updateEventItem(item);
  GenDebug << "done selecting...";

  // the document has changed now.
  m_document_changed = true;

  GenDebug << "done.";
}

/*
 * protected slot
 */
void
EventViewWidget::raiseEvent()
{
  EventItem *item = (EventItem *)m_events_lv->currentItem();
  if ( !item )
    return;

  QListViewItemIterator it( item );
  QListViewItem *parent = item->parent();
  --it;
  while ( it.current() ) {
    if ( it.current()->parent() == parent )
      break;
    --it;
  }

  if ( !it.current() )
    return;
  EventItem *other = (EventItem *)it.current();

  other->moveItem( item );

  int orderswap = item->getOrder();
  item->setOrder(other->getOrder());
  other->setOrder(orderswap);

  // the document has changed now.
  m_document_changed = true;

}

/*
 * protected slot
 */
void
EventViewWidget::clearClicked()
{
  if ( !discardSafely() ) // we don't clear if save was aborted.
    return;

  m_events_lv->clear();
  m_document_changed = false;
}


/*
 * protected slot
 */
void
EventViewWidget::openClicked()
{
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;

  QString file_name = QFileDialog::getOpenFileName( QString::null,
  "Event List (*.el *.cfg)", this );

  if ( !file_name.isNull() )                  // got a file name
    loadFromFile( file_name );  // loadFromFile updates m_file_name
  updateEventInp();
}


void
EventViewWidget::loadFromFile( const QString &file_name )
{
  cInitFile event_file( file_name.latin1() );

  // Load in the proper event list and set it up.
  event_file.Load();
  event_file.Compress();

  m_events_lv->clear();

  EventItem *last_item = 0;

  GenDebug << "inserting events...";
  for (int i = 0; i < event_file.GetNumLines(); i++) {
    last_item = addEventFileFormat(event_file.GetLine(i), last_item);
  }
  GenDebug << "done inserting events...";
  GenDebug << "sorting...";
  m_events_lv->sort();
  GenDebug << "done sorting...";

  m_document_changed = false;
  m_file_name = file_name;
  m_file_name_l->setText( m_file_name );
  GenDebug << "done.";
}


/*
 * protected slot
 */
void
EventViewWidget::loadFromMemoryClicked()
{
  GenDebug << "called.";
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;

  loadFromMemory();
  updateEventInp();
}


void
EventViewWidget::loadFromMemory()
{
  if(m_mission_control == 0)
    return;

  m_mission_control->lock();
  if(m_mission_control->getEventList() == 0){
    m_mission_control->unlock();
    return;
  }

  m_events_lv->clear();

  GenDebug << "starting iterator test.";

  EventItem *last_item = 0;

  for(
    cEventListIterator it = m_mission_control->getEventList()->begin();
    it != m_mission_control->getEventList()->end();
    ++it
  ){

    #if LOCAL_DEBUG
    it.PrintEvent();
    #endif

    /*
    ugly but functional conversion from stringstream to QString to,
    finally, EventItem via addEventFileFormat().  -- K.
    */
    strstream tmp_sstream;
    it.PrintEvent(tmp_sstream);
    QString event_string(&tmp_sstream.str()[0]);
    event_string.truncate(tmp_sstream.pcount());
    last_item = addEventFileFormat(event_string, last_item);
  }

  GenDebug << "iterator test done.";

  m_mission_control->unlock();

  //m_events_lv->sort();
  m_document_changed = false;
}


EventItem *
EventViewWidget::addEventFileFormat(
  const cString& in_line,
  EventItem *last_item
){
  return addEventFileFormat(QString(in_line), last_item);
}


EventItem *
EventViewWidget::addEventFileFormat(
  const QString& in_line,
  EventItem *last_item
){
  GenDebug << "active:  " << "06:18:46" << '.';
  GenDebug
  << " --- in_line:  \"" << in_line << "\"\n"
  << " --- *last_item:  \"" << last_item << "\".";

  // Timing
  QString trigger = "immediate";
  QString start = "begin";
  QString interval = "once";
  QString stop = "end";
  QString event = "";
  QString params = "";

  bool is_immediate = true;

  QStringList word_list(
    QStringList::split(
      QRegExp("\\s"),
      QString(in_line)
    )
  ); 

  QStringList::Iterator cur_word = word_list.begin();

  if(cur_word == word_list.end()) return 0;
  //if( *cur_word == "i"
  //    ||
  //    *cur_word == "immediate" 
  //    ||
  if( *cur_word == "u"
      ||
      *cur_word == "update"
      ||
      *cur_word == "g"
      ||
      *cur_word == "generation"
  ){
    trigger = *cur_word;
    ++cur_word;
    is_immediate = false;

    GenDebug << " --- trigger:  \"" << trigger << "\".";
  }else if( *cur_word == "i"
      ||
      *cur_word == "immediate" 
  ){
    trigger = *cur_word;
    ++cur_word;
    is_immediate = true;

    GenDebug << " --- trigger:  \"" << trigger << "\".";
  }else{
    // If Trigger is skipped then assume IMMEDIATE
    trigger = "immediate";
    is_immediate = true;
    GenDebug << " --- immediate trigger assumed.";
  }
  
  /*
  FIXME:  handle error condition below.
  */
  if(cur_word != word_list.end()){
    // Do we now have timing specified?
    // Parse the Timing; get the start:interval:stop
    QStringList timing_list(
      QStringList::split(
        ":",
        *cur_word   
      )
    ); 
    QStringList::Iterator timing_str = timing_list.begin();

    // If first value is valid, we are getting a timing.
    if(timing_str == timing_list.end()){
      // We don't have timing, so assume IMMEDIATE
      trigger = "immediate";
      is_immediate = true;
      GenDebug << " --- timing_str is null; assuming immediate.";
    } else {
      bool ok;
      int number;
      number = (*timing_str).toInt(&ok);
      if(ok || *timing_str == "begin"){
        // First number is start.
        start = *timing_str;

        GenDebug << " --- start:  " << start << '.';

        if(++timing_str != timing_list.end()){
          interval = *timing_str;

          GenDebug << " --- interval:  " << interval << '.';

          if(++timing_str != timing_list.end()){
            stop = *timing_str;

            GenDebug << " --- stop:  " << stop << '.';
          } else {
            // If no other words... is "start:interval" syntax
            GenDebug << " --- assuming \"start:interval\" syntax.";
          }
        } else {
          // If no other words... is "start" syntax
          GenDebug << " --- assuming \"start\" syntax.";
        }
        // timing provided, so get next word
        ++cur_word;
      } else {
        // We don't have timing, so assume IMMEDIATE
        trigger = "immediate";
        is_immediate = true;
        GenDebug << " --- no timing; assuming immediate.";
      }
    }
  }

  if(cur_word != word_list.end()){
    event = *cur_word;
    ++cur_word;
  }
  while(cur_word != word_list.end()){
    params += *cur_word + " ";
    ++cur_word;
  }

  GenDebug << " --- event:  " << event << '.';
  GenDebug << " --- params:  " << params << '.';

  int* event_ind = m_event_name_lookup->find( event );
  EventItem *item = 0;
  if ( event_ind ){
    item = new EventItem(m_events_lv, last_item);
    item->setTrigger( trigger );
    if(is_immediate){
      item->setStart( "" );
      item->setStep( "" );
      item->setStop( "" );
    } else {
      item->setStart( start );
      item->setStep( interval );
      item->setStop( stop );
    }
    item->setEvent( *event_ind );
    item->setParameters( params );
  }
  return item;
}


/*
 * protected slot
 */
void
EventViewWidget::saveClicked()
{
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;

  if ( m_file_name.isEmpty() )
    saveAsClicked(); // we really need 'Save as' now
  else
    saveToFile( m_file_name );
}

/*
 * protected slot
 */
void
EventViewWidget::saveAsClicked()
{
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;


  // get a file name
  QString file_name = m_file_name;

  if ( file_name.isEmpty() )
    file_name = "event_list.cfg";

  file_name = QFileDialog::getSaveFileName( file_name, "Event List (*.el *.cfg)", this );

  if ( !file_name.isNull() )                  // got a file name
    saveToFile( file_name );  // saveToFile updates m_file_name
}

void
EventViewWidget::saveToFile( const QString &file_name )
{
  // first we check whether we have something to save...
  EventItem *item = static_cast<EventItem *>( m_events_lv->firstChild() );
  if ( !item )
    return;

  if ( !file_name.isNull() ) {                 // got a file name
    QFile f( file_name );

    if ( f.open(IO_WriteOnly) ) {    // file opened successfully
      // update the file name
      m_file_name = file_name;
      m_file_name_l->setText( m_file_name );

      // now write the file
      QTextStream t( &f );        // use a text stream
      t.setEncoding( QTextStream::Latin1 );

      do {
	      t << item->fileFormat();
	      item = static_cast<EventItem *>( item->nextSibling() );
      }
      while ( item );       // until end of the list
      f.close();

      // the document is saved now.
      m_document_changed = false;
    }
  }
}


/*
 * protected slot
 */
void
EventViewWidget::commitToMemoryClicked()
{
  GenDebug << "called.";
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;

  commitToMemory();
}


void
EventViewWidget::commitToMemory()
{
  GenDebug << "called.";

  // first we check whether we have something to save...
  EventItem *item = static_cast<EventItem *>( m_events_lv->firstChild() );
  if ( !item )
    return;

  if(m_mission_control == 0)
    return;
  m_mission_control->lock();
  if(m_mission_control->getEventList() == 0){
    m_mission_control->unlock();
    return;
  }

  m_mission_control->getEventList()->DeleteAll();
  do {
    if(!item->fileFormat()){
      ; // do nothing
    } else {
      m_mission_control->getEventList()->AddEventFileFormat(
        cString(item->fileFormat().latin1())
      );
    }

    GenDebug << " --- " << item->fileFormat() << '.';

	  item = static_cast<EventItem *>( item->nextSibling() );
  } while(item);

  m_mission_control->unlock();
  m_document_changed = false;
}


bool
EventViewWidget::discardSafely()
{
  GenDebug << "called.";
  if ( m_document_changed ){
    switch( QMessageBox::warning( this, "Message",
				  "This Event List has been modified.\nWould you like to save it?\n",
				  "&Yes", "&No", "&Cancel",
				  0, 2 ) ){
    case 0: // save before discarding
      saveClicked();
      return !m_document_changed; // in case save didn't succeed.
    case 1: // don't save
      return true;
    case 2: // cancel
      return false;
    default:
      /*
      if we get here then something strange has happened.
      */
      return false;
      break; // not reached.
    }
  }
  return true;
}


void
EventViewWidget::closeEvent( QCloseEvent *e )
{
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;


  EventListEditor::closeEvent( e );
}




void
EventViewWidget::helpContents(){
  m_mission_control->emitHelpURLSig("index.html");
}
void
EventViewWidget::helpGenesis(){
  m_mission_control->emitHelpURLSig("genesis.html");
}
void
EventViewWidget::helpInstructionSet(){
  m_mission_control->emitHelpURLSig("inst_set.html");
}
void
EventViewWidget::helpEvents(){
  m_mission_control->emitHelpURLSig("events.html");
}
void
EventViewWidget::helpEnvironments(){
  m_mission_control->emitHelpURLSig("environment.html");
}
void
EventViewWidget::helpAnalyzeMode(){
  m_mission_control->emitHelpURLSig("analyze_mode.html");
}

void
EventViewWidget::helpButtonPushed(void)
{
  GenDebug << "entered."; 

  /*
  FIXME:  replace with real help documentation.  -- kgn
  */
  QMessageBox::information(
    this,
    "Events-Editing Help",
    "The beta version of Avida "
    "lacks documentation for this "
    "part of the user interface. "
    "We're working on it."
  );

  GenDebug("done.");
}



