
import pyMdtr; reload(pyMdtr)
from pyMdtr import pyMdtr

import qt

class pyMainControllerFactory(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)
  def construct(self, main_mdl):
    self.m_main_mdl = main_mdl
    self.m_controller_creators_dict = {}
    self.m_main_controllers_list = []
    self.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr = pyMdtr()
    self.connect(self.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr,
      qt.PYSIGNAL("newMainControllerSig"), self.newMainControllerSlot)
    self.connect(self.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr,
      qt.PYSIGNAL("deleteControllerSig"), self.deleteControllerSlot)
    return self
  def addControllerCreator(self, creator_key, creator):
    self.m_controller_creators_dict[creator_key] = creator
  def newMainControllerSlot(self, creator_key, *args):
    new_controller = self.m_controller_creators_dict[creator_key]()
    new_controller.construct(self.m_main_mdl, *args)
    self.m_main_controllers_list.append(new_controller)
  def deleteControllerSlot(self, controller):
    self.m_main_controllers_list.remove(controller)

# Unit tests.

from py_test_utils import *
from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyMainControllerFactory(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    self.adoptUnitTestSuite("pyMdtr")

    class deleteChecks(pyTestCase):
      def test(self):
        class pyMdl: pass
        stub_mdl = pyMdl()
        stub_mdl.m_main_mdtr = pyMdtr()
        main_controller_factory_factory = lambda : pyMainControllerFactory().construct(stub_mdl)

        these_objects_persist_on_purpose = [
          '.m_main_mdl',
          '.m_main_mdl.m_main_mdtr',
          '.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr'
        ]

        endotests = recursiveDeleteChecks(main_controller_factory_factory, these_objects_persist_on_purpose)
        for (endotest, attr_name) in endotests:
          try:
            endotest.verify()
            self.test_is_true(True)
          except AssertionError, err:
            pyMainControllerFactory_delete_checks = """
            Buried attribute either should have been deleted and wasn't,
            or shouldn't have and was :
            %s
            """ % attr_name
            self.test_is_true(False, pyMainControllerFactory_delete_checks)

    self.adoptUnitTestCase(deleteChecks())

