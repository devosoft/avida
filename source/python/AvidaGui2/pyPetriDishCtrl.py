
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
from qt import *
from qtcanvas import *
#from pyPetriDishView import pyPetriDishView

class pyPopulationCellItem:
  def __init__(self, population_cell, x, y, w, h, canvas):
    self.m_population_cell = population_cell
    self.m_canvas_rectangle = QCanvasRectangle(x, y, w, h, canvas)
    self.m_canvas_rectangle.show()
    self.m_index = 0

  def checkNormalizedIndexUsingFunctor(self, functor, min, range):
    index = 1.0
    if 0.0 < range: index = (functor(self.m_population_cell) - min) / range
    if 1.0 < index: index = 1.0
    elif index < 0.0: index = 0.0

    if self.m_index == index:
      return False
    else:
      self.m_index = index
      return True

  def updateColorUsingFunctor(self, functor):
    color = functor(self.m_index)
    self.m_canvas_rectangle.setBrush(QBrush(color))
    self.m_canvas_rectangle.setPen(QPen(color))

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
        old_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      old_avida.removeGuiWorkFunctor(self)
      del old_avida
    if(self.m_avida):
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)

    self.m_map_cell_w = 10
    self.m_map_cell_h = 10
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

  def mousePressEvent(self,e):
    if e.button() != Qt.LeftButton:
      return


    #if the run has not started yet, do nothing
    if self.m_avida == None:
      return

    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()

    clicked_map_cell_w = round(float(e.x())/self.m_map_cell_w)				 
    clicked_map_cell_h = round(float(e.y())/self.m_map_cell_h)

#    print "clicked_map_cell_w is %f, clicked_map_cell_h is %f" %(clicked_map_cell_w,clicked_map_cell_h)


   
    #if the user clicks outside the area that has organisms, do nothing
    if (clicked_map_cell_w > world_w) or (clicked_map_cell_h > world_h):
      return
    
    
    #get the cell number in avida that corresponds to this coordinate under the system where the 0th cell is in the middle
    #this code is obsolete if the 0th cell is in the top right corner
#    world_w_mid = world_w/2
#    world_h_mid = world_h/2
#    bottom_left_cell = ((((world_h+1)-world_w_mid)*world_w) - world_w_mid)

#    if clicked_map_cell_h == world_h_mid:   #if it is on the y midpoint line that gets split...
#      if clicked_map_cell_w >= world_w_mid: #and it is after the origin (first avida cell)
#        clickedCellNum = clicked_map_cell_w - world_w_mid
#      else:
#        clickedCellNum = ((world_h_mid-1)*world_w)+ bottom_left_cell + clicked_map_cell_w
#    elif clicked_map_cell_h > world_h_mid:  #if above the split
#      clickedCellNum = ( (clicked_map_cell_h - (world_h_mid + 1))*world_w ) + world_h_mid + clicked_map_cell_w
#    elif clicked_map_cell_h < world_h_mid:  #if below the spilt
#      clickedCellNum = ( bottom_left_cell + ( ((clicked_map_cell_h - 1)* world_w) + clicked_map_cell_w) )


    clickedCellNum = (((clicked_map_cell_h-1)*world_h) + clicked_map_cell_w-1)


    clickedCell = self.m_avida.m_population.GetCell(int(clickedCellNum))

    #broadcast that an organism was clicked on
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("orgClickedOnSig"), (clickedCellNum,))





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

  def doSomeWork(self, avida):
    def temp_normalized_index_functor(population_cell):
      dbl = 0.0
      if population_cell.IsOccupied():
        dbl = population_cell.GetOrganism().GetPhenotype().GetFitness()
      return dbl

    for x in range(3600):
      if len(self.m_cell_info) <= self.m_thread_work_cell_item_index:
        self.m_thread_work_cell_item_index = 0
        return False
      else:
        cell_info_item = self.m_cell_info[self.m_thread_work_cell_item_index]
        if cell_info_item.checkNormalizedIndexUsingFunctor(
          temp_normalized_index_functor,
          self.m_cs_min_value,
          self.m_cs_value_range
        ):
          self.m_changed_cell_items.append(cell_info_item)
        self.m_thread_work_cell_item_index += 1
    return True


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
