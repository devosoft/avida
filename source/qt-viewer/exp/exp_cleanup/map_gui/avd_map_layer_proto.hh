#ifndef AVD_MAP_LAYER_PROTO_HH
#define AVD_MAP_LAYER_PROTO_HH

class avdMapCanvas;
class cPopulation;
class avdMapLayer : public cRetainable {
  QGuardedPtr<avdMapCanvas> m_canvas;
  cPopulation *m_population;
public:
  avdMapLayer(avdMapCanvas *canvas, cPopulation *population);
  ~avdMapLayer();
  void setCanvas(avdMapCanvas *canvas);
  void setPopulation(cPopulation *population);
  avdMapCanvas *getCanvas(){ return m_canvas; }
  cPopulation *getPopulation(){ return m_population; }
};

#endif

// arch-tag: proto file for map layer objects
