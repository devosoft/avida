
from qt import Qt, PYSIGNAL
from qtcanvas import QCanvasView

class pyPetriCanvasView(QCanvasView):
  def __init__(self,parent,name,f):
    QCanvasView.__init__(self,parent,name,f)
  def contentsMousePressEvent(self,e): # QMouseEvent e
    if e.button() != Qt.LeftButton: return
    point = self.inverseWorldMatrix().map(e.pos())
    ilist = self.canvas().collisions(point) #QCanvasItemList ilist
    for item in ilist:
      if item.rtti()==271828:
        self.emit(PYSIGNAL("orgClickedOnSig"), (item.m_population_cell.GetID(),))

