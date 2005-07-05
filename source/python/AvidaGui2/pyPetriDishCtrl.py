
print """
XXX fixme: in pyPetriDishCtrl.py,
pyPetriDishCtrl.setAvidaSlot(),
most of the code in this function should only be performed when
self.m_avida is not None.
@kgn
"""

from AvidaCore import cConfig
from AvidaCore import cInitFile, cString

from math import exp
from qt import PYSIGNAL, QBrush, QColor, QLayout, QPen, QSize, Qt, QVBoxLayout, QWidget, QWMatrix, QTextDrag, QStoredDrag
from qtcanvas import QCanvas, QCanvasRectangle
from pyPetriCanvasView import pyPetriCanvasView
from pyPopulationCellItem import pyPopulationCellItem
#from pyPetriDishView import pyPetriDishView

class pySquareVBoxLayout(QVBoxLayout):
  def __init__(self, *args): apply(QVBoxLayout.__init__, (self,)+args)
  def hasHeightForWidth(self): return True
  def heightForWidth(self, w): return w

#class pyPetriDishCtrl(pyPetriDishView):
class pyPetriDishCtrl(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    #pyPetriDishView.__init__(self,parent,name,fl)
    QWidget.__init__(self,parent,name,fl)
    if not name: self.setName("pyPetriDishCtrl")

    #self.resize(QSize(202,202).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None

    self.m_canvas = None
    self.m_cell_info = None
    #self.m_petri_dish_layout = QVBoxLayout(self,0,0,"m_petri_dish_layout")
    self.m_petri_dish_layout = pySquareVBoxLayout(self,0,0,"m_petri_dish_layout")
    print "pyPetriDishCtrl.construct() self.m_petri_dish_layout.heightForWidth(20) :", self.m_petri_dish_layout.heightForWidth(20)
    #self.m_petri_dish_layout.setResizeMode(QLayout.Minimum)
    self.m_canvas_view = pyPetriCanvasView(None, self,"m_canvas_view")
    self.m_petri_dish_layout.addWidget(self.m_canvas_view)
    self.m_changed_cell_items = []
    self.m_indexer = None
    self.m_color_lookup_functor = None
    self.m_background_rect = None
    self.m_change_list = None
    self.m_org_clicked_on_item = None
    self.m_occupied_cells_ids = []

    self.m_target_dish_width = 350
    self.m_target_dish_scaling = 5.
    self.m_map_cell_width = 5

    self.connect( self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), 
      self.setAvidaSlot)
    self.connect( self.m_canvas_view, PYSIGNAL("orgClickedOnSig"), 
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("orgClickedOnSig"))
    self.connect( self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("orgClickedOnSig"), self.updateOrgClickedOutlineCellNumberSlot)
    self.connect( self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("orgClickedOnSig"), self.setDragSlot)

  def destruct(self):
    self.m_avida = None
    self.m_canvas = None
    self.m_cell_info = None
    self.m_changed_cell_items = []
    self.m_indexer = None
    self.m_color_lookup_functor = None
    self.m_background_rect = None
    self.m_change_list = None
    self.m_org_clicked_on_item = None
    self.m_occupied_cells_ids = []
    self.m_petri_dish_layout = None
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
    self.disconnect(self.m_canvas_view, PYSIGNAL("orgClickedOnSig"), 
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("orgClickedOnSig"))
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("orgClickedOnSig"), self.updateOrgClickedOutlineCellNumberSlot)
    self.m_canvas_view = None
    self.m_session_mdl = None

  def setColorLookupFunctor(self, color_lookup_functor):
    self.m_color_lookup_functor = color_lookup_functor

  def createNewCellItem(self, n):
    self.m_occupied_cells_ids.append(n)
    return pyPopulationCellItem(
      self.m_avida.m_population.GetCell(n),
      (n%self.m_world_w) * self.m_map_cell_width,
      (n/self.m_world_w) * self.m_map_cell_width,
      self.m_map_cell_width,
      self.m_map_cell_width,
      self.m_canvas)

  def setAvidaSlot(self, avida):
    print "pyPetriDishCtrl.setAvidaSlot() ..."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pyPetriDishCtrl.setAvidaSlot() deleting old_avida ..."
      del old_avida
    if(self.m_avida):
      pass

    self.m_change_list = self.m_avida.m_avida_threaded_driver.GetChangeList()

    self.m_world_w = cConfig.GetWorldX()
    self.m_world_h = cConfig.GetWorldY()
    self.m_initial_target_zoom = int(self.m_target_dish_width / self.m_world_w)
    print "self.m_map_cell_width", self.m_map_cell_width
    
    self.emit(PYSIGNAL("zoomSig"), (self.m_initial_target_zoom,))

    if self.m_canvas: del self.m_canvas
    self.m_canvas = QCanvas(self.m_map_cell_width * self.m_world_w, 
      self.m_map_cell_width * self.m_world_h)
    self.m_canvas.setBackgroundColor(Qt.darkGray)
    self.m_canvas_view.setCanvas(self.m_canvas)

    if self.m_background_rect: del self.m_background_rect
    self.m_background_rect = QCanvasRectangle(
      0, 0,
      self.m_map_cell_width * self.m_world_w,
      self.m_map_cell_width * self.m_world_h,
      self.m_canvas)
    self.m_background_rect.setBrush(QBrush(Qt.black))
    self.m_background_rect.setPen(QPen(Qt.black))
    self.m_background_rect.show()
    self.m_background_rect.setZ(0.0)

    if self.m_cell_info: del self.m_cell_info
    self.m_cell_info = [None] * self.m_world_w * self.m_world_h
    self.m_occupied_cells_ids = []

    self.m_thread_work_cell_item_index = 0
    self.m_cs_min_value = 0
    self.m_cs_value_range = 0
    self.m_changed_cell_items = self.m_cell_info[:]
    self.updateCellItems(True)

  def setDragSlot(self, org_clicked_on_item = None):
    if org_clicked_on_item:
      clicked_cell_num = org_clicked_on_item.m_population_cell.GetID()
      clicked_cell = self.m_avida.m_population.GetCell(int(clicked_cell_num))
      organism = clicked_cell.GetOrganism()

      # tee up drag information
      dragHolder = self.itemDrag( ('organism.' + str(organism.GetGenome().AsString())), self )
      dragHolder.dragCopy()

  def setRange(self, min, max):
    self.m_cs_min_value = min
    self.m_cs_value_range = max - min

  def setIndexer(self, indexer):
    self.m_indexer = indexer

  def updateOrgClickedOutlineCellNumberSlot(self, org_clicked_on_item = None):
    if self.m_org_clicked_on_item:
      self.m_org_clicked_on_item.setPen(QPen(Qt.NoPen))
    self.m_org_clicked_on_item = org_clicked_on_item
    if self.m_org_clicked_on_item:
      self.updateCellItems(self.m_org_clicked_on_item.m_population_cell.GetID())


  def updateCellItem(self, cell_id):
    if self.m_cell_info[cell_id] is None:
      self.m_cell_info[cell_id] = self.createNewCellItem(cell_id)
    cell_info_item = self.m_cell_info[cell_id]
    self.m_indexer(cell_info_item, self.m_cs_min_value, self.m_cs_value_range)
    cell_info_item.updateColorUsingFunctor(self.m_color_lookup_functor)

    if self.m_org_clicked_on_item:
      if cell_info_item.m_population_cell.GetID == self.m_org_clicked_on_item.m_population_cell.GetID:
        cell_info_item.setPen(QPen(QColor(0,255,0)))      

  def updateCellItems(self, should_update_all = False):
    if self.m_cell_info:

      self.m_avida and self.m_avida.m_avida_threaded_driver.m_lock.acquire()
      if self.m_change_list:
        for index in xrange(self.m_change_list.GetChangeCount()):
          self.updateCellItem(self.m_change_list[index])
        self.m_change_list.Reset()
      self.m_avida and self.m_avida.m_avida_threaded_driver.m_lock.release()

      if should_update_all:
        for cell_id in self.m_occupied_cells_ids:
          self.updateCellItem(cell_id)

      if self.m_canvas: self.m_canvas.update()
