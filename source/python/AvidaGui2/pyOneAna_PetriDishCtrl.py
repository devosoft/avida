# -*- coding: utf-8 -*-

from qt import *
from pyOneAna_PetriDishView import pyOneAna_PetriDishView


class pyOneAna_PetriDishCtrl(pyOneAna_PetriDishView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_PetriDishView.__init__(self,parent,name,fl)
    self.m_one_ana_pop_name.setText('A Relatively Boring Population')
