# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_StatsView import pyOneAna_StatsView


class pyOneAna_StatsCtrl(pyOneAna_StatsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_StatsView.__init__(self,parent,name,fl)
