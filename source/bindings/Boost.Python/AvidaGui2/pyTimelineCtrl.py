# -*- coding: utf-8 -*-

from qt import *
from pyTimelineView import pyTimelineView


class pyTimelineCtrl(pyTimelineView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyTimelineView.__init__(self,parent,name,fl)
