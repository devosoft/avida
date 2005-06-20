import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import qt

class pySessionControllerFactory(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)

  def construct(self, m_session_mdl):
    self.m_session_mdl = m_session_mdl
    self.m_controller_creators_dict = {}
    self.m_session_controllers_list = []
    self.connect(self.m_session_mdl.m_session_mdtr,
      qt.PYSIGNAL("newSessionControllerSig"), self.newSessionControllerSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      qt.PYSIGNAL("deleteControllerSig"), self.deleteControllerSlot)
    return self

  def addControllerCreator(self, creator_key, creator):
    self.m_controller_creators_dict[creator_key] = creator

  def newSessionControllerSlot(self, creator_key, *args):
    new_controller = self.m_controller_creators_dict[creator_key]()
    new_controller.construct(self.m_session_mdl, *args)
    self.m_session_controllers_list.append(new_controller)

  def deleteControllerSlot(self, controller):
    self.m_session_controllers_list.remove(controller)


# Unit tests.

from py_test_utils import *
from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pySessionControllerFactory(pyUnitTestSuite):
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
        session_controller_factory_factory = lambda : pySessionControllerFactory().construct(stub_mdl)

        things_that_will_live_on = [
          # I instantiated these above, and (on purpose) they won't be deleted.
          '.m_session_mdl',
          '.m_session_mdl.m_session_mdtr',
        ]

        endotests = recursiveDeleteChecks(session_controller_factory_factory, things_that_will_live_on)
        for (endotest, attr_name) in endotests:
          try:
            endotest.verify()
            self.test_is_true(True)
          except AssertionError, err:
            pySessionControllerFactory_delete_checks = """
            Buried attribute either should have been deleted and wasn't,
            or shouldn't have and was :
            %s
            """ % attr_name
            self.test_is_true(False, pySessionControllerFactory_delete_checks)
        
    self.adoptUnitTestCase(deleteChecks())
