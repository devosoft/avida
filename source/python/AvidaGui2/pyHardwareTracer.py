
from pyHardwareCPUTrace import pyHardwareCPUTrace

from AvidaCore import cCPUTestInfo, cTestCPU, pyHardwareTracerBase

class pyHardwareTracer(pyHardwareTracerBase):

  def TraceHardware_CPU(self, hardware):
    self.m_hardware_trace.recordFrame(hardware)

  def TraceHardware_CPUBonus(self, hardware):
    self.m_hardware_trace.recordFrame(hardware)

  def backupTestCPU(self, environment):
    backup_environment = cTestCPU.GetEnvironment()
    original_resource_count = cTestCPU.GetResourceCount()
    backup_usage = cTestCPU.GetUseResources()

    cTestCPU.SetEnvironment(environment)
    cTestCPU.SetupResources()
    cTestCPU.SetUseResources(True)

    return backup_environment, original_resource_count, backup_usage

  def restoreTestCPU(self, backup_environment, original_resource_count, backup_usage):
    cTestCPU.SetEnvironment(backup_environment)
    cTestCPU.GetResourceCount().SetTo(original_resource_count)
    cTestCPU.SetUseResources(True)

  def traceAnalyzeGenotype(self, analyze_genotype, environment, should_use_resources):
    backup_environment = None
    original_resource_count = None
    backup_usage = None
    if should_use_resources:
      backup_environment, original_resource_count, backup_usage = self.backupTestCPU(environment)

    test_info = cCPUTestInfo()
    test_info.TestThreads()
    self.m_hardware_trace = pyHardwareCPUTrace()
    test_info.SetTraceExecution(self)
    cTestCPU.TestGenome(test_info, analyze_genotype.GetGenome())

    analyze_genotype.Recalculate()
    self.m_hardware_trace.recordGenotypeSummary(analyze_genotype)

    if should_use_resources: self.restoreTestCPU(backup_environment, original_resource_count, backup_usage)
