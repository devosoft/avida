# -*- coding: utf-8 -*-

from AvidaCore import cConfig

from math import exp
from qt import *
from qtcanvas import *
#from pyPetriDishView import pyPetriDishView

#class pyPetriDishCtrl(pyPetriDishView):
class pyPetriDishCtrl(QWidget):

  def __init__(self,parent = None,name = None,fl = 0):
    #pyPetriDishView.__init__(self,parent,name,fl)
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyPetriDishCtrl")

    self.resize(QSize(202,202).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)

  def setAvidaSlot(self, avida):
    print "pyPetriDishCtrl.setAvidaSlot() : called."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)

    self.m_map_cell_w = 2
    self.m_map_cell_h = 2
    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()

    if self.m_canvas: del self.m_canvas
    self.m_canvas = QCanvas(self.m_map_cell_w * world_w, self.m_map_cell_h * world_h)
    self.m_canvas_view.setCanvas(self.m_canvas)

    if self.m_cell_info: del self.m_cell_info
    self.m_cell_info = [[QCanvasRectangle(
      x * self.m_map_cell_w,
      y * self.m_map_cell_h,
      self.m_map_cell_w,
      self.m_map_cell_h,
      self.m_canvas) for y in range(world_h)] for x in range(world_w)]

    for x in range(world_w):
      for y in range(world_h):
        self.m_cell_info[x][y].setBrush(QBrush(QColor(x*255/world_w, y*255/world_h, x*y*255/(world_w*world_h))))
        self.m_cell_info[x][y].setPen(QPen(QColor(x*255/world_w, y*255/world_h, x*y*255/(world_w*world_h))))
        self.m_cell_info[x][y].show()

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)

    self.m_canvas = None
    self.m_cell_info = None
    self.m_petri_dish_layout = QVBoxLayout(self,0,0,"m_petri_dish_layout")
    self.m_petri_dish_layout.setResizeMode(QLayout.Minimum)
    self.m_canvas_view = QCanvasView(None, self,"m_canvas_view")
    self.m_petri_dish_layout.addWidget(self.m_canvas_view)
    
  def calcColorScale(self):
    self.m_cs_min_value = 0
    self.m_cs_value_range = self.m_avida.m_population.GetStats().GetMaxFitness()

  def doubleToColor(self, x):
    def sigmoid(w, midpoint, steepness):
      val = steepness*(w-midpoint)
      return exp(val)/(1+exp(val))     

    y = 1
    if self.m_cs_value_range > 0: y = (x - self.m_cs_min_value)/self.m_cs_value_range
    if y > 1: y = 1
    elif y < 0: y = 0

    h = (y*360 + 100) % 360
    v = sigmoid(y, 0.3, 10) * 255
    s = sigmoid(1 - y, 0.1, 30) * 255

    return QColor(h, s, v, QColor.Hsv)

  def calcCellState(self, population_cell):
    state = Qt.black
    if population_cell.IsOccupied():
      organism = population_cell.GetOrganism()
      phenotype = organism.GetPhenotype()
      dbl = phenotype.GetFitness()
      state = self.doubleToColor(dbl)
    return state

  def avidaUpdatedSlot(self):
    print "pyPetriDishCtrl.avidaUpdatedSlot() : called."
    self.calcColorScale()
    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()
    for x in range(world_w):
      for y in range(world_h):
        cell = self.m_avida.m_population.GetCell(x + world_w*y)
        color = self.calcCellState(cell)
        xm = (x + world_w/2) % world_w
        ym = (y + world_h/2) % world_h
        self.m_cell_info[xm][ym].setBrush(QBrush(color))
        self.m_cell_info[xm][ym].setPen(QPen(color))
    self.m_canvas.update()
