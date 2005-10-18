
from pyHardwareCPUTrace import pyHardwareCPUTrace

from AvidaCore import cCPUTestInfo, cTestCPU, pyHardwareTracerBase

class pyHardwareTracer(pyHardwareTracerBase):

  def __init__(self, progress_callback = None):
    pyHardwareTracerBase.__init__(self)
    self.m_progress_callback = progress_callback

  def TraceHardware_CPU(self, hardware):
    self.m_hardware_trace.recordFrame(hardware)
    if self.m_progress_callback is not None:
      self.m_trace_progress = self.m_trace_progress + 1
      self.m_progress_callback(self.m_trace_progress)

  def TraceHardware_CPUBonus(self, hardware):
    self.m_hardware_trace.recordFrame(hardware)
    if self.m_progress_callback is not None:
      self.m_trace_progress = self.m_trace_progress + 1
      self.m_progress_callback(self.m_trace_progress)

  # XXX Changes to Avida core have broken the next two functions, and
  # now isn't a good time to fix them. @kgn
  #
  #def backupTestCPU(self, environment):
  #  print "pyHardwareTracer.backupTestCPU()..."
  #  backup_environment = cTestCPU.GetEnvironment()
  #  original_resource_count = cTestCPU.GetResourceCount()
  #  backup_usage = cTestCPU.GetUseResources()

  #  cTestCPU.SetEnvironment(environment)
  #  cTestCPU.SetupResources()
  #  cTestCPU.SetUseResources(True)

  #  return backup_environment, original_resource_count, backup_usage

  #def restoreTestCPU(self, backup_environment, original_resource_count, backup_usage):
  #  cTestCPU.SetEnvironment(backup_environment)
  #  cTestCPU.GetResourceCount().SetTo(original_resource_count)
  #  cTestCPU.SetUseResources(True)

  def traceAnalyzeGenotype(self, analyze_genotype, environment, should_use_resources):
    print "pyHardwareTracer.traceAnalyzeGenotype()..."
    backup_environment = None
    original_resource_count = None
    backup_usage = None

    # Backup test cpu if we need to use resources.
    #if should_use_resources:
    #  backup_environment, original_resource_count, backup_usage = self.backupTestCPU(environment)
    if should_use_resources:
      print " *** Warning in pyHardwareTracer.traceAnalyzeGenotype():"
      print " *** traceAnalyzeGenotype called with should_use_resources = True,"
      print " *** but resources in the python test-cpu are disabled for now."

    # Build the test info for tracing hardware.
    test_info = cCPUTestInfo()
    test_info.TestThreads()
    # Build storage for hardware trace info.
    self.m_hardware_trace = pyHardwareCPUTrace()
    test_info.SetTraceExecution(self)
    # Trace the genotype's execution.
    self.m_trace_progress = 0
    cTestCPU.TestGenome(test_info, analyze_genotype.GetGenome())
    # Record some of the genotype's statistics.
    analyze_genotype.Recalculate()
    self.m_hardware_trace.recordGenotypeSummary(analyze_genotype)

    # Restore test cpu if we need to use resources.
    #if should_use_resources: self.restoreTestCPU(backup_environment, original_resource_count, backup_usage)
