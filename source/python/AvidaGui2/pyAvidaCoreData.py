
from AvidaCore import *


class pyAvidaCoreData:
  def construct(self, genesis_filename):
    self.m_genesis_filename = genesis_filename

    # Try to load a genesis file by name. Create the file if needed.
    self.m_genesis = cGenesis()
    self.m_genesis.Open(self.m_genesis_filename)
    if 0 == self.m_genesis.IsOpen():
      print("Warning: Unable to find file '", self.m_genesis_filename(), "'. Creating.")
      cConfig.PrintGenesis(self.m_genesis_filename)
      self.m_genesis.Open(self.m_genesis_filename)

    cConfig.Setup(self.m_genesis)
    
    # Try to load the environment file specified in the genesis file.
    self.m_environment = cEnvironment()
    if 0 == self.m_environment.Load(cConfig.GetEnvironmentFilename()):
      print("Unable to load environment... aborting!")
      import sys
      sys.exit()
    self.m_environment.GetInstSet().SetInstLib(cHardwareCPU.GetInstLib())
    cHardwareUtil.LoadInstSet_CPUOriginal(cConfig.GetInstFilename(), self.m_environment.GetInstSet())
    cConfig.SetNumInstructions(self.m_environment.GetInstSet().GetSize())
    cConfig.SetNumTasks(self.m_environment.GetTaskLib().GetSize())
    cConfig.SetNumReactions(self.m_environment.GetReactionLib().GetSize())
    cConfig.SetNumResources(self.m_environment.GetResourceLib().GetSize())
    
    # Test-CPU creation.
    #self.m_resource_count = cResourceCount(self.m_environment.GetResourceLib().GetSize())
    self.m_test_interface = cPopulationInterface()
    BuildTestPopInterface(self.m_test_interface)
    cTestCPU.Setup(
      self.m_environment.GetInstSet(),
      self.m_environment,
      self.m_environment.GetResourceLib().GetSize(),
      self.m_test_interface)
    return self


# Unit tests.

from py_test_utils import *
from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyAvidaCoreData(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    testing %s
    """ % self.__class__.__name__

    class deleteChecks(pyTestCase):
      def test(self):
        avida_core_data_factory = lambda : pyAvidaCoreData().construct(cString("genesis"))
        endotests = recursiveDeleteChecks(avida_core_data_factory, [])
        for (endotest, attr_name) in endotests:
          try:
            endotest.verify()
            self.test_is_true(True)
          except AssertionError, err:
            pyAvidaCoreData_delete_checks = """
            Buried attribute either should have been deleted and wasn't,
            or shouldn't have and was :
            %s
            """ % attr_name
            self.test_is_true(False, pyAvidaCoreData_delete_checks)

    self.adoptUnitTestCase(deleteChecks())
