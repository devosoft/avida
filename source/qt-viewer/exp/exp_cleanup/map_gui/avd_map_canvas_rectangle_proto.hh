#ifndef AVD_MAP_CANVAS_RECTANGLE_PROTO_HH
#define AVD_MAP_CANVAS_RECTANGLE_PROTO_HH

class QCanvas;
class QRect;
class avdMapCanvasRectangle : public QCanvasRectangle, public cRetainable {
public:
  avdMapCanvasRectangle(QCanvas *canvas);
  avdMapCanvasRectangle(const QRect &r, QCanvas *canvas);
  avdMapCanvasRectangle(int x, int y, int width, int height, QCanvas *canvas);
};

#endif

// arch-tag: proto file for map canvas rectangle object
