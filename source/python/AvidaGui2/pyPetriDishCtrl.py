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



  def mousePressEvent(self,e):
    if e.button() != Qt.LeftButton:
      return
    print "mouse pressed"
    a = e.x()
    b = e.y()

    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()

    print "world_w is %d, world_h is %d" %(world_w,world_h)
    

    print "e.x is %f, e.y is %f" %(a,b)
	
	
    clicked_map_cell_w = round(float(e.x())/2.0)						 
    clicked_map_cell_h = round(float(e.y())/2.0)


    print "clicked_map_cell_w is %f, clicked_map_cell_h is %f" %(clicked_map_cell_w,clicked_map_cell_h)    


    #get the cell number in avida that corresponds to this coordinate
    world_w_mid = world_w/2
    world_h_mid = world_h/2
    bottom_left_cell = ((((world_h+1)-world_w_mid)*world_w) - world_w_mid)

    if clicked_map_cell_h == world_h_mid:   #if it is on the y midpoint line that gets split...
      if clicked_map_cell_w >= world_w_mid: #and it is after the origin (first avida cell)
        clickedCellNum = clicked_map_cell_w - world_w_mid
      else:
        clickedCellNum = ((world_h_mid-1)*world_w)+ bottom_left_cell + clicked_map_cell_w
    elif clicked_map_cell_h > world_h_mid:  #if above the split
      print "here"
      clickedCellNum = ( (clicked_map_cell_h - (world_h_mid + 1))*world_w ) + world_h_mid + clicked_map_cell_w
    elif clicked_map_cell_h < world_h_mid:  #if below the spilt
      clickedCellNum = ( bottom_left_cell + ( ((clicked_map_cell_h - 1)* world_w) + clicked_map_cell_w) )
    else:
      print "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

    print "clickedCellNum is %f" %(clickedCellNum)

#    clickedCell = self.m_avida.m_population.GetCell(int(clickedCellNum))
#    print "clickedCell.IsOccupied() returns "



    #get the cell number in avida that corresponds to this coordinate
#    world_w_mid = world_w/2
#    world_h_mid = world_h/2
    
#    if clicked_map_cell_h == world_h_mid:   #if it is on the y midpoint line that gets split...
#      if clicked_map_cell_w >= world_w_mid: #and it is after the origin (first avida cell)
#        clickedCellNum = clicked_map_cell_w - world_w_mid
#      else:
#        clickedCellNum = (world_h*world_w)+BOTTOMLEFT+clicked_map_cell_w
#    elif clicked_map_cell_h > world_h_mid:  #if above the split
#      clickedCellNum = ( (clicked_map_cell_h - (world_h_mid + 1))*world_w ) + clicked_map_cell_w
#    elif clicked_map_cell_h < world_h_mid:  #if below the spilt
#      clickedCellNum = ( ( ( (world_h+1)-world_w_mid)*world_w) - world_w_mid) + clicked_map_cell_w 
#    else: 
#      print "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"    

    clickedCellNum = 20000
    print "clickedCellNum is %f" %(clickedCellNum)

      
    #the line below does not work, delete
    #clickedCell = self.m_avida.m_population.GetCell(int(clicked_map_cell_w) + world_w*int(clicked_map_cell_h))
    clickedCell = self.m_avida.m_population.GetCell(int(clickedCellNum))
    print "clickedCell.IsOccupied() returns " 
    print clickedCell.IsOccupied()



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
    
  def extractPopulationSlot(self):
    population_dict = {}
    world_w = cConfig.GetWorldX()
    world_h = cConfig.GetWorldY()
    for x in range(world_w):
      for y in range(world_h):
        cell = self.m_avida.m_population.GetCell(x + world_w*y)
        if cell.IsOccupied() == True:
          organism = cell.GetOrganism()
          genome = organism.GetGenome()
          population_dict[cell.GetID()] = str(genome.AsString())
    self.emit(PYSIGNAL("freezeDishPhaseIISig"), ("/freezer", population_dict, ))
