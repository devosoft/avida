#ifndef AVD_MAP_CANVAS_VIEW_HH
#define AVD_MAP_CANVAS_VIEW_HH

#ifndef QCANVAS_H
#include <qcanvas.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class avdMapCanvasView : public QCanvasView, public cRetainable {
public:
  avdMapCanvasView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
  avdMapCanvasView(QCanvas *canvas, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
};

#endif

// arch-tag: header file for map canvas view
