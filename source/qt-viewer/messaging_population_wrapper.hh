//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGING_POPULATION_WRAPPER_HH
#define MESSAGING_POPULATION_WRAPPER_HH

#include <limits.h>
#include <qstring.h>

#include "population_wrapper.hh"
//#include "avd_message_queue_hdrs.hh"

template <class T> class avd_MessageQueue;
class QMutex;
//class QString;


class
MessagingPopulationWrapper : public PopulationWrapper {

  Q_OBJECT

private:

  // m_mutex should be a recursive mutex until we're certain that
  // recursive locking isn't required.
  QMutex *m_mutex;

  // m_avida_thread_q and m_gui_thread_q should share m_mutex, but
  // should create their own wait conditions.
  avd_MessageQueue<QString> *m_avida_thread_q;
  avd_MessageQueue<QString> *m_gui_thread_q;

private slots:
  void lockreader(void);

public:

  MessagingPopulationWrapper(
    cPopulation *population,
    QObject *parent = 0,
    const char *name = 0
  );
  ~MessagingPopulationWrapper();

  // m_mutex management functions.
  void lock(void);
  bool locked(void);
  bool tryLock(void);
  void unlock(void);

  // m_avida_thread_q management functions.
  void sendToAvida(const QString &message);
  void urgentToAvida(const QString &message);
  QString avidaReceive(int timeout = ULONG_MAX);
  int avidaCountMessagesPending(void);
  void flushAvidaMessages(void);
  void avidaForceWakeOne(void);
  void avidaForceWakeAll(void);
  
  // m_gui_thread_q management functions.
  void sendToGUI(const QString &message);
  void urgentToGUI(const QString &message);
  QString GUIReceive(int timeout = ULONG_MAX);
  int GUICountMessagesPending(void);
  void flushGUIMessages(void);
  void GUIForceWakeOne(void);
  void GUIForceWakeAll(void);
  
public slots:
  void startAvidaSlot(void){ emit startAvidaSig(); }
  void stopAvidaSlot(void){ emit stopAvidaSig(); }
  void stepAvidaSlot(int cell_id){ emit stepAvidaSig(cell_id); }
  void updateAvidaSlot(void){ emit updateAvidaSig(); }
  void resetAvidaSlot(void){ emit resetAvidaSig(); }
  void exitAvidaSlot(void){ emit exitAvidaSig(); }

  void avidaUpdatedSlot(void){ emit avidaUpdatedSig(); }
  void avidaSteppedSlot(int cell_id){ emit avidaSteppedSig(cell_id); }
  void avidaBreakpointSlot(int cell_id){ emit avidaBreakpointSig(cell_id); }
  void avidaStoppedSlot(void){ emit avidaStoppedSig(); }

  void queryGUISlot(void){ emit queryGUISig(); }
  void guiStatusSlot(bool stopped){ emit guiStatusSig(stopped); }
signals:
  void startAvidaSig(void);
  void stopAvidaSig(void);
  void stepAvidaSig(int cell_id);
  void updateAvidaSig(void);
  void resetAvidaSig(void);
  void exitAvidaSig(void);

  void avidaUpdatedSig(void);
  void avidaStoppedSig(void);
  void avidaSteppedSig(int cell_id);
  void avidaBreakpointSig(int cell_id);

  void queryGUISig(void);
  void guiStatusSig(bool running);
};


#endif /* !MESSAGING_POPULATION_WRAPPER_HH */

