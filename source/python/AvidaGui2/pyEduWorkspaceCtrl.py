# -*- coding: utf-8 -*-

from pyEduWorkspaceView import pyEduWorkspaceView
from pyMdtr import pyMdtr
from pyOneAnalyzeCtrl import pyOneAnalyzeCtrl
from pyOneOrganismCtrl import pyOneOrganismCtrl
from pyOnePopulationCtrl import pyOnePopulationCtrl
from pyTwoAnalyzeCtrl import pyTwoAnalyzeCtrl
from pyTwoOrganismCtrl import pyTwoOrganismCtrl
from pyTwoPopulationCtrl import pyTwoPopulationCtrl
from pyPetriConfigureCtrl import pyPetriConfigureCtrl

from qt import *

class pyEduWorkspaceCtrl(pyEduWorkspaceView):

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_mdl.m_session_mdtr.m_workspace_mdtr = pyMdtr()
    self.m_nav_bar_ctrl.construct(session_mdl)
    self.m_freezer_ctrl.construct(session_mdl)
    self.m_cli_to_ctrl_dict = {}
    self.m_ctrl_to_cli_dict = {}

    while self.m_widget_stack.visibleWidget():
      self.m_widget_stack.removeWidget(self.m_widget_stack.visibleWidget())

    self.m_one_population_ctrl  = pyOnePopulationCtrl(self.m_widget_stack,  "m_one_population_ctrl")
    self.m_two_population_ctrl  = pyTwoPopulationCtrl(self.m_widget_stack,  "m_two_population_ctrl")
    self.m_one_organism_ctrl    = pyOneOrganismCtrl(self.m_widget_stack,    "m_one_organism_ctrl")
    self.m_two_organism_ctrl    = pyTwoOrganismCtrl(self.m_widget_stack,    "m_two_organism_ctrl")
    self.m_one_analyze_ctrl     = pyOneAnalyzeCtrl(self.m_widget_stack,     "m_one_analyze_ctrl")
    self.m_two_analyze_ctrl     = pyTwoAnalyzeCtrl(self.m_widget_stack,     "m_two_analyze_ctrl")

    for (cli, ctrl) in (
      (self.m_nav_bar_ctrl.m_one_population_cli, self.m_one_population_ctrl),
      (self.m_nav_bar_ctrl.m_two_population_cli, self.m_two_population_ctrl),
      (self.m_nav_bar_ctrl.m_one_organism_cli,   self.m_one_organism_ctrl),
      (self.m_nav_bar_ctrl.m_two_organism_cli,   self.m_two_organism_ctrl),
      (self.m_nav_bar_ctrl.m_one_analyze_cli,    self.m_one_analyze_ctrl),
      (self.m_nav_bar_ctrl.m_two_analyze_cli,    self.m_two_analyze_ctrl),
    ):
      self.m_cli_to_ctrl_dict[cli] = ctrl
      self.m_ctrl_to_cli_dict[ctrl] = cli

    self.m_nav_bar_ctrl.m_one_population_cli.setState(QCheckListItem.On)
    self.m_nav_bar_ctrl.m_one_population_cli.setState(QCheckListItem.Off)

    #for ctrl in self.m_ctrl_to_cli_dict.keys():
    #  ctrl.construct(self.m_session_mdl)
    self.m_one_population_ctrl.construct(self.m_session_mdl)

    self.connect(self.m_nav_bar_ctrl.m_list_view, SIGNAL("clicked(QListViewItem *)"), self.navBarItemClickedSlot)
    self.connect(self.m_widget_stack, SIGNAL("aboutToShow(QWidget *)"), self.ctrlAboutToShowSlot)

    self.m_nav_bar_ctrl.m_one_population_cli.setState(QCheckListItem.On)
    self.m_widget_stack.raiseWidget(self.m_one_population_ctrl)

    self.show()

  def __del__(self):
    for key in self.m_cli_to_ctrl_dict.keys():
      del self.m_cli_to_ctrl_dict[key]
    for key in self.m_ctrl_to_cli_dict.keys():
      del self.m_ctrl_to_cli_dict[key]

  def navBarItemClickedSlot(self, item):
    if item:
      if self.m_cli_to_ctrl_dict.has_key(item):
        self.m_widget_stack.raiseWidget(self.m_cli_to_ctrl_dict[item])

  def ctrlAboutToShowSlot(self, ctrl):
    if ctrl:
      if self.m_ctrl_to_cli_dict.has_key(ctrl):
        for cli in self.m_cli_to_ctrl_dict.keys():
          cli.setState(QCheckListItem.Off)
        self.m_ctrl_to_cli_dict[ctrl].setState(QCheckListItem.On)

  def close(self, also_delete = False):
    # XXX trying to temporarily prevent closing this window.
    print "pyEduWorkspaceCtrl.close(): Cowardly refusing to close."
    return False

######### pyuic-one-time-generated-code lives below this line.

  def __del__(self):
    print "pyEduWorkspaceCtrl.__del__(): Not implemented yet"

  def __init__(self, parent = None, name = None, fl = 0):
    pyEduWorkspaceView.__init__(self,parent,name,fl)
    print "pyEduWorkspaceCtrl.__init__(): Not implemented yet"

  # public slot

  def fileNew(self):
    print "pyEduWorkspaceCtrl.fileNew(): Not implemented yet"

  # public slot

  def fileOpen(self):
    print "pyEduWorkspaceCtrl.fileOpen(): Not implemented yet"

  # public slot

  def fileSave(self):
    print "pyEduWorkspaceCtrl.fileSave(): Not implemented yet"

  # public slot

  def fileSaveAs(self):
    print "pyEduWorkspaceCtrl.fileSaveAs(): Not implemented yet"

  # public slot

  def filePrint(self):
    print "pyEduWorkspaceCtrl.filePrint(): Not implemented yet"

  # public slot

  def fileExit(self):
    print "pyEduWorkspaceCtrl.fileExit(): Not implemented yet"

  # public slot

  def editUndo(self):
    print "pyEduWorkspaceCtrl.editUndo(): Not implemented yet"

  # public slot

  def editRedo(self):
    print "pyEduWorkspaceCtrl.editRedo(): Not implemented yet"

  # public slot

  def editCut(self):
    print "pyEduWorkspaceCtrl.editCut(): Not implemented yet"

  # public slot

  def editCopy(self):
    print "pyEduWorkspaceCtrl.editCopy(): Not implemented yet"

  # public slot

  def editPaste(self):
    print "pyEduWorkspaceCtrl.editPaste(): Not implemented yet"

  # public slot

  def editFind(self):
    print "pyEduWorkspaceCtrl.editFind(): Not implemented yet"

  # public slot

  def helpIndex(self):
    print "pyEduWorkspaceCtrl.helpIndex(): Not implemented yet"

  # public slot

  def helpContents(self):
    print "pyEduWorkspaceCtrl.helpContents(): Not implemented yet"

  # public slot

  def helpAbout(self):
    print "pyEduWorkspaceCtrl.helpAbout(): Not implemented yet"

