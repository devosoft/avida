#ifndef AVD_MAP_CANVAS_VIEW_HH
#include "map_gui/avd_map_canvas_view.hh"
#endif

avdMapCanvasView::avdMapCanvasView(QWidget *parent, const char *name, WFlags f)
: QCanvasView(parent, name, f)
{}
avdMapCanvasView::avdMapCanvasView(QCanvas *canvas, QWidget *parent, const char *name, WFlags f)
: QCanvasView(canvas, parent, name, f)
{}

// arch-tag: implementation file for map canvas view
