from pyEduSessionMenuBarHdlr import *
from pyEduWorkspaceCtrl import *
from pyAvidaCoreData import *
from pyMdtr import *
from pySessionControllerFactory import *
# from pySessionDumbCtrl import *

from AvidaCore import cString

import qt

import os, os.path
import tempfile

class pySessionCtrl(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)

  def __del__(self):
    # Clean this session's temporary subdirectory.
    print 'pySessionCtrl.__del__() about to remove %s...' % self.m_session_mdl.m_tempdir
    for root, dirs, files in os.walk(self.m_session_mdl.m_tempdir, topdown=False):
      for name in files:
        os.remove(os.path.join(root, name))
      for name in dirs:
        os.rmdir(os.path.join(root, name))
    os.removedirs(self.m_session_mdl.m_tempdir)
    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    print "pySessionCtrl.__del__() sending setAvidaSig(None) ..."
#    
#    Why does the following command give error:
#
#   "'NoneType' object has no attribute 'm_avida_thread_mdtr'" in <bound 
#   method pySessionCtrl.__del__ of <AvidaGui2.pySessionCtrl.pySessionCtrl 
#   object at 0xd734d20>> ignored
#
#    self.m_session_mdl.m_session_mdtr.emit(
#      PYSIGNAL("setAvidaSig"),
#      (None,))

    print 'pySessionCtrl.__del__() done.'

  def construct(self, main_mdl):
    print("""
    FIXME : pySessionCtrl
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    ...But I'd say it's okay for the moment, since we can't run more than one instance of Avida concurrently (yet)...
    """)

    # Variables tracking the state of the session

    self.sessionInitialized = False
    self.m_should_update = False

    # Create "model" for storing state data.
    class pyMdl: pass
    self.m_session_mdl = pyMdl()
    self.m_session_mdl.saved_empty_dish = False
    self.m_session_mdl.saved_full_dish = False
    self.m_session_mdl.new_empty_dish = True
    self.m_session_mdl.new_full_dish = True
    self.m_session_mdl.m_current_workspace = "default.workspace"
    self.m_session_mdl.m_current_freezer = os.path.join(self.m_session_mdl.m_current_workspace, "freezer")

    # Create a temporary subdirectory for general use in this session. Add a 
    # subdirectory to that for output files -- these files will only get 
    # put into named directories when frozen

    self.m_session_mdl.m_tempdir = tempfile.mkdtemp('','AvidaEd-pid%d-'%os.getpid())
    self.m_session_mdl.m_tempdir_out = os.path.join(self.m_session_mdl.m_tempdir, "output")
    os.mkdir(self.m_session_mdl.m_tempdir_out)

    # Create session mediator.
    self.m_session_mdl.m_session_mdtr = pyMdtr()

    # create session controller factory.
    self.m_session_controller_factory = pySessionControllerFactory()
    self.m_session_controller_factory.construct(self.m_session_mdl)

    # Connect various session controller creators to the controller
    # factory.
    self.m_session_controller_factory.addControllerCreator("pyEduSessionMenuBarHdlr", pyEduSessionMenuBarHdlr)
    # self.m_session_controller_factory.addControllerCreator("pySessionDumbCtrl", pySessionDumbCtrl)
    self.m_session_controller_factory.addControllerCreator("pyEduWorkspaceCtrl", pyEduWorkspaceCtrl)

    self.m_session_mdl.m_session_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduSessionMenuBarHdlr",))
    
    ## XXX Temporary. Cause instantiation of a dumb gui for testing. @kgn
    # self.m_session_mdl.m_session_mdtr.emit(
    #   qt.PYSIGNAL("newSessionControllerSig"), ("pySessionDumbCtrl",))

    self.m_session_mdl.m_session_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduWorkspaceCtrl",))

    self.connect(self.m_session_mdl.m_session_mdtr, qt.PYSIGNAL("doOrphanSessionSig"), self.doOrphanSessionSlot)

    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)

    self.connect(
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doStartSig"),
      self.doStart)

    self.connect(
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doPauseSig"),
      self.doPause)

    self.connect(
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("fromLiveCtrlStartAvidaSig"),
      self.doStart)

    self.connect(
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("fromLiveCtrlPauseAvidaSig"),
      self.doPause)

    self.connect(
      self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("restartPopulationSig"),
      self.restartPopulationSlot)

    self.doPause()

    return self

  def restartPopulationSlot(self): 
    print "BDB restartPopulationSlot Called"
    self.sessionInitialized = False
    self.m_should_update = False

  def setAvidaSlot(self, avida):
    "print *** pySessionCtrl setAvidaSlot ***"
    if (avida == None):
      print "*** Avida = None"
    old_avida = self.m_avida
    self.m_avida = avida
    if old_avida and hasattr(old_avida, "m_avida_thread_mdtr"):
      print "pySessionCtrl.setAvidaSlot(): disconnecting old_avida ..."
      self.disconnect(
        old_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      self.disconnect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doStartAvidaSig"))
      self.disconnect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doPauseAvidaSig"))
      self.disconnect(
        self, PYSIGNAL("doUpdateAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      self.disconnect(
        self.m_session_mdl.m_session_mdtr,
        PYSIGNAL("fromLiveCtrlUpdateAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      print "pySessionCtrl.setAvidaSlot(): deleting old_avida ..."
      del old_avida
    if self.m_avida and hasattr(self.m_avida, "m_avida_thread_mdtr"):
      print "pySessionCtrl.setAvidaSlot(): connecting self.m_avida ..."
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      self.connect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doStartAvidaSig"))
      self.connect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
        self.m_avida.m_avida_thread_mdtr,  PYSIGNAL("doPauseAvidaSig"))
      self.connect(
        self, PYSIGNAL("doUpdateAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      self.connect(
        self.m_session_mdl.m_session_mdtr,
        PYSIGNAL("fromLiveCtrlUpdateAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))


  def doOrphanSessionSlot(self):
    print """
    FIXME : pySessionCtrl
    In doOrphanSessionSlot, do cleanup, i.e., if session not saved, ask user to verify session close.
    """
    ## XXX Temporary.
    print """
    FIXME : pySessionCtrl
    There's gotta be a better way for the session to close itself than this...
    """
    self.m_session_mdl.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
      qt.PYSIGNAL("deleteControllerSig"), (self,))

  def doStart(self):
    if self.sessionInitialized == False:
      self.m_session_mdl.m_session_mdtr.emit(
        PYSIGNAL("doInitializeAvidaPhaseISig"),
        (self.m_session_mdl.m_tempdir,))
      self.sessionInitialized = True
    self.m_should_update = True
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doStartAvidaSig"), ())

  def doPause(self):
    self.m_should_update = False
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doPauseAvidaSig"), ())

  def avidaUpdatedSlot(self):

    # When there is a new update assume that the session has an unsaved 
    # state and the dish is no longer new

    self.m_session_mdl.saved_full_dish = False
    self.m_session_mdl.new_full_dish = False

  def unitTest(self, recurse = False):
    return pyUnitTestSuiteRecurser("pySessionCtrl", globals(), recurse).construct().runTest().lastResult()

# Unit tests.

from py_test_utils import *
from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pySessionCtrl(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__

    self.adoptUnitTestSuite("pyAvidaCoreData")
    self.adoptUnitTestSuite("pyEduSessionMenuBarHdlr")
    self.adoptUnitTestSuite("pyMdtr")
    self.adoptUnitTestSuite("pySessionControllerFactory")
    # self.adoptUnitTestSuite("pySessionDumbCtrl")

    class deleteChecks(pyTestCase):
      def test(self):
        class pyMdl: pass
        mdl = pyMdl()
        session_ctrl_factory = lambda : pySessionCtrl().construct(mdl, cString("genesis.avida"))

        these_will_live_on = [
          # this is the mdl object created above.
          #'.m_session_mdl.m_main_mdl'
        ]

        print """
        FIXME: pySessionCtrl
        How do I clean-up the population driver without causing a crash.
        Because then all of the objects below will be properly deleted.
        """
        these_are_not_being_deleted_and_it_really_sucks = [
        #  ".m_session_mdl.m_avida_threaded_driver",
        #  ".m_session_mdl.m_avida_threaded_driver.m_thread",
        #  ".m_session_mdl.m_avida_threaded_driver.m_updated_semaphore",
        #  ".m_session_mdl.m_avida_threaded_driver.m_do_update_semaphore",
        #  ".m_session_mdl.m_avida_threaded_driver.m_environment",
        #  ".m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver",
        #  ".m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver.m_thread",
        #  ".m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver.m_do_update_semaphore",
        #  ".m_session_controller_factory.m_session_controllers_list[1].m_updated_semaphore",
        ]

        things_that_will_not_go_away = these_will_live_on + these_are_not_being_deleted_and_it_really_sucks

        endotests = recursiveDeleteChecks(session_ctrl_factory, things_that_will_not_go_away)

        for (endotest, attr_name) in endotests:
          try:
            endotest.verify()
            self.test_is_true(True)
          except AssertionError, err:
            pySessionCtrl_delete_checks = """
            Buried attribute either should have been deleted and wasn't,
            or shouldn't have and was :
            %s
            """ % attr_name
            self.test_is_true(False, pySessionCtrl_delete_checks)
        
    self.adoptUnitTestCase(deleteChecks())

