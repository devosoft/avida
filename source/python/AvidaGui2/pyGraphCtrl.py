# -*- coding: utf-8 -*-

from qt import *
from qwt import *
from pyGraphView import pyGraphView

class pyGraphCtrl(QwtPlot):

  def __init__(self, *args):
    QwtPlot.__init__(self, *args)
  def construct(self, session_mdl):
    self.setCanvasBackground(Qt.white)
    self.m_zoomer = QwtPlotZoomer(
      QwtPlot.xBottom,
      QwtPlot.yLeft,
      QwtPicker.DragSelection,
      QwtPicker.AlwaysOff,
      self.canvas())
    self.m_zoomer.setZoomBase()
    self.m_zoomer.setRubberBandPen(QPen(Qt.black))
    # Configure zoomer for use with one-button mouse -- for OS X.
    # - Click and drag to create a zoom rectangle;
    # - Option-click to zoom-out to full view.
    self.m_zoomer.initMousePattern(1)
