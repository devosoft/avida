
from AvidaCore import *

class pyAvidaCoreData:

  def __init__(self, genesis_filename):
    # Try to load a genesis file by name. Create the file if needed.
    self.genesis = cGenesis()
    self.genesis_filename = genesis_filename
    self.genesis.Open(self.genesis_filename)
    if 0 == self.genesis.IsOpen():
      print("Warning: Unable to find file '", self.genesis_filename(), "'. Creating.")
      cConfig.PrintGenesis(self.genesis_filename)
      self.genesis.Open(self.genesis_filename)
    cConfig.Setup(self.genesis)
    
    # Try to load the environment file specified in the genesis file.
    self.environment = cEnvironment()
    if 0 == self.environment.Load(cConfig.GetEnvironmentFilename()):
      print("Unable to load environment... aborting!")
      import sys
      sys.exit()
    self.environment.GetInstSet().SetInstLib(cHardwareCPU.GetInstLib())
    cHardwareUtil.LoadInstSet_CPUOriginal(cConfig.GetInstFilename(), self.environment.GetInstSet())
    cConfig.SetNumInstructions(self.environment.GetInstSet().GetSize())
    cConfig.SetNumTasks(self.environment.GetTaskLib().GetSize())
    cConfig.SetNumReactions(self.environment.GetReactionLib().GetSize())
    cConfig.SetNumResources(self.environment.GetResourceLib().GetSize())
    
    # Test-CPU creation.
    self.resource_count = cResourceCount(self.environment.GetResourceLib().GetSize())
    self.test_interface = cPopulationInterface()
    BuildTestPopInterface(self.test_interface)
    cTestCPU.Setup(self.environment.GetInstSet(), self.environment, self.resource_count, self.test_interface)
    
