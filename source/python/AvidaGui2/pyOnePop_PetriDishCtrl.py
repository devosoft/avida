# -*- coding: utf-8 -*-

from qt import *
from pyMapProfile import pyMapProfile
from pyOnePop_PetriDishView import pyOnePop_PetriDishView


class pyOnePop_PetriDishCtrl(pyOnePop_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_PetriDishView.__init__(self,parent,name,fl)

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
    self.connect(self.m_petri_dish_toggle, SIGNAL("clicked()"), 
      self.ToggleDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.RenameDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.MakeConfigVisiableSlot)

    self.connect(self.m_zoom_spinbox, SIGNAL("valueChanged(int)"),
      self.m_petri_dish_ctrl.zoomSlot)
    self.connect(self.m_petri_dish_ctrl, PYSIGNAL("zoomSig"),
      self.m_zoom_spinbox.setValue)

    self.m_map_profile = pyMapProfile()
    self.m_mode_combobox.clear()
    self.m_mode_combobox.setInsertionPolicy(QComboBox.AtBottom)
    for i in range(self.m_map_profile.getSize()):
      self.m_mode_combobox.insertItem(self.m_map_profile.getModeName(i))
    self.connect(self.m_mode_combobox, SIGNAL("activated(int)"), self.modeActivatedSlot)
    self.m_mode_combobox.setCurrentItem(1)
    self.m_mode_index = self.m_mode_combobox.currentItem()
    self.modeActivatedSlot(self.m_mode_index)

    self.m_petri_dish_ctrl.emit(PYSIGNAL("zoomSig"), (9,))

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

    if self.m_avida:
      self.m_map_profile.getUpdater(self.m_mode_combobox.currentItem()).resetRange(self.m_avida.m_population)


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

  def modeActivatedSlot(self, index):
    #print "pyOnePop_PetriDishCtrl.modeActivatedSlot index", index
    if self.m_avida:
      self.m_avida.m_avida_threaded_driver.m_lock.acquire()
    self.m_mode_index = index
    updater = self.m_map_profile.getUpdater(self.m_mode_index)
    updater and updater.reset(True)
    #(min, max) = updater and updater.resetRange(self.m_avida and self.m_avida.m_population or None) or (0.0, 0.0)
    #self.m_gradient_scale_ctrl.setRange(min, max)
    #self.m_gradient_scale_ctrl.activate(True)
    #self.m_petri_dish_ctrl.setRange(min, max)
    self.m_petri_dish_ctrl.setIndexer(self.m_map_profile.getIndexer(self.m_mode_index))
    #self.m_petri_dish_ctrl.updateCellItems()
    #self.avidaUpdatedSlot()
    if self.m_avida:
      self.m_avida.m_avida_threaded_driver.m_lock.release()

  def avidaUpdatedSlot (self):
    updater = self.m_map_profile.getUpdater(self.m_mode_index)
    if updater:
      (old_min, old_max) = updater.getRange()
      (min, max) = self.m_avida and updater.updateRange(self.m_avida.m_population) or (0, 0)
      if updater.shouldReset() or ((old_min, old_max) != (min, max)):
        self.m_gradient_scale_ctrl.setRange(min, max)
        self.m_gradient_scale_ctrl.activate(True)
        self.m_petri_dish_ctrl.setRange(min, max)
        updater.reset(False)
    else:
      self.m_gradient_scale_ctrl.setRange(0, 0)
      self.m_gradient_scale_ctrl.activate(True)
      self.m_petri_dish_ctrl.setRange(0, 0)
    self.m_petri_dish_ctrl.updateCellItems()

    stats = update = None
    if self.m_avida: stats = self.m_avida.m_population.GetStats()
    if stats: update = stats.GetUpdate()
    if update: self.m_update_label.setText(QString("%1").arg(update))
    
  def RenameDishSlot(self, dishName):
    self.PopulationTextLabel.setText(dishName)
