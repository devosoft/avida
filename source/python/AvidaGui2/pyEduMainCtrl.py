"""
Main controller for AvidaEdu.
"""

import pyEduMainMenuBarHdlr; reload(pyEduMainMenuBarHdlr)
from pyEduMainMenuBarHdlr import *

import pyMainControllerFactory; reload(pyMainControllerFactory)
from pyMainControllerFactory import *

import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import pySessionCtrl; reload(pySessionCtrl)
from pySessionCtrl import *

import qt

class pyEduMainCtrl(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)
    
  def construct(self):
    class pyMdl: pass
    self.m_main_mdl = pyMdl()
    self.m_main_mdl.m_main_mdtr = pyMdtr()
    self.m_main_controller_factory = pyMainControllerFactory()
    self.m_main_controller_factory.construct(self.m_main_mdl)
    self.m_main_controller_factory.addControllerCreator(
      "pyEduMainMenuBarHdlr", pyEduMainMenuBarHdlr)
    #self.m_main_controller_factory.addControllerCreator(
    #  "pyEduMainConsoleCtrl", pyEduMainConsoleCtrlCreator())
    #self.m_main_controller_factory.addControllerCreator(
    #  "pyEduMainPrefsCtrl", pyEduMainPrefsCtrlCreator())
    #self.m_main_controller_factory.addControllerCreator(
    #  "pyMainQuitHdlr", pyMainQuitHdlrCreator())
    self.m_main_controller_factory.addControllerCreator(
      "pySessionCtrl", pySessionCtrl)

    self.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newMainControllerSig"), ("pyEduMainMenuBarHdlr",))
    return self
    
  def unitTest(self, recurse = False):
    return pyUnitTestSuiteRecurser(
      "pyEduMainCtrl",
      globals(),
      recurse
    ).construct().runTest().lastResult()


# Unit tests.

from py_test_utils import *
from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyEduMainCtrl(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__

    self.adoptUnitTestSuite("pyEduMainMenuBarHdlr")
    self.adoptUnitTestSuite("pyMainControllerFactory")
    self.adoptUnitTestSuite("pyMdtr")

    # Making sure that various objects are deleted when pyEduMainCtrl
    # object is deleted.
    class deleteChecks(pyTestCase):
      def test(self):
        def instantiateSessionCtrl():
          edu_main_ctrl = pyEduMainCtrl()
          edu_main_ctrl.construct()
          edu_main_ctrl.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
            qt.PYSIGNAL("newMainControllerSig"),
            ("pySessionCtrl", cString("genesis.avida"), ))
          return edu_main_ctrl

        print """
        FIXME: pyEduMainCtrl
        it would be really nice if we could clean-up the population
        driver without causing a crash. then all of the objects below
        will be properly deleted.
        """
        these_are_not_being_deleted_and_it_really_sucks = [
          ".m_main_controller_factory.m_main_controllers_list[0].m_menu_bar",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_mdl.m_avida_threaded_driver",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_mdl.m_avida_threaded_driver.m_thread",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_mdl.m_avida_threaded_driver.m_updated_semaphore",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_mdl.m_avida_threaded_driver.m_do_update_semaphore",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_mdl.m_avida_threaded_driver.m_environment",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver.m_thread",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_controller_factory.m_session_controllers_list[1].m_avida_threaded_driver.m_do_update_semaphore",
        #  ".m_main_controller_factory.m_main_controllers_list[1].m_session_controller_factory.m_session_controllers_list[1].m_updated_semaphore",
        ]

        endotests = recursiveDeleteChecks(instantiateSessionCtrl, these_are_not_being_deleted_and_it_really_sucks)
        for (endotest, attr_name) in endotests:
          try:
            endotest.verify()
            self.test_is_true(True)
          except AssertionError, err:
            pyEduMainCtrl_delete_checks = """
            Buried attribute either should have been deleted and wasn't,
            or shouldn't have and was :
            %s
            """ % attr_name
            self.test_is_true(False, pyEduMainCtrl_delete_checks)
        
    self.adoptUnitTestCase(deleteChecks())

