#from pyAvidaCoreData import pyAvidaCoreData
#from pyAvidaThreadedDriver import pyAvidaThreadedDriver
from pyEduSessionMenuBarHdlr import *
from pyEduWorkspaceCtrl import *
from pyAvidaCoreData import *
from pyMdtr import *
from pySessionControllerFactory import *
from pySessionDumbCtrl import *
from pySessionWorkThreadHdlr import *
from AvidaCore import cString
import qt

class pySessionCtrl(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)
  def construct(self, main_mdl, genesis_filename):
    print("""
    FIXME : pySessionCtrl
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    ...But I'd say it's okay for the moment, since we can't run more than one instance of Avida concurrently (yet)...
    """)

    # create "model" for storing state data
    class pyMdl: pass
    self.m_session_mdl = pyMdl()
    self.m_session_mdl.m_genesis_filename = genesis_filename
    #self.m_session_mdl.m_main_mdl = main_mdl

    # create session mediator
    self.m_session_mdl.m_session_mdtr = pyMdtr()

    # create session controller factory
    self.m_session_controller_factory = pySessionControllerFactory()
    self.m_session_controller_factory.construct(self.m_session_mdl)

    # create an avida processing thread
    ## XXX excising obsolete code. @kgn
    # self.m_session_mdl.m_avida_core_data = pyAvidaCoreData()
    # self.m_session_mdl.m_avida_core_data.construct(self.m_session_mdl.m_genesis_filename)

    # connect various session controller creators to the controller
    # factory.
    self.m_session_controller_factory.addControllerCreator("pyEduSessionMenuBarHdlr", pyEduSessionMenuBarHdlr)
    ## XXX excising obsolete code. @kgn
    # self.m_session_controller_factory.addControllerCreator("pySessionWorkThreadHdlr", pySessionWorkThreadHdlr)
    self.m_session_controller_factory.addControllerCreator("pySessionDumbCtrl", pySessionDumbCtrl)
    self.m_session_controller_factory.addControllerCreator("pyEduWorkspaceCtrl", pyEduWorkspaceCtrl)

    self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newSessionControllerSig"), ("pyEduSessionMenuBarHdlr",))
    ## XXX this was temporary code, now kept around for reference. @kgn
    # self.m_session_mdl.m_session_mdtr.m_session_controller_factory_mdtr.emit(
    #   qt.PYSIGNAL("newSessionControllerSig"), ("pySessionWorkThreadHdlr",))
    
    ## XXX temporary. cause instantiation of a dumb gui for testing. @kgn
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
    ## XXX temporary.
    print """
    FIXME : pySessionCtrl
    There's gotta be a better way for the session to close itself than this...
    """
    self.m_session_mdl.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
      qt.PYSIGNAL("deleteControllerSig"), (self,))
  def unitTest(self, recurse = False):
    return pyUnitTestSuiteRecurser("pySessionCtrl", globals(), recurse).construct().runTest().lastResult()
  #def __del__(self):
  #  print("""
  #  FIXME : pySessionCtrl
  #  __del__() doesn't clean-up correctly (halt the processing thread,
  #  then delete it before exiting), probably because I'm locking
  #  per-thread rather than on the Avida library.

  #  I'm using the wrong locking model in the driver threads...
  #  I need to lock on access to the Avida core library, rather than on
  #  per-thread locks (in order to protect static data in the library).
  #  """)
  #  # Explicit deletion, rather than reliance on refcounting, permits
  #  # the thread to delete itself in a thread-safe way.
  #  #self.m_session_mdl.m_session_controllers_list.reverse()
  #  #self.avida_threaded_driver = self.session_mdl.avida_threaded_driver
  #  #for controller in self.session_mdl.session_controllers_list:
  #  #  print "pySessionCtrl.destruct() deleting controller:", controller
  #  #  controller.destruct()
  #  #  del controller

  #  #while 0 < len(self.m_session_mdl.m_session_controllers_list):
  #  #  print "pySessionCtrl.destruct() again deleting controller:", self.m_session_mdl.m_session_controllers_list[0]
  #  #  del self.m_session_mdl.m_session_controllers_list[0]

  #  print """
  #  FIXME: pySessionCtrl
  #  why do I need to check for presence of attribute self.m_session_mdl.m_avida_threaded_driver?
  #  """
  #  if hasattr(self.m_session_mdl, "m_avida_threaded_driver"):
  #    del self.m_session_mdl.m_avida_threaded_driver
  #  else:
  #    print("pySessionCtrl.destruct() self.session_mdl.avida_threaded_driver missing.")

  #  del self.m_session_mdl.m_avida_core_data
  #  del self.m_session_controller_factory
  #  del self.m_session_mdl.m_session_mdtr
  #  del self.m_session_mdl.m_main_mdl

  #  print """
  #  FIXME: pySessionCtrl
  #  why do I need to check for presence of attribute self.m_session_mdl?
  #  """
  #  if hasattr(self, "m_session_mdl"):
  #    del self.m_session_mdl


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

