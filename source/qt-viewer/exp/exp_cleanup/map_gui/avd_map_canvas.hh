#ifndef AVD_MAP_CANVAS_HH
#define AVD_MAP_CANVAS_HH

#ifndef QCANVAS_H
#include <qcanvas.h>
#endif
#ifndef QPIXMAP_H
#include <qpixmap.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class avdMapCanvas : public QCanvas, public cRetainable {
public:
  avdMapCanvas(QObject *parent = 0, const char *name = 0);
  avdMapCanvas(int w, int h);
  avdMapCanvas(QPixmap p, int h, int v, int tilewidth, int tileheight);
};

#endif

// arch-tag: header file for map canvas object
