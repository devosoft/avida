# -*- coding: utf-8 -*-

from pyOneOrg_ScopeView import pyOneOrg_ScopeView
import qt

class pyOneOrg_ScopeCtrl(pyOneOrg_ScopeView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneOrg_ScopeView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_organism_scope_ctrl.construct(self.m_session_mdl)
    self.m_execution_step_slider.setMinValue(0)
    self.m_execution_step_slider.setMaxValue(0)
    self.connect(
      self.m_execution_step_slider, qt.SIGNAL("valueChanged(int)"),
      self.m_organism_scope_ctrl.showFrame)
    self.connect(
      self.m_organism_scope_ctrl, qt.PYSIGNAL("gestationTimeChangedSig"),
      self.gestationTimeChangedSlot)
    self.connect(
      self.m_organism_scope_ctrl, qt.PYSIGNAL("executionStepResetSig"),
      self.executionStepResetSlot)

  def gestationTimeChangedSlot(self, gestation_time):
    print "pyOneOrg_ScopeCtrl.gestationTimeChangedSlot called, gestation_time ", gestation_time
    self.m_execution_step_slider.setMaxValue(gestation_time - 1)

  def executionStepResetSlot(self, execution_step):
    print "pyOneOrg_ScopeCtrl.executionStepResetSlot called, execution_step ", execution_step
    self.m_execution_step_slider.setValue(execution_step)
    # This may be redundant (I'm not sure). @kgn
    self.m_execution_step_slider.emit(qt.SIGNAL("valueChanged(int)"),(execution_step,))

