#ifndef QLINEEDIT_H
#include <qlineedit.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif
#ifndef AVD_MAP_CANVAS_VIEW_HH
#include "map_gui/avd_map_canvas_view.hh"
#endif
#ifndef AVD_MAP_VIEW_HH
#include "map_gui/avd_map_view.hh"
#endif


avdMapView::avdMapView( QWidget *parent, const char *name, WFlags f)
:QMainWindow(parent, name, f){
  QWidget *central = new QWidget(
    this, "<avdMapView(central)>");
  setCentralWidget(central);
  QVBoxLayout *layout = new QVBoxLayout(
    central, 10, -1, "<avdMapView(layout)>");
  avdMapCanvasView *canvas_view = new avdMapCanvasView(
    central, "<avdMapView(canvas_view)>");
  setCanvasView(canvas_view); getCanvasView()->release();
  getCanvasView()->nameWatched("<avdMapView(canvas_view)>");
  layout->addWidget(getCanvasView());
}
avdMapView::~avdMapView(){
  setCanvasView(0);
}
void avdMapView::setCanvasView(avdMapCanvasView *canvas_view)
{ SETretainable(m_canvas_view, canvas_view); }

// arch-tag: implementation file for map gui view
