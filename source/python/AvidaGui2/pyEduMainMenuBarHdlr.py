import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import pySessionCtrl; reload(pySessionCtrl)
from pySessionCtrl import *

from AvidaCore import cString

import qt

class pyEduMainMenuBarHdlr(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)
  def __del__(self):
    for menu in [self.m_window_menu, self.m_view_menu, self.m_control_menu, self.m_edit_menu, self.m_file_menu]:
      if menu.parent(): menu.parent().removeChild(menu)
      del menu
    if self.m_menu_bar.parent(): self.m_menu_bar.parent().removeChild(self.m_menu_bar)
    del self.m_menu_bar

  def construct(self, main_mdl):
    self.m_main_mdl = main_mdl
    self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr = pyMdtr()
    self.m_menu_bar = None

    for widget in qt.QApplication.topLevelWidgets():
      if "EduMainMenuBar" == widget.name():
        self.m_menu_bar = widget
    if None == self.m_menu_bar:
      self.m_menu_bar = qt.QMenuBar(None, "EduMainMenuBar")
    self.m_menu_bar.clear()

    self.m_file_menu = qt.QPopupMenu(None, "EduMainMenuBar file_menu")
    self.m_new_workspace_fmi_id = self.m_file_menu.insertItem("New Workspace",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doNewWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_N)
    self.m_open_workspace_fmi_id = self.m_file_menu.insertItem("Open Workspace...",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doOpenWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_O)
    self.m_close_workspace_fmi_id = self.m_file_menu.insertItem("Close Workspace",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCloseWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_W)
    self.m_file_menu.insertSeparator()
    self.m_save_workspace_fmi_id = self.m_file_menu.insertItem("Save Workspace",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_S)
    self.m_save_workspace_as_fmi_id = self.m_file_menu.insertItem("Save Workspace As...",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceAsSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_S)
    self.m_file_menu.insertSeparator()
    self.m_import_fmi_id = self.m_file_menu.insertItem("Import...",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doImportSig"))
    self.m_export_fmi_id = self.m_file_menu.insertItem("Export...",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"))
    self.m_file_menu.insertSeparator()
    self.m_quit_fmi_id = self.m_file_menu.insertItem("Quit",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_Q)
    self.m_menu_bar.insertItem("File", self.m_file_menu)

    self.m_edit_menu = qt.QPopupMenu(None, "EduMainMenuBar edit_menu")
    self.m_undo_emi_id = self.m_edit_menu.insertItem("Undo",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doUndoSig"),
      qt.Qt.CTRL + qt.Qt.Key_Z)
    self.m_redo_emi_id = self.m_edit_menu.insertItem("Redo",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doRedoSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_Z)
    self.m_edit_menu.insertSeparator()
    self.m_cut_emi_id = self.m_edit_menu.insertItem("Cut",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCutSig"),
      qt.Qt.CTRL + qt.Qt.Key_X)
    self.m_copy_emi_id = self.m_edit_menu.insertItem("Copy",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCopySig"),
      qt.Qt.CTRL + qt.Qt.Key_C)
    self.m_paste_emi_id = self.m_edit_menu.insertItem("Paste",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doPasteSig"),
      qt.Qt.CTRL + qt.Qt.Key_V)
    self.m_edit_menu.insertSeparator()
    self.m_select_all_emi_id = self.m_edit_menu.insertItem("Select All",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSelectAllSig"),
      qt.Qt.CTRL + qt.Qt.Key_A)
    self.m_menu_bar.insertItem("Edit", self.m_edit_menu)

    self.m_control_menu = qt.QPopupMenu(None, "EduMainMenuBar control_menu")
    self.m_start_pause_cmi_id = self.m_control_menu.insertItem("Start",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doStartPauseSig"),
      qt.Qt.Key_Space)
    self.m_next_update_or_step_cmi_id = self.m_control_menu.insertItem("Next Update",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doNextUpdateOrStepSig"),
      qt.Qt.Key_Return)
    self.m_menu_bar.insertItem("Control", self.m_control_menu)

    self.m_view_menu = qt.QPopupMenu(None, "EduMainMenuBar view_menu")
    self.m_menu_bar.insertItem("View", self.m_view_menu)

    self.m_window_menu = qt.QPopupMenu(None, "EduMainMenuBar window_menu")
    self.m_zoom_window_wmi_id = self.m_window_menu.insertItem("Zoom Window",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doZoomWindowSig"))
    self.m_minimize_window_wmi_id = self.m_window_menu.insertItem("Minimize Window",
      self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doMinimizeWindowSig"))
    self.m_menu_bar.insertItem("Window", self.m_window_menu)

    self.m_file_menu.setItemEnabled(self.m_close_workspace_fmi_id, False)
    self.m_file_menu.setItemEnabled(self.m_save_workspace_fmi_id, False)
    self.m_file_menu.setItemEnabled(self.m_save_workspace_as_fmi_id, False)
    self.m_file_menu.setItemEnabled(self.m_import_fmi_id, False)
    self.m_file_menu.setItemEnabled(self.m_export_fmi_id, False)

    self.m_edit_menu.setItemEnabled(self.m_undo_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_redo_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_cut_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_copy_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_paste_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_select_all_emi_id, False)

    self.m_control_menu.setItemEnabled(self.m_start_pause_cmi_id, False)
    self.m_control_menu.setItemEnabled(self.m_next_update_or_step_cmi_id, False)

    self.m_window_menu.setItemEnabled(self.m_zoom_window_wmi_id, False)
    self.m_window_menu.setItemEnabled(self.m_minimize_window_wmi_id, False)

    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNewWorkspaceSig"), self.doNewWorkspaceSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSaveWorkspaceSig"), self.doSaveWorkspaceSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSaveWorkspaceAsSig"), self.doSaveWorkspaceAsSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doOpenWorkspaceSig"), self.doOpenWorkspaceSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCloseWorkspaceSig"), self.doCloseWorkspaceSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doImportSig"), self.doImportSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doExportSig"), self.doExportSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doQuitSig"), self.doQuitSlot)

    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doUndoSig"), self.doUndoSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doRedoSig"), self.doRedoSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCutSig"), self.doCutSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCopySig"), self.doCopySlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doPasteSig"), self.doPasteSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSelectAllSig"), self.doSelectAllSlot)

    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doStartPauseSig"), self.doStartPauseSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNextUpdateOrStepSig"), self.doNextUpdateOrStepSlot)

    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doZoomWindowSig"), self.doZoomWindowSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_edu_main_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doMinimizeWindowSig"), self.doMinimizeWindowSlot)

  def doNewWorkspaceSlot(self):
    print "pyEduMainMenuBarHdlr.doNewWorkspaceSlot()."
  def doSaveWorkspaceSlot(self):
    print "pyEduMainMenuBarHdlr.doSaveWorkspaceSlot()."
  def doSaveWorkspaceAsSlot(self):
    print "pyEduMainMenuBarHdlr.doSaveWorkspaceAsSlot()."
  def doOpenWorkspaceSlot(self):
    print "pyEduMainMenuBarHdlr.doOpenWorkspaceSlot()..."
    qt.QMessageBox.warning(
      None, "Watch out...", "We aren't yet checking genesis files for errors, so beware.", qt.QMessageBox.Ok, 0)
    genesis_filename = qt.QFileDialog.getOpenFileName(
      ".", "Avida Config FIles (*.avida)", None, "Temporary open file dialog", "Choose a Genesis File" )
    if genesis_filename.isEmpty():
      print "pyEduMainMenuBarHdlr.doOpenWorkspaceSlot() genesis_filename was empty."
    else:
      print "pyEduMainMenuBarHdlr.doOpenWorkspaceSlot() genesis_filename: '" + str(genesis_filename) + "'"
      self.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
        qt.PYSIGNAL("newMainControllerSig"), ("pySessionCtrl", cString(str(genesis_filename)), ))
      #session_ctrl = pySessionCtrl(self.main_mdl)
      #session_ctrl.m_session_mdl.m_genesis_filename = cString(str(genesis_filename))
      #session_ctrl.construct()
      #self.m_main_mdl.m_main_controllers_list.append(session_ctrl)
    print "pyEduMainMenuBarHdlr.doOpenWorkspaceSlot() done."
  def doCloseWorkspaceSlot(self):
    print "pyEduMainMenuBarHdlr.doCloseWorkspaceSlot()."
  def doImportSlot(self):
    print "pyEduMainMenuBarHdlr.doImportSlot()."
  def doExportSlot(self):
    print "pyEduMainMenuBarHdlr.doExportSlot()."
  def doQuitSlot(self):
    print "pyEduMainMenuBarHdlr.doQuitSlot()."

  def doUndoSlot(self):
    print "pyEduMainMenuBarHdlr.doUndoSlot()."
  def doRedoSlot(self):
    print "pyEduMainMenuBarHdlr.doRedoSlot()."
  def doCutSlot(self):
    print "pyEduMainMenuBarHdlr.doCutSlot()."
  def doCopySlot(self):
    print "pyEduMainMenuBarHdlr.doCopySlot()."
  def doPasteSlot(self):
    print "pyEduMainMenuBarHdlr.doPasteSlot()."
  def doSelectAllSlot(self):
    print "pyEduMainMenuBarHdlr.doSelectAllSlot()."

  def doStartPauseSlot(self):
    print "pyEduMainMenuBarHdlr.doStartPauseSlot()."
  def doNextUpdateOrStepSlot(self):
    print "pyEduMainMenuBarHdlr.doNextUpdateOrStepSlot()."

  def doZoomWindowSlot(self):
    print "pyEduMainMenuBarHdlr.doZoomWindowSlot()."
  def doMinimizeWindowSlot(self):
    print "pyEduMainMenuBarHdlr.doMinimizeWindowSlot()."


# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyEduMainMenuBarHdlr(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    print """
    FIXME: pyEduMainMenuBarHdlr
    pyEduMainMenuBarHdlr has memory leaks in the form of menus that are never deleted.
    Track them down in the pyEduMainMenuBarHdlr unit test suite, verifying deletes.
    """
    self.adoptUnitTestSuite("pyMdtr")
    self.adoptUnitTestSuite("pySessionCtrl")

    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)
    self.adoptUnitTestCase(crashDummy())
