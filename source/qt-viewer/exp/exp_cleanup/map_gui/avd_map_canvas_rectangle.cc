#ifndef AVD_MAP_CANVAS_RECTANGLE_HH
#include "map_gui/avd_map_canvas_rectangle.hh"
#endif

avdMapCanvasRectangle::avdMapCanvasRectangle(QCanvas *canvas)
: QCanvasRectangle(canvas)
{}
avdMapCanvasRectangle::avdMapCanvasRectangle(const QRect &r, QCanvas *canvas)
: QCanvasRectangle(r, canvas)
{}
avdMapCanvasRectangle::avdMapCanvasRectangle(int x, int y, int width, int height, QCanvas *canvas)
: QCanvasRectangle(x, y, width, height, canvas)
{}

// arch-tag: implementation file for map canvas rectangle object
