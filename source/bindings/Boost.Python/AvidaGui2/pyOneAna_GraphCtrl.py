# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_GraphView import pyOneAna_GraphView


class pyOneAna_GraphCtrl(pyOneAna_GraphView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_GraphView.__init__(self,parent,name,fl)
