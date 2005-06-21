# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_PetriDishView import pyOneAna_PetriDishView
import os

class pyOneAna_PetriDishCtrl(pyOneAna_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_PetriDishView.__init__(self,parent,name,fl)
    self.m_one_ana_pop_name.setText('Drag A Full Petri Dish Into This Window To Analyze It')

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl     
    self.connect( self.m_session_mdl.m_session_mdtr, PYSIGNAL("freezerItemDroppedInOneAnalyzeSig"),
      self.freezerItemDropped)  
    self.connect( self.m_session_mdl.m_session_mdtr, PYSIGNAL("freezerItemDoubleClickedOnInOneAnaSig"),
      self.freezerItemDoubleClickedOn)  


  def freezerItemDropped(self, e):

    # Try to decode to the data you understand...
    freezer_item_name = QString()
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      self.m_one_ana_pop_name.setText(os.path.splitext((os.path.split(str(freezer_item_name))[1]))[0])

  def freezerItemDoubleClickedOn(self, freezer_item_name):
    print freezer_item_name
    self.m_one_ana_pop_name.setText(os.path.split(os.path.splitext(os.path.split(freezer_item_name)[0])[0])[1])

