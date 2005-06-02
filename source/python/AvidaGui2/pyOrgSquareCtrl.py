
print """
XXX fixme: in pyOrgSquareCtrl.py,
pyOrgSquareCtrl.setAvidaSlot(),
most of the code in this function should only be performed when
self.m_avida is not None.
@kgn
"""

from AvidaCore import cConfig

from qt import PYSIGNAL, QBrush, QColor, QLayout, QPen, QSize, Qt, QVBoxLayout, QWidget, QWMatrix
from qtcanvas import QCanvas, QCanvasRectangle, QCanvasView

class pySquareVBoxLayout(QVBoxLayout):
  def __init__(self, *args): apply(QVBoxLayout.__init__, (self,)+args)
  def hasHeightForWidth(self): return True
  def heightForWidth(self, w): return w

class pyOrgSquareCtrl(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_canvas = QCanvas(25,25)
    self.m_canvas.setBackgroundColor(Qt.darkGray)
    self.m_canvas_view = QCanvasView(self.m_canvas, self,"m_canvas_view")

  def paint(self, color):
    self.m_canvas.setBackgroundColor(color)
 