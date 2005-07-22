# -*- coding: utf-8 -*-

from qt import *
from pyOneAnalyzeView import pyOneAnalyzeView
import os.path

class pyOneAnalyzeCtrl(pyOneAnalyzeView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAnalyzeView.__init__(self,parent,name,fl)
    self.setAcceptDrops(1)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_one_ana_graph_ctrl.construct(self.m_session_mdl)
    self.m_one_ana_petri_ctrl.construct(self.m_session_mdl) 
    self.connect( self, PYSIGNAL("freezerItemDroppedInOneAnalyzeSig"),
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezerItemDroppedInOneAnalyzeSig"))
    self.connect( self, PYSIGNAL("freezerItemDoubleClickedOnInOneAnaSig"),
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezerItemDoubleClickedOnInOneAnaSig"))
    self.connect( self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezerItemDoubleClicked"),
      self.freezerItemDoubleClicked)

  def dropEvent( self, e ):
    freezer_item_name = QString()
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      if os.path.exists( str(freezer_item_name)) == False:
        print "that was not a valid path(3)" 
      else: 
        self.emit(PYSIGNAL("freezerItemDroppedInOneAnalyzeSig"), (e,))

  def freezerItemDoubleClicked(self, freezer_item_name):
    if os.path.exists( str(freezer_item_name)) == False:
      print "that was not a valid path(3)"
    else:
      if self.isVisible():
        self.emit(PYSIGNAL("freezerItemDoubleClickedOnInOneAnaSig"), 
          (freezer_item_name,))
      


