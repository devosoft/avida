from pyEduSessionMenuBarHdlr import *
from pyEduWorkspaceCtrl import *
from pyAvidaCoreData import *
from pyMdtr import *
from pySessionControllerFactory import *
from pySessionDumbCtrl import *
from pySessionWorkThreadHdlr import *

from AvidaCore import cString

import qt

import os
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
    print 'pySessionCtrl.__del__() done.'

  def construct(self, main_mdl):
    print("""
    FIXME : pySessionCtrl
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    ...But I'd say it's okay for the moment, since we can't run more than one instance of Avida concurrently (yet)...
    """)

    # Create "model" for storing state data.
    class pyMdl: pass
    self.m_session_mdl = pyMdl()
    self.m_session_mdl.current_freezer = "freezer/"

    # Create a temporary subdirectory for general use in this session.
    self.m_session_mdl.m_tempdir = tempfile.mkdtemp('-pid%d'%os.getpid(),'AvidaEd-')

    # Create session mediator.
    self.m_session_mdl.m_session_mdtr = pyMdtr()

    # create session controller factory.
    self.m_session_controller_factory = pySessionControllerFactory()
    self.m_session_controller_factory.construct(self.m_session_mdl)

    # Connect various session controller creators to the controller
    # factory.
    self.m_session_controller_factory.addControllerCreator("pyEduSessionMenuBarHdlr", pyEduSessionMenuBarHdlr)
    self.m_session_controller_factory.addControllerCreator("pySessionDumbCtrl", pySessionDumbCtrl)
    self.m_session_controller_factory.addControllerCreator("pyEduWorkspaceCtrl", pyEduWorkspaceCtrl)

    self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduSessionMenuBarHdlr",))
    
    ## XXX Temporary. Cause instantiation of a dumb gui for testing. @kgn
    self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pySessionDumbCtrl",))

    self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduWorkspaceCtrl",))

    self.connect(self.m_session_mdl.m_session_mdtr, qt.PYSIGNAL("doOrphanSessionSig"), self.doOrphanSessionSlot)
    return self
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
    self.adoptUnitTestSuite("pySessionDumbCtrl")
    self.adoptUnitTestSuite("pySessionWorkThreadHdlr")

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

