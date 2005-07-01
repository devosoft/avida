# -*- coding: utf-8 -*-

from qt import *
from pyOnePopulationView import pyOnePopulationView
import os.path

class pyOnePopulationCtrl(pyOnePopulationView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePopulationView.__init__(self,parent,name,fl)
    self.setAcceptDrops(1)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_one_pop_petri_dish_ctrl.construct(self.m_session_mdl)
    self.m_one_pop_graph_ctrl.construct(self.m_session_mdl)
    self.m_one_pop_stats_ctrl.construct(self.m_session_mdl)
    self.m_one_pop_timeline_ctrl.hide()
    self.connect( self, PYSIGNAL("petriDishDroppedInPopViewSig"),
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("petriDishDroppedInPopViewSig"))   
    self.connect( self, PYSIGNAL("freezerItemDoubleClickedOnInOnePopSig"), 
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezerItemDoubleClickedOnInOnePopSig"))
    self.connect( self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezerItemDoubleClicked"), self.freezerItemDoubleClicked)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("restartPopulationSig"), self.restartPopulationSlot)


  def dropEvent( self, e ):
    freezer_item_name = QString()
    print "dropEvent in one population"
    if ( QTextDrag.decode( e, freezer_item_name ) ) : #freezer_item_name is a string...the file name 
      if os.path.exists(str(freezer_item_name)) == False:
        print "that was not a valid path (1)" 
      else: 
        self.emit(PYSIGNAL("petriDishDroppedInPopViewSig"), (e,))

  def freezerItemDoubleClicked(self, freezer_item_name):
   if self.isVisible():
     self.emit(PYSIGNAL("freezerItemDoubleClickedOnInOnePopSig"), 
       (freezer_item_name,))

  def restartPopulationSlot(self, session_mdl):
    print "pyOnePopulationCtrl.py:restartPopulationSlot called"
    self.m_one_pop_petri_dish_ctrl.restart(self.m_session_mdl)
    self.m_one_pop_graph_ctrl.restart()
    self.m_one_pop_stats_ctrl.restart()
    session_mdl.m_session_mdtr.emit(PYSIGNAL("doStartAvidaSig"), ())



