#ifndef AVD_AVIDA_DRIVER_HH
#define AVD_AVIDA_DRIVER_HH



#ifndef AVIDA_HH
#include "avida.hh"
#endif
#ifndef AVIDA_DRIVER_ANALYZE_HH
#include "avida_driver_analyze.hh"
#endif
#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif
#ifndef AVIDA_DRIVER_POPULATION_HH
#include "avida_driver_population.hh"
#endif
 
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif  
#ifndef QOBJECT_H
#include <qobject.h>
#endif  
#ifndef QTHREAD_H
#include <qthread.h>
#endif  
#ifndef QTIMER_H
#include <qtimer.h>
#endif  

#ifndef AVD_MESSAGE_QUEUE_HDRS_HH
#include "avd_message_queue_hdrs.hh"
#endif


class avd_MissionControl;
class MessagingPopulationWrapper;
class QCustomEvent;
class QWaitCondition;

class avd_AvidaDriver :
  public QObject,
  public cAvidaDriver_Population,
  public QThread
{
  Q_OBJECT
private:
  avd_MissionControl *m_mission_control;
  //MessagingPopulationWrapper *m_pop_wrap;

  // m_avida_thread_q and m_gui_thread_q should share m_mutex, but
  // should create their own wait conditions.
  avd_MessageQueue<QString> *m_avida_thread_q;
  avd_MessageQueue<QString> *m_gui_thread_q;


  static const QString
    update_avida_message,
    step_organism_message,
    //reset_avida_message,
    exit_avida_message,

    avida_updated_message,
    organism_stepped_message,
    avida_breakpoint_message,
    //avida_continuing_message,
    //avida_reset_message,
    avida_exited_message,

    hello_query_message,
    hello_response_message,
    
    malformed_message_to_avida,
    null_message,

    avida_threadstat_breakpoint,
    avida_threadstat_stepping,
    avida_threadstat_waiting;
  /*
  These should be set and read only by the avida thread.
  m_avida_thread_status should be one of
    avida_threadstat_breakpoint,
    avida_threadstat_stepping,
    avida_threadstat_waiting;
  */
  QString m_avida_thread_status;
  int m_avida_step_organism;
  int m_current_organism_id;

  /*
  These should be set and read only by the gui thread.
  */
  //bool m_should_avida_update;
  enum ExpectedAvidaState{
    STOPPED,
    STEPPING,
    RUNNING 
  } m_expected_avida_state;
  int m_gui_step_organism;
    
  //unsigned int
  //  m_messaging_timer_delay,
  //  m_min_update_interval,
  //  m_retry_timer_delay;

  /*
  following variables are used to maintain responsiveness of avida's
  processing thread.  goal is to process just enough organisms to
  consume m_desired_update_chunk_time_interval_in_milliseconds.

  the code lies in ProcessOrganisms, and maintains a running weighted
  average in m_required_chunk_size_in_organisms.  for each chunk of
  organisms processed, the time taken to process that chunk is
  determined, and used to revise the estimated size of the next chunk.
  */
  int m_desired_update_chunk_time_interval_in_milliseconds;
  // following variable is the elapsed time in processing last chunk of
  // organisms.  valid exactly when m_update_chunking_timer_isvalid is
  // true.
  int m_last_chunk_interval_in_milliseconds;
  // following variable is the running average mentioned above.
  int m_required_chunk_size_in_organisms;
  // following variable is used to measure time elapsed in processing
  // one chunk.
  QTime *m_update_chunking_timer;
  // if processing is interrupted, the elapsed time for the current
  // chunk is discarded, and the running average is NOT updated.
  // following variable is true unless processing was interrupted (by a
  // breakpoint, a stop-processing request in the middle of an update,
  // etc.).
  bool m_update_chunking_timer_isvalid;

  QTimer *m_update_delay_timer;

public:
  avd_AvidaDriver();
  avd_AvidaDriver(avd_MissionControl *mission_control, cEnvironment &environment);
  ~avd_AvidaDriver();
  
  virtual void Run();
  virtual void run();

  // Avida thread management
  virtual void ProcessOrganisms();

  //
  bool avidaIsRunning();
protected:
  void customEvent(QCustomEvent *custom_event);

protected slots:

  void isAvidaRunningSlot(void);

  void scheduleUpdateSlot(void);
  void updateAvidaSlot(void);
  void stepOrganismSlot(int cell_id);
  void startAvidaSlot(void);
  void stopAvidaSlot(void);
  //void resetAvidaSlot(void);
  void exitAvidaSlot(void);
  void helloAvidaSlot(void);
  void malformedMessageToAvidaSlot(void);

protected slots:

  // GUI thread management
  void guiMessageCatcher(void);

protected:

  // GUI thread management
  bool avidaUpdated(const QStringList &);
  bool organismStepped(const QStringList &);
  bool avidaBreakpoint(const QStringList &);
  //bool avidaContinuing(const QStringList &);
  //bool avidaReset(const QStringList &);
  bool avidaExited(const QStringList &);
  bool avidaSaysHi(const QStringList &);
  void avidaGotBunkMessage(const QStringList &);
  void avidaSentBunkMessage(const QStringList &);

  // Avida thread management
  void avidaMainMessageCatcher(void);
  void avidaSubupdateMessageCatcher(void);
  void avidaSteppingMessageCatcher(void);
  void avidaBreakpointMessageCatcher(void);

  bool parseUpdateAvidaArgs(const QStringList &);
  bool parseStepAvidaArgs(const QStringList &, int &cell_id_return);
  //bool parseContinueAvidaArgs(const QStringList &);
  //bool parseResetAvidaArgs(const QStringList &);
  bool parseExitAvidaArgs(const QStringList &);
  bool parseHelloAvidaArgs(const QStringList &);

  void malformedMessageToAvida(const QStringList &);

  virtual void NotifyUpdate(void);
  virtual void NotifyStep(int cell_id);
  virtual void SignalBreakpoint(void);
};  
    
  
#else
#warning multiple inclusion of avd_avida_driver.hh
#endif /* !AVD_AVIDA_DRIVER_HH */
    
/* vim: set ts=2 ai et: */

