# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_StatsView import pyOnePop_StatsView


class pyOnePop_StatsCtrl(pyOnePop_StatsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_StatsView.__init__(self,parent,name,fl)
