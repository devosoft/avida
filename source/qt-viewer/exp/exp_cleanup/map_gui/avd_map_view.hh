#ifndef AVD_MAP_VIEW_HH
#define AVD_MAP_VIEW_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class avdMapCanvasView;
class avdMapView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdMapCanvasView> m_canvas_view;
  void setCanvasView(avdMapCanvasView *m_canvas_view);
public:
  avdMapView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avdMapView();
  avdMapCanvasView *getCanvasView(){ return m_canvas_view; }
};

#endif

// arch-tag: header file for map gui view
