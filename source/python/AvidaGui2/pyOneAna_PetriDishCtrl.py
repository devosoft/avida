# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_PetriDishView import pyOneAna_PetriDishView


class pyOneAna_PetriDishCtrl(pyOneAna_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_PetriDishView.__init__(self,parent,name,fl)
    self.m_one_ana_pop_name.setText('Drag A Full Petri Dish Into This Window To Analyze It')

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl     
    self.connect( self.m_session_mdl.m_session_mdtr, PYSIGNAL("petriDishDroppedSig"),
      self.petriDropped)  


  def petriDropped(self, e):

    # Try to decode to the data you understand...
    string = QString()
    if ( QTextDrag.decode( e, string ) ) :
      self.m_one_ana_pop_name.setText(string)
