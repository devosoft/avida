import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import qt

print """
FIXME : pyEduSessionMenuBarHdlr
- move close-workspace code out of doCloseWorkspaceSlot.
- There's a great deal of repetetive code.
"""

class pyEduSessionMenuBarHdlr(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)
  def __del__(self):
    print("pyEduSessionMenuBarHdlr.__del__()...")
    ## painfully find and raise the top menu bar.
    #for widget in qt.QApplication.topLevelWidgets():
    #  widget.hide()
    #  widget.show()
    #  widget.raiseW()
    #  print "pyEduSessionMenuBarHdlr.__del__() checking widget name:", widget.name()
    #  if "EduMainMenuBar" == widget.name():
    #    print "pyEduSessionMenuBarHdlr.__del__() found existing menu bar:", widget
    #    self.m_menu_bar = widget
    #  else:
    #    print "pyEduSessionMenuBarHdlr.__del__() not a menu bar:", widget
    #if None != self.m_menu_bar:
    #  print "pyEduSessionMenuBarHdlr.__del__() using menu bar", self.m_menu_bar, "named", self.m_menu_bar.name()
    #  self.m_menu_bar.hide()
    #  self.m_menu_bar.show()
    #  self.m_menu_bar.raiseW()
    #else:
    #  print "pyEduSessionMenuBarHdlr.__del__() menu bar is null."
    ##qt.QApplication.desktop().raiseW()
    ##qt.QApplication.desktop().show()

    print("pyEduSessionMenuBarHdlr.__del__() done.")

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr = pyMdtr()

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSetupMainWindowMenuBarSig"), self.doSetupMainWindowMenuBarSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNewWorkspaceSig"), self.doNewWorkspaceSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSaveWorkspaceSig"), self.doSaveWorkspaceSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSaveWorkspaceAsSig"), self.doSaveWorkspaceAsSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doOpenWorkspaceSig"), self.doOpenWorkspaceSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCloseWorkspaceSig"), self.doCloseWorkspaceSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doImportSig"), self.doImportSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doExportSig"), self.doExportSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doQuitSig"), self.doQuitSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doUndoSig"), self.doUndoSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doRedoSig"), self.doRedoSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCutSig"), self.doCutSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doCopySig"), self.doCopySlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doPasteSig"), self.doPasteSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doSelectAllSig"), self.doSelectAllSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doStartSig"), self.doStartSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doPauseSig"), self.doPauseSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNextUpdateSig"), self.doNextUpdateSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNextStepSig"), self.doNextStepSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doWorkspaceWindowTmpSig"), self.doWorkspaceWindowTmpSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doZoomWindowSig"), self.doZoomWindowSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doMinimizeWindowSig"), self.doMinimizeWindowSlot)

  def doSetupMainWindowMenuBarSlot(self, main_window):
    main_window.m_menu_bar = main_window.menuBar()
    main_window.m_file_menu = qt.QPopupMenu()
    main_window.m_new_workspace_fmi_id = main_window.m_file_menu.insertItem("New Workspace",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doNewWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_N)
    main_window.m_open_workspace_fmi_id = main_window.m_file_menu.insertItem("Open Workspace...",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doOpenWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_O)
    main_window.m_close_workspace_fmi_id = main_window.m_file_menu.insertItem("Close Workspace",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCloseWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_W)
    main_window.m_file_menu.insertSeparator()
    main_window.m_save_workspace_fmi_id = main_window.m_file_menu.insertItem("Save Workspace",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_S)
    main_window.m_save_workspace_as_fmi_id = main_window.m_file_menu.insertItem("Save Workspace As...",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceAsSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_S)
    main_window.m_file_menu.insertSeparator()
    main_window.m_import_fmi_id = main_window.m_file_menu.insertItem("Import...",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doImportSig"))
    main_window.m_export_fmi_id = main_window.m_file_menu.insertItem("Export...",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"))
    main_window.m_file_menu.insertSeparator()
    main_window.m_quit_fmi_id = main_window.m_file_menu.insertItem("Quit",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSaveWorkspaceSig"),
      qt.Qt.CTRL + qt.Qt.Key_Q)
    main_window.m_menu_bar.insertItem("File", main_window.m_file_menu)

    main_window.m_edit_menu = qt.QPopupMenu()
    main_window.m_undo_emi_id = main_window.m_edit_menu.insertItem("Undo",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doUndoSig"),
      qt.Qt.CTRL + qt.Qt.Key_Z)
    main_window.m_redo_emi_id = main_window.m_edit_menu.insertItem("Redo",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doRedoSig"),
      qt.Qt.CTRL + qt.Qt.SHIFT + qt.Qt.Key_Z)
    main_window.m_edit_menu.insertSeparator()
    main_window.m_cut_emi_id = main_window.m_edit_menu.insertItem("Cut",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCutSig"),
      qt.Qt.CTRL + qt.Qt.Key_X)
    main_window.m_copy_emi_id = main_window.m_edit_menu.insertItem("Copy",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doCopySig"),
      qt.Qt.CTRL + qt.Qt.Key_C)
    main_window.m_paste_emi_id = main_window.m_edit_menu.insertItem("Paste",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doPasteSig"),
      qt.Qt.CTRL + qt.Qt.Key_V)
    main_window.m_edit_menu.insertSeparator()
    main_window.m_select_all_emi_id = main_window.m_edit_menu.insertItem("Select All",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doSelectAllSig"),
      qt.Qt.CTRL + qt.Qt.Key_A)
    main_window.m_menu_bar.insertItem("Edit", main_window.m_edit_menu)

    main_window.m_control_menu = qt.QPopupMenu()
    main_window.m_start_cmi_id = main_window.m_control_menu.insertItem("Start",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doStartSig"),
      qt.Qt.Key_Space)
    main_window.m_pause_cmi_id = main_window.m_control_menu.insertItem("Pause",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doPauseSig"),
      qt.Qt.Key_Space)
    main_window.m_next_update_cmi_id = main_window.m_control_menu.insertItem("Next Update",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doNextUpdateSig"),
      qt.Qt.Key_Return)
    main_window.m_next_step_cmi_id = main_window.m_control_menu.insertItem("Next Step",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doNextStepSig"),
      qt.Qt.Key_Return)
    main_window.m_menu_bar.insertItem("Control", main_window.m_control_menu)

    main_window.m_view_menu = qt.QPopupMenu()
    main_window.m_tmp_view_window_vmi_id = main_window.m_view_menu.insertItem("Workspace Window (tmp)",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doWorkspaceWindowTmpSig"))
    main_window.m_menu_bar.insertItem("View", main_window.m_view_menu)

    main_window.m_window_menu = qt.QPopupMenu()
    main_window.m_zoom_window_wmi_id = main_window.m_window_menu.insertItem("Zoom Window",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doZoomWindowSig"))
    main_window.m_minimize_window_wmi_id = main_window.m_window_menu.insertItem("Minimize Window",
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr, qt.PYSIGNAL("doMinimizeWindowSig"))
    main_window.m_menu_bar.insertItem("Window", main_window.m_window_menu)

  def doNewWorkspaceSlot(self):
    print "pyEduSessionMenuBarHdlr.doNewWorkspaceSlot()."
  def doSaveWorkspaceSlot(self):
    print "pyEduSessionMenuBarHdlr.doSaveWorkspaceSlot()."
  def doSaveWorkspaceAsSlot(self):
    print "pyEduSessionMenuBarHdlr.doSaveWorkspaceAsSlot()."
  def doOpenWorkspaceSlot(self):
    print "pyEduSessionMenuBarHdlr.doOpenWorkspaceSlot()."
  def doCloseWorkspaceSlot(self):
    print "pyEduSessionMenuBarHdlr.doCloseWorkspaceSlot()."
    self.m_session_mdl.m_session_mdtr.emit(qt.PYSIGNAL("doOrphanSessionSig"), ())
  def doImportSlot(self):
    print "pyEduSessionMenuBarHdlr.doImportSlot()."
  def doExportSlot(self):
    print "pyEduSessionMenuBarHdlr.doExportSlot()."
  def doQuitSlot(self):
    print "pyEduSessionMenuBarHdlr.doQuitSlot()."


  def doUndoSlot(self):
    print "pyEduSessionMenuBarHdlr.doUndoSlot()."
  def doRedoSlot(self):
    print "pyEduSessionMenuBarHdlr.doRedoSlot()."
  def doCutSlot(self):
    print "pyEduSessionMenuBarHdlr.doCutSlot()."
  def doCopySlot(self):
    print "pyEduSessionMenuBarHdlr.doCopySlot()."
  def doPasteSlot(self):
    print "pyEduSessionMenuBarHdlr.doPasteSlot()."
  def doSelectAllSlot(self):
    print "pyEduSessionMenuBarHdlr.doSelectAllSlot()."


  def doStartSlot(self):
    print "pyEduSessionMenuBarHdlr.doStartSlot()."
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doStartSig"), ())
  def doPauseSlot(self):
    print "pyEduSessionMenuBarHdlr.doPauseSlot()."
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doPauseSig"), ())
  def doNextUpdateSlot(self):
    print "pyEduSessionMenuBarHdlr.doNextUpdateSlot()."
  def doNextStepSlot(self):
    print "pyEduSessionMenuBarHdlr.doNextStepSlot()."

  def doWorkspaceWindowTmpSlot(self):
    print "pyEduSessionMenuBarHdlr.doWorkspaceWindowTmpSlot()..."
    self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduWorkspaceCtrl",))
    print "pyEduSessionMenuBarHdlr.doWorkspaceWindowTmpSlot() done."


  def doZoomWindowSlot(self):
    print "pyEduSessionMenuBarHdlr.doZoomWindowSlot()."
  def doMinimizeWindowSlot(self):
    print "pyEduSessionMenuBarHdlr.doMinimizeWindowSlot()."


# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyEduSessionMenuBarHdlr(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    testing %s
    """ % self.__class__.__name__
    print """
    FIXME: pyEduSessionMenuBarHdlr
    - pyEduSessionMenuBarHdlr has no unit tests.
    - pyEduSessionMenuBarHdlr has memory leaks in the form of menus that are never deleted.  Track them down in the
      pyEduSessionMenuBarHdlr unit test suite, verifying deletes.
    """
    self.adoptUnitTestSuite("pyMdtr")

    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)
    self.adoptUnitTestCase(crashDummy())
