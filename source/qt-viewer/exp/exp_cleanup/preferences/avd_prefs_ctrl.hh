#ifndef AVD_PREFS_CTRL_HH
#define AVD_PREFS_CTRL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif
#ifndef TRETAINABLE_LIST_HH
#include "containers/tRetainable_list.hh"
#endif


class QMenuBar;

class avdMCMediator;
class avdPrefsView;
class avdPrefsItem;
class avdPrefsItemView;

class myInterface;
class cTestInterface;

class avdPrefsCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdPrefsView> m_view;
  tRetainableList<avdPrefsItem> *m_prefs_items;
  QGuardedPtr<avdMCMediator> m_mdtr;
public:
  void setView(avdPrefsView *view);
  void setPrefsItemsList(tRetainableList<avdPrefsItem> *itemlist);
  void setMdtr(avdMCMediator *mdtr);

  avdPrefsView *getView(){ return m_view; }
  tRetainableList<avdPrefsItem> *getPrefsItemsList(){ return m_prefs_items; }
  avdMCMediator *getMdtr(){ return m_mdtr; }
public:
  avdPrefsCtrl(QObject *parent=0, const char *name=0);
  ~avdPrefsCtrl();
  bool setup();
signals:
  void doResetMenuBar(QMenuBar *);

  void settingsChangedSig();

  void doSendMyInterface(myInterface *);
  void doSendTestInterface(cTestInterface *);
public slots:
  void showPrefsGUISlot();
};

#endif

// arch-tag: header file for preferences gui controller
