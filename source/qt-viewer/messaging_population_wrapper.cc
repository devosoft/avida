//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

//#include <qmutex.h>
#include <qtimer.h>

#include "messaging_population_wrapper.hh"
//#include "avd_message_queue_hdrs.hh"
#include "avd_message_queue.hh"

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

using namespace std;


static int lock_count;

void MessagingPopulationWrapper::lockreader(void){
  GenDebug(
    "<MessagingPopulationWrapper::lockreader> "
    "lock_count "
  )(lock_count)(".\n");
}

MessagingPopulationWrapper::MessagingPopulationWrapper(
  cPopulation *population,
  QObject *parent,
  const char *name
)
: PopulationWrapper(population, parent, name)
{
  //m_mutex = new QMutex(true); // parameter true:  recursive mutex. 
  m_mutex = new QMutex(false); // parameter false:  nonrecursive mutex. 
  m_avida_thread_q = new avd_MessageQueue<QString>(
    /*
    FIXME:  switch to static const "null_message" from qtdriver class.
    */
    QString(),
    m_mutex
  );
  m_gui_thread_q = new avd_MessageQueue<QString>(
    /*
    FIXME:  switch to static const "null_message" from qtdriver class.
    */
    QString(),
    m_mutex
  );

  /*
  XXX:  for double-checking that mutex/locking is in order.
  namely, value lock_count should vary in a constant range.
  */
  lock_count = 0;
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(lockreader()));
  //timer->start(1000);
}

MessagingPopulationWrapper::~MessagingPopulationWrapper(void){
  delete m_gui_thread_q;
  delete m_avida_thread_q;
  delete m_mutex;
}

// m_mutex management functions.

void MessagingPopulationWrapper::lock(void){
  m_mutex->lock();
  lock_count++;
}
bool MessagingPopulationWrapper::locked(void){ return m_mutex->locked(); }
bool MessagingPopulationWrapper::tryLock(void){
  bool gotit = m_mutex->tryLock();
  if(gotit){
    lock_count++;
  }
  return gotit;
}
void MessagingPopulationWrapper::unlock(void){
  m_mutex->unlock();
  --lock_count;
}


// m_avida_thread_q management functions.

void
MessagingPopulationWrapper::sendToAvida(const QString &message){
  m_avida_thread_q->send(message);
}

void
MessagingPopulationWrapper::urgentToAvida(const QString &message){
  m_avida_thread_q->urgent(message);
}

QString
MessagingPopulationWrapper::avidaReceive(int timeout){
  return m_avida_thread_q->receive(timeout);
}

int
MessagingPopulationWrapper::avidaCountMessagesPending(void){
  return m_avida_thread_q->getNumberPending();
}

void
MessagingPopulationWrapper::flushAvidaMessages(void){
  m_avida_thread_q->flush();
}

void
MessagingPopulationWrapper::avidaForceWakeOne(void){
  m_avida_thread_q->forceWakeOne();
}

void
MessagingPopulationWrapper::avidaForceWakeAll(void){
  m_avida_thread_q->forceWakeAll();
}


// m_gui_thread_q management functions.

void
MessagingPopulationWrapper::sendToGUI(const QString &message){
  m_gui_thread_q->send(message);
}

void
MessagingPopulationWrapper::urgentToGUI(const QString &message){
  m_gui_thread_q->urgent(message);
}

QString
MessagingPopulationWrapper::GUIReceive(int timeout){
  return m_gui_thread_q->receive(timeout);
}

int
MessagingPopulationWrapper::GUICountMessagesPending(void){
  return m_gui_thread_q->getNumberPending();
}

void
MessagingPopulationWrapper::flushGUIMessages(void){
  m_gui_thread_q->flush();
}

void
MessagingPopulationWrapper::GUIForceWakeOne(void){
  m_gui_thread_q->forceWakeOne();
}

void
MessagingPopulationWrapper::GUIForceWakeAll(void){
  m_gui_thread_q->forceWakeAll();
}

