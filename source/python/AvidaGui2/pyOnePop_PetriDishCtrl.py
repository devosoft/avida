# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_PetriDishView import pyOnePop_PetriDishView


class pyOnePop_PetriDishCtrl(pyOnePop_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_PetriDishView.__init__(self,parent,name,fl)
    self.connect(self.m_petri_dish_toogle, SIGNAL("clicked()"), 
      self.ToogleDish)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl

    self.m_petri_dish_ctrl.construct(self.m_session_mdl)
    self.m_gradient_scale_ctrl.construct(self.m_session_mdl)
    self.m_live_controls_ctrl.construct(self.m_session_mdl)
    self.m_petri_configure_ctrl.construct(self.m_session_mdl)

  def ToogleDish (self):
    current_page = self.m_petri_dish_widget_stack.visibleWidget()
    current_page_int = self.m_petri_dish_widget_stack.id(current_page)
    if (current_page_int == 0):
       self.m_petri_dish_widget_stack.raiseWidget(1)
    else:
       self.m_petri_dish_widget_stack.raiseWidget(0)
