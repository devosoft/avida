# -*- coding: utf-8 -*-

from qt import *
from pyLiveControlsView import pyLiveControlsView


class pyLiveControlsCtrl(pyLiveControlsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyLiveControlsView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
