# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_PetriDishView import pyOnePop_PetriDishView


class pyOnePop_PetriDishCtrl(pyOnePop_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_PetriDishView.__init__(self,parent,name,fl)
    self.connect(self.m_petri_dish_toggle, SIGNAL("clicked()"), 
      self.ToggleDishSlot)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.m_petri_dish_ctrl.construct(self.m_session_mdl)
    self.m_gradient_scale_ctrl.construct(self.m_session_mdl)
    self.m_live_controls_ctrl.construct(self.m_session_mdl)
    self.m_petri_configure_ctrl.construct(self.m_session_mdl)
    self.connect(self.m_petri_configure_ctrl, PYSIGNAL("freezeDishPhaseISig"), 
      self.m_petri_dish_ctrl.extractPopulationSlot)
    self.connect(self.m_petri_dish_ctrl, PYSIGNAL("freezeDishPhaseIISig"), 
      self.m_petri_configure_ctrl.FreezePetriSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.RenameDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.MakeConfigVisiableSlot)

  def setAvidaSlot(self, avida):
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)



  def ToggleDishSlot (self):
    current_page = self.m_petri_dish_widget_stack.visibleWidget()
    current_page_int = self.m_petri_dish_widget_stack.id(current_page)
    if (current_page_int == 0):
       self.m_petri_dish_widget_stack.raiseWidget(1)
    else:
       self.m_petri_dish_widget_stack.raiseWidget(0)
 
  def MakeConfigVisiableSlot (self):
    current_page = self.m_petri_dish_widget_stack.visibleWidget()
    current_page_int = self.m_petri_dish_widget_stack.id(current_page)
    if (current_page_int == 0):
       self.m_petri_dish_widget_stack.raiseWidget(1)

  def avidaUpdatedSlot (self):
    stats = self.m_avida.m_population.GetStats()
    update = stats.GetUpdate()
    self.m_update_label.setText(QString("%1").arg(update))
    
  def RenameDishSlot(self, dishName):
    self.PopulationTextLabel.setText(dishName)


