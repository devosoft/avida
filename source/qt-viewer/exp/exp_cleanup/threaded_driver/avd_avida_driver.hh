#ifndef AVD_AVIDA_DRIVER_HH
#define AVD_AVIDA_DRIVER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif  
#ifndef QOBJECT_H
#include <qobject.h>
#endif  
#ifndef QTHREAD_H
#include <qthread.h>
#endif  

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

#ifndef AVIDA_HH
#include "main/avida.hh"
#endif


class QCustomEvent;
class QWaitCondition;
class avdAvidaThreadDrvr :
  public QObject
, public QThread
, public cAvidaDriver_Population
, public cRetainable
{ Q_OBJECT
protected:
  QMutex *m_mutex;
  QWaitCondition m_wait_condition;
  bool (avdAvidaThreadDrvr::*m_mode_function)();
  bool (avdAvidaThreadDrvr::*m_fast_stage_function)();
  bool (avdAvidaThreadDrvr::*m_step_stage_function)();
  void (avdAvidaThreadDrvr::*m_emit_function)();
  bool m_run_continuously;
  bool m_step_mode;
  int m_step_cell_id;
  int m_breakpoint_cell_id;
  int m_current_cell_id;
  int m_UD_size;
  double m_step_size;
  int m_UD_progress;
  int m_mutations_progress;
public:
  avdAvidaThreadDrvr();
  avdAvidaThreadDrvr(cEnvironment &environment);
  ~avdAvidaThreadDrvr();
public:
  /*
  Entry point into the driver by the gui thread; all it really does is
  start the processing thread, and then return.
  */
  virtual void Run();
public slots:
  /*
  The following six slots are called by the gui, and control the
  processing thread.
  */
  void doPauseAvidaSlot();
  void doStartAvidaSlot();
  void doUpdateAvidaSlot();
  void doStepAvidaSlot(int cell_id);
  void doCloseAvidaSlot();
protected:
  void wakeProcessingThread(bool continuous, bool step);
signals:
  void avidaUpdatedSig(cAvidaDriver_Population *pop);
  void avidaSteppedSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaBreakSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaClosedSig(cAvidaDriver_Population *pop);
  void doStartAvidaSig();
protected:
  /*
  Entry point into the driver by the processing thread, and contains the
  main loop of the processing thread.  The loop continues until told by
  the gui thread to exit.  The processing thread is a state machine; the
  loop drives the transitions between states.
  */
  virtual void run();
protected:
  /*
  State transitions are effected by setting the pointers
  m_mode_function, m_fast_stage_function, and m_step_stage_function to
  point to various of the following nine functions.
  */
  /*
  m_mode_function is set by the gui thread to one of following three
  functions, which is then executed by the processing thread.
  */
  bool fastMode();
  bool stepMode();
  bool closingMode();
  /*
  m_fast_stage_function, and m_step_stage_function are each set to one
  of the following functions by the processing thread.  If the
  processing thread is in fast mode then fastMode() will execute
  m_fast_stage_function; otherwise stepMode() will execute
  m_step_stage_function.

  If the processing thread is in closingMode() then it shuts itself down.
  */
  bool preUpdate();
  bool fastUpdate();
  bool stepUpdate();
  bool postUpdate();
  bool ptMutations();
  bool postPtMutations();
protected:
  virtual void SignalBreakpoint(void);
protected:
  /*
  When the processing thread is about to perform an important state
  transition (e.g., from the end of the current update to the start of
  the next one), it sets the pointer m_emit_function to one of the
  following four functions; then it alerts the gui thread of the pending
  state transition by sending an event of type "avdGuiMsgEvent" to the
  gui thread.  The gui thread catches this event in customEvent(),
  below.  After catching the event, the gui thread calls the function
  pointed to by m_emit_function.  This alerts the various user interface
  elements of the state change.

  Meanwhile, the processing thread stops and waits for new instructions
  by the gui thread.
  */
  void emitUpdatedSig();
  void emitSteppedSig();
  void emitBreakpointSig();
  void emitClosedSig();
protected:
  void customEvent(QCustomEvent *custom_event);
};
 
#endif
    
// arch-tag: header file for avida processing thread
/* vim: set ts=2 ai et: */
