# -*- coding: utf-8 -*-

from qt import *
from pyLiveControlsView import pyLiveControlsView


class pyLiveControlsCtrl(pyLiveControlsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyLiveControlsView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
      self.doPauseAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
      self.doStartAvidaSlot)
    self.connect(
      self.m_play_button, SIGNAL("clicked()"),
      self.clickedStartAvidaSlot)
    self.connect(
      self.m_pause_button, SIGNAL("clicked()"),
      self.clickedPauseAvidaSlot)

  def setAvidaSlot(self, avida):
    print "pyLiveControlsCtrl.setAvidaSlot() ..."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pyLiveControlsCtrl.setAvidaSlot() disconnecting old_avida ..."
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      print "pyLiveControlsCtrl.setAvidaSlot() connecting self.m_avida ..."
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)

  def avidaUpdatedSlot(self):
    pass
    
  def doPauseAvidaSlot(self):
    self.m_play_pause_widgetStack.raiseWidget(0)
    
  def doStartAvidaSlot(self):
    self.m_play_pause_widgetStack.raiseWidget(1)
    
  def clickedPauseAvidaSlot(self):
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("fromLiveCtrlPauseAvidaSig"), ())
    
  def clickedStartAvidaSlot(self):
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("fromLiveCtrlStartAvidaSig"), ())

  def destruct(self):
    print "*** called pyLiveControlsCtrl.py:destruct ***"
    self.m_avida = None
    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
      self.doPauseAvidaSlot)
    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
      self.doStartAvidaSlot)
    self.disconnect(
      self.m_play_button, SIGNAL("clicked()"),
      self.clickedStartAvidaSlot)
    self.disconnect(
      self.m_pause_button, SIGNAL("clicked()"),
      self.clickedPauseAvidaSlot)
    self.m_session_mdl = None

