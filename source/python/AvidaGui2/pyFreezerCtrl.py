# -*- coding: utf-8 -*-

from qt import *
from pyFreezerView import pyFreezerView


class pyFreezerCtrl(pyFreezerView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyFreezerView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    pass
