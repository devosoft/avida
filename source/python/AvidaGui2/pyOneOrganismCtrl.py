# -*- coding: utf-8 -*-

from qt import *
from pyOneOrganismView import pyOneOrganismView


class pyOneOrganismCtrl(pyOneOrganismView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneOrganismView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_one_org_scope_ctrl.construct(self.m_session_mdl)