#jmc this is where you put the AllCellsPaintedSignal      

  def extractPopulationSlot(self, send_reset_signal = False, send_quit_signal = False):

    # If there is an active Avida object find all the cells that are occupied
    # and place them in a dictionary.  Fire off the signal for the next freezer
    # phase with that signal.

    population_dict = {}
    if self.m_avida != None:
      for x in range(self.m_world_w):
        for y in range(self.m_world_h):
          cell = self.m_avida.m_population.GetCell(x + self.m_world_w*y)
          if cell.IsOccupied() == True:
            organism = cell.GetOrganism()
            genome = organism.GetGenome()
            population_dict[cell.GetID()] = str(genome.AsString())
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("freezeDishPhaseIISig"), 
      (population_dict, send_reset_signal, send_quit_signal, ))

  def zoomSlot(self, zoom_factor):
    if self.m_canvas_view:
      m = QWMatrix()
      m.scale(zoom_factor/self.m_target_dish_scaling, zoom_factor/self.m_target_dish_scaling)
      trans_h = (self.m_canvas_view.size().height() - (self.m_map_cell_width * self.m_world_h)*
        (zoom_factor/self.m_target_dish_scaling))/2

      if zoom_factor == 0:
        m.translate(trans_h/(1/self.m_target_dish_scaling),trans_h/(1/self.m_target_dish_scaling))
      else:
        m.translate(trans_h/(zoom_factor/self.m_target_dish_scaling),trans_h/(zoom_factor/self.m_target_dish_scaling))    
      self.m_canvas_view.setWorldMatrix(m)
 
  class itemDrag(QTextDrag):
    def __init__(self, item_name, parent=None, name=None):
        QStoredDrag.__init__(self, 'item name (QString)', parent, name)
        self.setText(item_name)
