#ifndef AVD_MAP_CTRL_HH
#define AVD_MAP_CTRL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif


class QMenuBar;
class avdMapCanvas;
class avdMapLayer;
class avdMapView;
class cAvidaDriver_Population;
class avdMapCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdMapCanvas> m_canvas;
  avdMapLayer *m_test_layer;
  QGuardedPtr<avdMapView> m_view;
protected:
  void setCanvas(avdMapCanvas *canvas);
  void setTestLayer(avdMapLayer *test_layer);
  void setView(avdMapView *view);
  avdMapCanvas *getCanvas(){ return m_canvas; }
  avdMapLayer *getTestLayer(){ return m_test_layer; }
  avdMapView *getView(){ return m_view; }
public:
  avdMapCtrl(QObject *parent = 0, const char *name = 0);
  ~avdMapCtrl();
  bool setup();
public slots:
  void avidaStateChangedSlot(cAvidaDriver_Population *pop);
signals:
  void doResetMenuBar(QMenuBar *);
};

#endif

// arch-tag: header file for map controller
