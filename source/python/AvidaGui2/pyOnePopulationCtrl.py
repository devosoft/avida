# -*- coding: utf-8 -*-

from qt import *
from pyOnePopulationView import pyOnePopulationView
import os

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
    self.connect( self, PYSIGNAL("petriDishDroppedInPopViewSig"), self.m_session_mdl.m_session_mdtr, PYSIGNAL("petriDishDroppedInPopViewSig"))

  def dropEvent( self, e ):
    string = QString()
    print "dropEvent"
    if ( QTextDrag.decode( e, string ) ) :
      if os.path.exists(str('default.workspace/freezer/' + str(string) + '.full/')) == False:
        print "that was not a valid path" 
      else: 
        self.emit(PYSIGNAL("petriDishDroppedInPopViewSig"), (e,))
        print "emitted"