
# -*- coding: utf-8 -*-

# Notes:
# - w*h population cell_info_items;
#   - cell_info_item[0] refers to population cell(0).
#   - x ord of item[n]: 
#   - stores normalized_index
#

# Coordinates for grid cells in toroidal geometry:
# x: n % w
# y: n / w
# xm: (x + w/2) % w
# ym: (y + h/2) % h
#
# xm: (n%w + w/2) % w
# ym: (n/w + h/2) % h

# Coordinates for grid cells in bounded geometry:
# x: n % w
# y: n / w
#
# xm, ym not needed. replace w/ x & y.

print """
XXX fixme: in pyPetriDishCtrl.py,
pyPetriDishCtrl.setAvidaSlot(),
most of the code in this function should only be performed when
self.m_avida is not None.
@kgn
"""

from AvidaCore import cConfig

from math import exp
from qt import Qt, QWidget, QSize, QVBoxLayout, QLayout, QColor, QWMatrix, PYSIGNAL
from qtcanvas import QCanvas
from pyPetriCanvasView import pyPetriCanvasView
from pyPopulationCellItem import pyPopulationCellItem
#from pyPetriDishView import pyPetriDishView

#class pyPetriDishCtrl(pyPetriDishView):
class pyPetriDishCtrl(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    #pyPetriDishView.__init__(self,parent,name,fl)
    QWidget.__init__(self,parent,name,fl)
    if not name: self.setName("pyPetriDishCtrl")

    self.resize(QSize(202,202).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None

    self.m_canvas = None
    self.m_cell_info = None
    self.m_petri_dish_layout = QVBoxLayout(self,0,0,"m_petri_dish_layout")
    self.m_petri_dish_layout.setResizeMode(QLayout.Minimum)
    self.m_canvas_view = pyPetriCanvasView(None, self,"m_canvas_view")
    self.m_petri_dish_layout.addWidget(self.m_canvas_view)
    self.m_changed_cell_items = []
    self.m_indexer = None

    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_canvas_view, PYSIGNAL("orgClickedOnSig"),
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("orgClickedOnSig"))

  def setAvidaSlot(self, avida):
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      old_avida.removeGuiWorkFunctor(self)
      del old_avida
    if(self.m_avida):
      pass

    self.m_map_cell_w = 5
    self.m_map_cell_h = 5
    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()

    if self.m_canvas: del self.m_canvas
    self.m_canvas = QCanvas(self.m_map_cell_w * world_w, self.m_map_cell_h * world_h)
    self.m_canvas_view.setCanvas(self.m_canvas)

    if self.m_cell_info: del self.m_cell_info
    self.m_cell_info = [pyPopulationCellItem(
      self.m_avida.m_population.GetCell(n),
      (n%world_w) * self.m_map_cell_w,
      (n/world_w) * self.m_map_cell_h,
      self.m_map_cell_w,
      self.m_map_cell_h,
      self.m_canvas) for  n in range(world_w * world_h)]

    self.m_thread_work_cell_item_index = 0
    self.m_cs_min_value = 0
    self.m_cs_value_range = 0
    self.m_changed_cell_items = self.m_cell_info[:]
    while self.doSomeWork(self.m_avida): pass
    self.avidaUpdatedSlot()
    self.m_avida.addGuiWorkFunctor(self)

  def calcColorScale(self):
    self.m_cs_min_value = 0
    self.m_cs_value_range = self.m_avida.m_population.GetStats().GetMaxFitness()

  def setRange(self, min, max):
    self.m_cs_min_value = min
    self.m_cs_value_range = max - min

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

  def setIndexer(self, indexer):
    print "pyPetriDishCtrl.setIndexer"
    self.m_indexer = indexer

    if self.m_cell_info:
      def temp_color_functor(index):
        return QColor(0, 0, 0)
      for cell_info_item in self.m_cell_info:
        cell_info_item.updateColorUsingFunctor(temp_color_functor)
      self.m_canvas.update()
      self.m_changed_cell_items = self.m_cell_info[:]


  #def doSomeWork(self, avida):
  #  def temp_normalized_index_functor(population_cell):
  #    #dbl = 0.0
  #    #if population_cell.IsOccupied():
  #    #  dbl = population_cell.GetOrganism().GetPhenotype().GetFitness()
  #    #return dbl
  #    return population_cell.IsOccupied() and population_cell.GetOrganism().GetPhenotype().GetFitness() or 0.0

  #  for x in range(3600):
  #    if len(self.m_cell_info) <= self.m_thread_work_cell_item_index:
  #      self.m_thread_work_cell_item_index = 0
  #      return False
  #    else:
  #      cell_info_item = self.m_cell_info[self.m_thread_work_cell_item_index]
  #      if cell_info_item.checkNormalizedIndexUsingFunctor(
  #        temp_normalized_index_functor,
  #        self.m_cs_min_value,
  #        self.m_cs_value_range
  #      ):
  #        self.m_changed_cell_items.append(cell_info_item)
  #      self.m_thread_work_cell_item_index += 1
  #  return True

  def doSomeWork(self, avida):
    def temp_normalized_index_functor(population_cell):
      return population_cell.IsOccupied() and population_cell.GetOrganism().GetPhenotype().GetFitness() or 0.0

    if self.m_indexer:
      for x in range(len(self.m_cell_info)):
        if len(self.m_cell_info) <= self.m_thread_work_cell_item_index:
          self.m_thread_work_cell_item_index = 0
          return False
        else:
          cell_info_item = self.m_cell_info[self.m_thread_work_cell_item_index]
          if self.m_indexer(cell_info_item, self.m_cs_min_value, self.m_cs_value_range):
            self.m_changed_cell_items.append(cell_info_item)
          self.m_thread_work_cell_item_index += 1
      return True
    else:
      return False

  def updateCellItems(self):
    def temp_color_functor(index):
      def sigmoid(w, midpoint, steepness):
        val = steepness*(w-midpoint)
        return exp(val)/(1+exp(val))     
      h = (index*360.0 + 100.0) % 360.0
      v = sigmoid(index, 0.3, 10.0) * 255.0
      s = sigmoid(1.0 - index, 0.1, 30.0) * 255.0
      return QColor(h, s, v, QColor.Hsv)
      
    for cell_info_item in self.m_changed_cell_items:
      cell_info_item.updateColorUsingFunctor(temp_color_functor)
    self.m_changed_cell_items = []
    if self.m_canvas: self.m_canvas.update()

  def avidaUpdatedSlot(self):
    def temp_color_functor(index):
      def sigmoid(w, midpoint, steepness):
        val = steepness*(w-midpoint)
        return exp(val)/(1+exp(val))     
      h = (index*360.0 + 100.0) % 360.0
      v = sigmoid(index, 0.3, 10.0) * 255.0
      s = sigmoid(1.0 - index, 0.1, 30.0) * 255.0
      return QColor(h, s, v, QColor.Hsv)
      
    self.calcColorScale()
    for cell_info_item in self.m_changed_cell_items:
      cell_info_item.updateColorUsingFunctor(temp_color_functor)
    self.m_changed_cell_items = []
    self.m_canvas.update()
    
  def extractPopulationSlot(self):
    population_dict = {}
    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()
    for x in range(world_w):
      for y in range(world_h):
        if self.m_avida != None:
          cell = self.m_avida.m_population.GetCell(x + world_w*y)
          if cell.IsOccupied() == True:
            organism = cell.GetOrganism()
            genome = organism.GetGenome()
            population_dict[cell.GetID()] = str(genome.AsString())
    self.emit(PYSIGNAL("freezeDishPhaseIISig"), ("/freezer", population_dict, ))

  def zoomSlot(self, zoom_factor):
    #print "pyPetriDishCtrl.zoomSlot zoom_factor", zoom_factor
    if self.m_canvas_view:
      m = QWMatrix()
      m.scale(zoom_factor/5.0, zoom_factor/5.0)
      self.m_canvas_view.setWorldMatrix(m)

