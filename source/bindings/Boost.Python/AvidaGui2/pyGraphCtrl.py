# -*- coding: utf-8 -*-

from qt import *
from qwt import *
from pyGraphView import pyGraphView


class pyGraphCtrl(QwtPlot):

  def __init__(self, *args):
    QwtPlot.__init__(self, *args)
  def construct(self, session_mdl):
    pass
