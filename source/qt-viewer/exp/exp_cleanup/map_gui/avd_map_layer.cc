#ifndef AVD_MAP_CANVAS_HH
#include "map_gui/avd_map_canvas.hh"
#endif
#ifndef AVD_MAP_LAYER_HH
#include "map_gui/avd_map_layer.hh"
#endif

avdMapLayer::avdMapLayer(avdMapCanvas *canvas, cPopulation *population)
: m_population(0)
{
  setCanvas(canvas);
  setPopulation(population);
}
avdMapLayer::~avdMapLayer()
{
  setCanvas(0);
  setPopulation(0);
}
void avdMapLayer::setCanvas(avdMapCanvas *canvas)
{ SETretainable(m_canvas, canvas); }
void avdMapLayer::setPopulation(cPopulation *population)
{ m_population = population; }

// arch-tag: implementation file for map layer objects
