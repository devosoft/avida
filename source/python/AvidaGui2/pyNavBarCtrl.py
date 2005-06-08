# -*- coding: utf-8 -*-

from qt import *
from pyNavBarView import *


class pyNavBarCtrl(pyNavBarView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyNavBarView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
#    def generateCLIGroup(list_view, group_name, first_name, second_name):
    def generateCLIGroup(list_view, group_name, first_name):
      group_lvi = QCheckListItem(list_view, group_name, QCheckListItem.RadioButtonController)
      group_lvi.setSelectable(False)
      group_lvi.setOpen(True)
      #
#      second_cli = QCheckListItem(group_lvi, second_name, QCheckListItem.RadioButton)
#      second_cli.setSelectable(False)
      first_cli = QCheckListItem(group_lvi, first_name, QCheckListItem.RadioButton)
      first_cli.setSelectable(False)
      #
#      return (group_lvi, first_cli, second_cli)
      return (group_lvi, first_cli)

    self.m_list_view.setSortColumn(-1)
    self.m_list_view.clear()

#    (self.m_analyze_lvi, self.m_one_analyze_cli, self.m_two_analyze_cli) = generateCLIGroup(
#      self.m_list_view, " Analysis", "Analyze One", "Compare Two")

    (self.m_analyze_lvi, self.m_one_analyze_cli) = generateCLIGroup(
      self.m_list_view, " Analysis", "Analyze One")

#    (self.m_organism_lvi, self.m_one_organism_cli, self.m_two_organism_cli) = generateCLIGroup(
#      self.m_list_view, " Organisms", "View One", "Compare Two")

    (self.m_organism_lvi, self.m_one_organism_cli) = generateCLIGroup(
      self.m_list_view, " Organisms", "View One")

#    (self.m_population_lvi, self.m_one_population_cli, self.m_two_population_cli) = generateCLIGroup(
#      self.m_list_view, " Populations", "View One", "Compare Two")

    (self.m_population_lvi, self.m_one_population_cli) = generateCLIGroup(
      self.m_list_view, " Populations", "View One")

    self.m_population_lvi.setPixmap(0, self.image0)
    self.m_organism_lvi.setPixmap(0, self.image1)
    self.m_analyze_lvi.setPixmap(0, self.image2)
