# -*- coding: utf-8 -*-

from qt import *
from pyOneAnalyzeView import pyOneAnalyzeView
import os

class pyOneAnalyzeCtrl(pyOneAnalyzeView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAnalyzeView.__init__(self,parent,name,fl)
    self.setAcceptDrops(1)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_one_ana_graph_ctrl.construct(self.m_session_mdl)
    self.m_one_ana_petri_ctrl.construct(self.m_session_mdl) 
    self.connect( self, PYSIGNAL("petriDishDroppedSig"), self.m_session_mdl.m_session_mdtr, PYSIGNAL("petriDishDroppedSig"))
    print "self" 
    print self

  def dropEvent( self, e ):
    string = QString()

    if ( QTextDrag.decode( e, string ) ) :
      if os.path.exists(str('default.workspace/freezer/' + str(string) + '.full/')) == False:
        print "that was not a valid path" 
      else: 
        self.emit(PYSIGNAL("petriDishDroppedSig"), (e,))


  def dragEnterEvent( self, e ):
#      # Check if you want the drag...
#        if (secret.canDecode( e ) or
#            QTextDrag.canDecode( e ) or
#            QImageDrag.canDecode( e ) or
#            QUriDrag.canDecode( e )):
    e.accept()