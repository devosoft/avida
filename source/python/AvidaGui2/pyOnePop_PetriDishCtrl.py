# -*- coding: utf-8 -*-

from qt import *
from pyMapProfile import pyMapProfile
from pyOnePop_PetriDishView import pyOnePop_PetriDishView
import os
from pyReadFreezer import pyReadFreezer

class pyOnePop_PetriDishCtrl(pyOnePop_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_PetriDishView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.dishDisabled = False
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_petri_dish_ctrl.construct ***"
    self.m_petri_dish_ctrl.construct(self.m_session_mdl)
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_gradient_scale_ctrl.construct ***"
    self.m_gradient_scale_ctrl.construct(self.m_session_mdl)
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_live_controls_ctrl.construct ***"
    self.m_live_controls_ctrl.construct(self.m_session_mdl)
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_petri_configure_ctrl.construct ***"
    self.m_petri_configure_ctrl.construct(self.m_session_mdl)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("freezeDishPhaseISig"),
      self.m_petri_dish_ctrl.extractPopulationSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezeDishPhaseISig"), self.freezeDishPhaseISlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
    self.connect(self.m_petri_dish_toggle, SIGNAL("clicked()"), 
      self.ToggleDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doDefrostDishSig"), self.shouldIDefrost)
#    self.connect(self.m_session_mdl.m_session_mdtr, 
#       PYSIGNAL("doDefrostDishSig"), self.RenameDishSlot)
#    self.connect(self.m_session_mdl.m_session_mdtr, 
#       PYSIGNAL("doDefrostDishSig"), self.MakeConfigVisiableSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
       PYSIGNAL("doDisablePetriDishSig"), self.SetDishDisabledSlot)
    self.connect(self.m_zoom_spinbox, SIGNAL("valueChanged(int)"), 
       self.m_petri_dish_ctrl.zoomSlot)
    self.connect(self.m_petri_dish_ctrl, PYSIGNAL("zoomSig"), 
       self.m_zoom_spinbox.setValue)
    self.connect(self.m_mode_combobox, SIGNAL("activated(int)"), 
       self.modeActivatedSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("petriDishDroppedInPopViewSig"), self.petriDropped)  

    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_mode_combobox.clear ***"
    self.m_mode_combobox.clear()
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call m_mode_combobox.setInsertionPolicy ***"
    self.m_mode_combobox.setInsertionPolicy(QComboBox.AtBottom)
    print "*** pyOnePop_PetriDishCtrl.py:consruct about to call pyMapProfile ***"
    self.m_map_profile = pyMapProfile(self.m_session_mdl)
    for i in range(self.m_map_profile.getSize()):
      self.m_mode_combobox.insertItem(self.m_map_profile.getModeName(i))

    # Start with second map mode -- "Fitness".
    self.m_mode_combobox.setCurrentItem(2)
    self.m_mode_index = self.m_mode_combobox.currentItem()
    self.modeActivatedSlot(self.m_mode_index)
    # self.m_petri_dish_ctrl.emit(PYSIGNAL("zoomSig"), 
    #   (self.m_petri_dish_ctrl.m_initial_target_zoom,))

  def setAvidaSlot(self, avida):
    print "pyOnePop_PetriDishCtrl.setAvidaSlot() ..."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pyOnePop_PetriDishCtrl.setAvidaSlot() disconnecting old_avida ..."
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      print "pyOnePop_PetriDishCtrl.setAvidaSlot() connecting self.m_avida ..."
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
    if self.dishDisabled:
      return
    current_page = self.m_petri_dish_widget_stack.visibleWidget()
    current_page_int = self.m_petri_dish_widget_stack.id(current_page)
    if (current_page_int != 1):
       self.m_petri_dish_widget_stack.raiseWidget(1)
       
  def SetDishDisabledSlot(self):
    self.dishDisabled = True

  def modeActivatedSlot(self, index):
    self.m_avida and self.m_avida.m_avida_threaded_driver.m_lock.acquire()

    self.m_mode_index = index
    self.m_petri_dish_ctrl.setIndexer(self.m_map_profile.getIndexer(self.m_mode_index))
    self.m_petri_dish_ctrl.setColorLookupFunctor(self.m_map_profile.getColorLookup(self.m_mode_index))
    self.m_petri_dish_ctrl.m_should_update_all = True
    self.m_gradient_scale_ctrl.setColorLookup(self.m_map_profile.getColorLookup(self.m_mode_index))
    self.m_updater = self.m_map_profile.getUpdater(self.m_mode_index)
    self.m_updater and self.m_updater.reset(True)

    self.m_avida and self.m_avida.m_avida_threaded_driver.m_lock.release()
    self.avidaUpdatedSlot(True)

  def avidaUpdatedSlot(self, should_update_all = False):
    if self.m_updater:
      (old_min, old_max) = self.m_updater.getRange()
      (min, max) = self.m_avida and self.m_updater.updateRange(self.m_avida.m_population) or (0, 0)
      if self.m_updater.shouldReset() or ((old_min, old_max) != (min, max)):
        self.m_gradient_scale_ctrl.setRange(min, max)
        self.m_gradient_scale_ctrl.activate(True)
        self.m_petri_dish_ctrl.setRange(min, max)
        self.m_updater.reset(False)
        should_update_all = True
    else:
      self.m_gradient_scale_ctrl.setRange(0, 0)
      self.m_gradient_scale_ctrl.activate(True)
      self.m_petri_dish_ctrl.setRange(0, 0)
    self.m_petri_dish_ctrl.updateCellItems(should_update_all)

    stats = update = None
    if self.m_avida: stats = self.m_avida.m_population.GetStats()
    if stats: update = stats.GetUpdate()
    if update: self.m_update_label.setText(QString("%1").arg(update))
    
  def RenameDishSlot(self, dishName):
    if self.dishDisabled:
      return
    self.PopulationTextLabel.setText(dishName)
    
  # Dummy routine
    
  def freezeDishPhaseISlot(self, send_reset_signal = False, send_quit_signal = False):
    print "in pyOnePop_PetriDishCtrl recieved freezeDishPhaseISig"
    print "send_reset_signal = " + str(send_reset_signal)
    print "send_quit_signal = " + str(send_quit_signal)

  def petriDropped(self, e):
    # Try to decode to the data you understand...
    print "*** Entered petriDropped"
    freezer_item_name = QString()
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      if freezer_item_name[-8:] == 'organism':
        print "we can't yet deal with organims in the population view"
        return
      elif freezer_item_name[-4:] == 'full':
        freezer_item_name_temp = os.path.join(str(freezer_item_name), 'petri_dish')
        self.m_session_mdl.saved_full_dish = True
      else:
        freezer_item_name_temp = str(freezer_item_name)
        self.m_session_mdl.saved_empty_dish = True
      thawed_item = pyReadFreezer(freezer_item_name_temp)
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doDefrostDishSig"),  
        (os.path.splitext((os.path.split(str(freezer_item_name))[1]))[0], thawed_item,))

      current_page = self.m_petri_dish_widget_stack.visibleWidget()
      current_page_int = self.m_petri_dish_widget_stack.id(current_page)
      self.MakeConfigVisiableSlot()

  def shouldIDefrost(self, dishName):
    if self.isVisible():
      self.RenameDishSlot(dishName)
      self.MakeConfigVisiableSlot()

  def restart(self, session_mdl):
    print "pyOnePop_PetriDishCtrl.py:restart called"
    self.dishDisabled = False

    self.m_avida.destruct()
    self.m_petri_dish_ctrl.destruct()
    self.m_gradient_scale_ctrl.destruct()
    self.m_live_controls_ctrl.restart()
    self.m_petri_configure_ctrl.destruct()
    self.disconnect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("freezeDishPhaseISig"),
      self.m_petri_dish_ctrl.extractPopulationSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezeDishPhaseISig"), self.freezeDishPhaseISlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
    self.disconnect(self.m_petri_dish_toggle, SIGNAL("clicked()"), 
      self.ToggleDishSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doDefrostDishSig"), self.shouldIDefrost)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
       PYSIGNAL("doDisablePetriDishSig"), self.SetDishDisabledSlot)
    self.disconnect(self.m_zoom_spinbox, SIGNAL("valueChanged(int)"), 
       self.m_petri_dish_ctrl.zoomSlot)
    self.disconnect(self.m_petri_dish_ctrl, PYSIGNAL("zoomSig"), 
       self.m_zoom_spinbox.setValue)
    self.disconnect(self.m_mode_combobox, SIGNAL("activated(int)"), 
       self.modeActivatedSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("petriDishDroppedInPopViewSig"), self.petriDropped)  
    self.m_mode_index = None
    self.m_session_mdl = None
    self.m_avida = None
    self.m_update_label.setText(" ")
    print "*** pyOnePop_PetriDishCtrl.py:restartPopulationSlot about to call se.f.construct ***"
    self.construct(session_mdl)
