# -*- coding: utf-8 -*-

from qt import *
from pyOnePopulationView import pyOnePopulationView


class pyOnePopulationCtrl(pyOnePopulationView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePopulationView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_one_pop_petri_dish_ctrl.construct(self.m_session_mdl)
    #self.m_one_pop_graph_ctrl.construct(self.m_session_mdl)
