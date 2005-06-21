
from qt import QBrush, QColor, QPen, Qt
from qtcanvas import QCanvasRectangle

class pyPopulationCellItem(QCanvasRectangle):

  s_empty_color = QColor(Qt.black)
  s_null_pen = QPen(Qt.NoPen)

  def rtti(self):
    return 271828

  def __init__(self, population_cell, x, y, w, h, canvas):
 #   x = x + 5
 #   y = y + 5
    QCanvasRectangle.__init__(self, x, y, w, h, canvas)
    self.m_population_cell = population_cell
    self.m_index = 0
    self.show()
    self.setZ(1.0 + population_cell.GetID())
    self.setPen(self.s_null_pen)

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
    color = (self.m_population_cell.IsOccupied() and functor) and functor(self.m_index) or self.s_empty_color
    self.setBrush(QBrush(color))

 

