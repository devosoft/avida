#ifndef AVD_MAP_CANVAS_HH
#include "map_gui/avd_map_canvas.hh"
#endif

avdMapCanvas::avdMapCanvas(QObject *parent, const char *name)
: QCanvas(parent, name)
{}
avdMapCanvas::avdMapCanvas(int w, int h)
: QCanvas(w, h)
{}
avdMapCanvas::avdMapCanvas(QPixmap p, int h, int v, int tilewidth, int tileheight)
: QCanvas(p, h, v, tilewidth, tileheight)
{}

// arch-tag: implementation file for map canvas object
