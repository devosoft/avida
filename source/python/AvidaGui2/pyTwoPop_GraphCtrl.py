# -*- coding: utf-8 -*-

from qt import *
from pyTwoPop_GraphView import pyTwoPop_GraphView


class pyTwoPop_GraphCtrl(pyTwoPop_GraphView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyTwoPop_GraphView.__init__(self,parent,name,fl)
