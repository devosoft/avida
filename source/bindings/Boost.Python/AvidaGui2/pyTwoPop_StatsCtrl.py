# -*- coding: utf-8 -*-

from qt import *
from pyTwoPop_StatsView import pyTwoPop_StatsView


class pyTwoPop_StatsCtrl(pyTwoPop_StatsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyTwoPop_StatsView.__init__(self,parent,name,fl)
