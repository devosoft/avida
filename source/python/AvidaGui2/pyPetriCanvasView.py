
from qt import Qt, PYSIGNAL
from qtcanvas import QCanvasView

class pyPetriCanvasView(QCanvasView):
  def __init__(self,parent,name,f =2):
    QCanvasView.__init__(self,parent,name,f)
  def contentsMousePressEvent(self,e): # QMouseEvent e
    if e.button() != Qt.LeftButton: return
    cell_id = None
    org_clicked_on_item = None
    point = self.inverseWorldMatrix().map(e.pos())
    if self.canvas():
      ilist = self.canvas().collisions(point) #QCanvasItemList ilist
      for item in ilist:
        if item.rtti()==271828:
          cell_id = item.m_population_cell.GetID()
          org_clicked_on__color = item.brush().color()
          org_clicked_on_item = item
      self.emit(PYSIGNAL("orgClickedOnSig"), (org_clicked_on_item,))
      if org_clicked_on_item:
        self.emit(PYSIGNAL("orgClickedOnSig"), (org_clicked_on_item,))
