#ifndef QT_TEST_CASE_HH
#include "qt_test_case.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QTIMER_H
#include <qtimer.h>
#endif

cQtTestCase::cQtTestCase(int argc, char **argv)
: QObject(0, "<cQtTestCase>")
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
cQtTestCase::~cQtTestCase(){
  if(m_deadman){ delete m_deadman; m_deadman = 0; }
  if(m_t){ delete m_t; m_t = 0; }
  if(m_a){ delete m_a; m_a = 0; }
}
void cQtTestCase::restartTimer(int milliseconds, bool one_shot){
  if(m_t) m_t->start(milliseconds, one_shot);
}
void cQtTestCase::resetDeadMan(int milliseconds){
  if(m_deadman) m_deadman->changeInterval(milliseconds);
}
int cQtTestCase::execApp(){
  return (m_a != 0)?(m_a->exec()):(1);
}
void cQtTestCase::exitApp(int retval){
  if (m_a != 0) m_a->exit(retval);
}
void cQtTestCase::pushFunctor(tFunctor &functor){
  m_functor_list.PushRear(&functor);
}
tFunctor *popFunctor(){
  return m_functor_list.Pop();
}
void cQtTestCase::twiddle(){
  restartTimer();
  qDebug("twiddling my thumbs...");
  resetDeadMan();
}
void cQtTestCase::timeOut(){
  qWarning("ERROR: timed out while waiting for event to be triggered...");
  test_non_error(false);
  twiddle();
}
void cQtTestCase::testDriver(){
  if(tFunctor *functor = popFunctor()){ (*functor)(); }
  else exitApp();
}
