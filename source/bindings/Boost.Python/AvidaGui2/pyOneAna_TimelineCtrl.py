# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_TimelineView import pyOneAna_TimelineView


class pyOneAna_TimelineCtrl(pyOneAna_TimelineView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_TimelineView.__init__(self,parent,name,fl)
