//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_VIEW_WIDGET_HH
#define EVENT_VIEW_WIDGET_HH

#include "event_list_editor.h"

#include <qlistview.h>
#include <qdict.h>

class QCloseEvent;

class cString;
class avd_MissionControl;


class EventItem : public QListViewItem
{
  int m_ordering;
  QString m_trigg;
  QString m_start;
  QString m_step;
  QString m_stop;
  int m_event;
  QString m_param;

public:
  EventItem(QListView *parent);
  EventItem(QListView *parent, EventItem *after);
  EventItem(QListView *parent, int ordering);
  ~EventItem();

  const int getOrder(){
    return m_ordering;
  }
  void setOrder( const int &ordering ){
    m_ordering = ordering;
  }
  void setTrigger( const QString &trigg ){
    m_trigg = trigg;
  }
  void setStart( const QString & start ){
    m_start = start.stripWhiteSpace();
  }
  void setStep( const QString & step ){
    m_step = step.stripWhiteSpace();
  }
  void setStop( const QString & stop ){
    m_stop = stop.stripWhiteSpace();
  }
  void setEvent( int event ){
    m_event = event;
  }
  void setParameters( const QString & param ){
    m_param = param;
  }

  int getEvent() const{
    return m_event;
  }

  /**
   * Overloaded from QListViewItem.
   **/
  QString text( int column ) const;

  /**
   * Returns an entry formated for the event list file.
   **/
  QString fileFormat() const;
};


class EventViewWidget : public EventListEditor
{
  Q_OBJECT
private:
  QString m_file_name;
  QDict<int> *m_event_name_lookup;

  bool m_document_changed;

  avd_MissionControl *m_mission_control;
public:
  EventViewWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~EventViewWidget();
  void setMissionControl(avd_MissionControl *mission_control);
protected slots:
  void updateEventItem(EventItem *item);
  void updateEventLV();
  void updateEventInp();
  void updateEventDescr( int item );
  void deleteEvent();
  void openClicked();
  void loadFromMemoryClicked();
  void lowerEvent();
  void newEvent();
  void raiseEvent();
  void saveClicked();
  void saveAsClicked();
  void commitToMemoryClicked();
  void clearClicked();
  /**
   * Overloaded from QWidget.
   **/
 void closeEvent( QCloseEvent *e );

  //void avidaStatusSlot(bool is_running);

  void helpContents();
  void helpGenesis();
  void helpInstructionSet();
  void helpEvents();
  void helpEnvironments();
  void helpAnalyzeMode();


  void helpButtonPushed();


protected:
  bool discardSafely();
  void saveToFile( const QString &file_name );
  void commitToMemory();
  void loadFromFile( const QString &file_name );
  void loadFromMemory();

  /**
   * This function is almost a verbatim copy from @ref cEventList.
   **/
  EventItem *addEventFileFormat(
    const cString& in_line,
    EventItem *last_item = 0
  );
  EventItem *addEventFileFormat(
    const QString& in_line,
    EventItem *last_item = 0
  );
};

#endif /* !EVENT_VIEW_WIDGET_HH */
