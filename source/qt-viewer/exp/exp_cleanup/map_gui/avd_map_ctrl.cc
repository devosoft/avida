#ifndef AVD_MAP_CANVAS_HH
#include "map_gui/avd_map_canvas.hh"
#endif
#ifndef AVD_MAP_CANVAS_VIEW_HH
#include "map_gui/avd_map_canvas_view.hh"
#endif
#ifndef AVD_MAP_GUI_DBG_HH
#include "map_gui/avd_map_gui_dbg.hh"
#endif
#ifndef AVD_MAP_LAYER_HH
#include "map_gui/avd_map_layer.hh"
#endif
#ifndef AVD_MAP_VIEW_HH
#include "map_gui/avd_map_view.hh"
#endif
#ifndef AVD_MAP_CTRL_HH
#include "map_gui/avd_map_ctrl.hh"
#endif

void avdMapCtrl::setCanvas(avdMapCanvas *canvas){ SETretainable(m_canvas, canvas); }
void avdMapCtrl::setTestLayer(avdMapLayer *test_layer){ SETretainable(m_test_layer, test_layer); }
void avdMapCtrl::setView(avdMapView *view){ SETretainable(m_view, view); }

avdMapCtrl::avdMapCtrl(QObject *parent, const char *name)
: QObject(parent, name)
, m_test_layer(0)
{
  Debug << "<avdMapCtrl> ctor; creating view.";

  avdMapView *view = new avdMapView(0, "<avdMapCtrl(m_view)>");
  setView(view); view->release();
  getView()->nameWatched("<avdMapCtrl(m_view)>");
  connect(getView(), SIGNAL(destroyed()), this, SLOT(deleteLater()));

  avdMapCanvas *canvas = new avdMapCanvas(getView(), "<avdMapCtrl(m_canvas)>");
  setCanvas(canvas); getCanvas()->release();
  getCanvas()->nameWatched("<avdMapCtrl(m_canvas)>");
  getView()->getCanvasView()->setCanvas(getCanvas());

  avdMapLayer *test_layer = new avdMapLayer(getCanvas(), 0);
  setTestLayer(test_layer); getTestLayer()->release();
  getTestLayer()->nameWatched("<avdMapCtrl(m_test_layer)>");

  Debug << "<avdMapCtrl> created view; done.";
}
avdMapCtrl::~avdMapCtrl(){
  setCanvas(0);
  setTestLayer(0);
  setView(0);
  Debug << "<avdMapCtrl> dtor.";
}
bool avdMapCtrl::setup(){
  Debug << "entered stub method.";
  emit(doResetMenuBar(getView()->menuBar()));
  getView()->show();
  return true;
}

void avdMapCtrl::avidaStateChangedSlot(cAvidaDriver_Population *pop){
  Debug << "entered stub method.";
}

// arch-tag: implementation file for map controller
