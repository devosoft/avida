//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
// 
// Read the COPYING and README files, or contact 'avida@alife.org',
//
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_MESSAGE_QUEUE_HH
#define AVD_MESSAGE_QUEUE_HH

//#ifndef QMUTEX_H
#include <qmutex.h>
//#endif
//#ifndef QOBJECT_H
#include <qobject.h>
//#endif
//#ifndef QVALUELIST_H
#include <qvaluelist.h>
//#endif
#ifndef QWAITCONDITION_H
#include <qwaitcondition.h>
#endif

//template <class T> class QValueList;
//class QMutex;
//class QWaitCondition;

template<class T>
class avd_MessageQueue {

protected:

  QValueList<T> m_queue_data; 
  QMutex *m_mutex;
  QWaitCondition *m_wait_condition;
  bool m_own_mutex;
  bool m_own_wait_condition;
  T m_bogus_value;

private:

  // prevent copying and assignment.
  avd_MessageQueue(avd_MessageQueue<T> &); // never defined.
  void operator=(avd_MessageQueue<T> &); // never defined.

public:

  avd_MessageQueue(
    T bogus_value,
    QMutex *mutex = 0,
    QWaitCondition *wait_condition = 0
  ){
    m_bogus_value = bogus_value;

    // if mutex was unspecified, create our own.
    if(0 == mutex){
      m_mutex = new QMutex();
      m_own_mutex = true;
    }else{
      m_mutex = mutex;
      m_own_mutex = false;
    }

    // if wait_condition was unspecified, create our own.
    if(0 == wait_condition){
      m_wait_condition = new QWaitCondition();
      m_own_wait_condition = true;
    }else{
      m_wait_condition = wait_condition;
      m_own_wait_condition = false;
    }
  }

  ~avd_MessageQueue(void){
    m_mutex->lock();

    // if we created our own wait_condition, delete it.
    if(m_own_wait_condition){ delete m_wait_condition; }
    m_mutex->unlock();

    // if we created our own mutex, delete it.
    if(m_own_mutex){ delete m_mutex; }
  }

  void send(const T &message){
    m_mutex->lock();
    m_queue_data.push_back(message);
    m_mutex->unlock();
    //m_wait_condition->wakeOne();
    m_wait_condition->wakeAll();
  }

  void urgent(const T &message){
    m_mutex->lock();
    m_queue_data.push_front(message);
    m_mutex->unlock();
    //m_wait_condition->wakeOne();
    m_wait_condition->wakeAll();
  }

  T receive(int timeout = ULONG_MAX){
    m_mutex->lock();
    if(m_queue_data.empty()){
      m_mutex->unlock();
      if(m_wait_condition->wait(timeout) == false){

        // timed-out before meeting wait condition; no messages.
        return m_bogus_value;
      }
      m_mutex->lock();
    }
    if(m_queue_data.empty()){

      // queue was emptied behind our backs; no messages.
      m_mutex->unlock();
      return m_bogus_value;
    }
    T message = m_queue_data.front();
    m_queue_data.pop_front();
    m_mutex->unlock();
    return message;
  }

  int getNumberPending(){
    m_mutex->lock();
    size_t size = m_queue_data.size();
    m_mutex->unlock();
    return size;
  }

  void flush(){
    m_mutex->lock();
    m_queue_data.clear();
    m_mutex->unlock();
  }

  void forceWakeOne(void){ m_wait_condition->wakeOne(); }
  void forceWakeAll(void){ m_wait_condition->wakeAll(); }
};


#endif /* ! AVD_MESSAGE_QUEUE_HH */
