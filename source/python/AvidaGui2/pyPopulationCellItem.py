
from qt import QBrush, QPen
from qtcanvas import QCanvasRectangle

class pyPopulationCellItem(QCanvasRectangle):
  def rtti(self):
    return 271828

  def __init__(self, population_cell, x, y, w, h, canvas):
    QCanvasRectangle.__init__(self, x, y, w, h, canvas)
    self.m_population_cell = population_cell
    self.m_index = 0
    self.show()

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
    self.setBrush(QBrush(color))
    self.setPen(QPen(color))

