# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_PetriDishView import pyOnePop_PetriDishView


class pyOnePop_PetriDishCtrl(pyOnePop_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_PetriDishView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl

    self.m_petri_dish_ctrl.construct(self.m_session_mdl)
    self.m_gradient_scale_ctrl.construct(self.m_session_mdl)
    self.m_live_controls_ctrl.construct(self.m_session_mdl)
