#ifndef XQT_TEST_CASE_HH
#define XQT_TEST_CASE_HH

#ifndef TLIST_HH
#include "tList.hh"
#endif

#ifndef TEST_CASE_H
#include "third-party/yaktest/test_case.h"
#endif
#ifndef TEST_RESULT_H
#include "third-party/yaktest/test_result.h"
#endif

/* XXX disables the Loki small-object allocator; it crashes under OSX10. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
#ifndef FUNCTOR_INC
#include "third-party/Loki/Functor.h"
#endif

#ifndef QOBJECT_H
#include <qobject.h>
#endif

class QApplication;
class QTimer;
class cXQtTestCase : public QObject, public test_case {
Q_OBJECT
protected: // variables
  typedef Loki::Functor<void> tFunctor;
  tList<tFunctor> m_functor_list;
  QApplication *m_a;
  QTimer *m_t;
  QTimer *m_deadman;
  int m_argc;
  char **m_argv;
protected: // constructors/destructors
  cXQtTestCase(int argc, char **argv);
  ~cXQtTestCase();
protected: // manipulators
  void restartTimer(int milliseconds = 0, bool one_shot = true);
  void resetDeadMan(int milliseconds = 5000);
  int execApp();
  void exitApp(int retval = 0);
  void pushFunctor(tFunctor &functor);
  tFunctor *popFunctor();
protected slots:
  virtual void twiddle();
  virtual void timeOut();
  virtual void testDriver();
};

#endif
