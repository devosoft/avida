#ifndef XQT_TEST_CASE_HH
#include "xqt_test_case.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QTIMER_H
#include <qtimer.h>
#endif

cXQtTestCase::cXQtTestCase(int argc, char **argv)
: QObject(0, "<cXQtTestCase>")
, m_a(0)
, m_t(0)
, m_deadman(0)
, m_argc(argc)
, m_argv(argv)
{
  m_a = new QApplication(m_argc, m_argv);
  m_t = new QTimer(this);
  connect(m_t, SIGNAL(timeout()), this, SLOT(testDriver()));
  m_deadman = new QTimer(this);
  connect(m_deadman, SIGNAL(timeout()), this, SLOT(timeOut()));
}
cXQtTestCase::~cXQtTestCase(){
  if(m_deadman){ delete m_deadman; m_deadman = 0; }
  if(m_t){ delete m_t; m_t = 0; }
  if(m_a){ delete m_a; m_a = 0; }
}
void cXQtTestCase::restartTimer(int milliseconds, bool one_shot){
  if(m_t) m_t->start(milliseconds, one_shot);
}
void cXQtTestCase::resetDeadMan(int milliseconds){
  if(m_deadman) m_deadman->changeInterval(milliseconds);
}
int cXQtTestCase::execApp(){
  restartTimer();
  resetDeadMan();
  return (m_a != 0)?(m_a->exec()):(1);
}
void cXQtTestCase::exitApp(int retval){
  if (m_a != 0) m_a->exit(retval);
}
void cXQtTestCase::pushFunctor(tFunctor &functor){
  m_functor_list.PushRear(&functor);
}
cXQtTestCase::tFunctor *cXQtTestCase::popFunctor(){
  return m_functor_list.Pop();
}
void cXQtTestCase::twiddle(){
  restartTimer();
  qDebug("twiddling my thumbs...");
  resetDeadMan();
}
void cXQtTestCase::timeOut(){
  qWarning("ERROR: timed out while waiting for event to be triggered...");
  test_non_error(false);
  twiddle();
}
void cXQtTestCase::testDriver(){
  if(tFunctor *functor = popFunctor()){ (*functor)(); }
  else exitApp();
}
