from AvidaCore import cCPUDefs

class pyHardwareCPUTrace:
  def __init__(self):
    self.clearFrames()

  def clearFrames(self):
    self.m_genome_info = []
    self.m_ihead_info = []
    self.m_rhead_info = []
    self.m_whead_info = []
    self.m_fhead_info = []
    self.m_tasks_info = []
    self.m_register_ax_info = []
    self.m_register_bx_info = []
    self.m_register_cx_info = []
    self.m_read_label_info = []
    self.m_last_copy_info = []
    self.m_last_copied_instruction = 0

    self.m_is_viable = None
    self.m_gestation_time = None
    self.m_fitness = None
    self.m_size = None

  def recordFrame(self, hardware):
    self.m_genome_info.append(hardware.GetMemory().AsString().GetData())
    self.m_ihead_info.append(hardware.GetHead(cCPUDefs.s_HEAD_IP).GetPosition())
    self.m_rhead_info.append(hardware.GetHead(cCPUDefs.s_HEAD_READ).GetPosition())
    self.m_whead_info.append(hardware.GetHead(cCPUDefs.s_HEAD_WRITE).GetPosition())
    self.m_fhead_info.append(hardware.GetHead(cCPUDefs.s_HEAD_FLOW).GetPosition())
    self.m_tasks_info.append(hardware.GetOrganism().GetPhenotype().GetCurTaskCount())
    self.m_register_ax_info.append(hardware.GetRegister(cCPUDefs.s_REG_AX))
    self.m_register_bx_info.append(hardware.GetRegister(cCPUDefs.s_REG_BX))
    self.m_register_cx_info.append(hardware.GetRegister(cCPUDefs.s_REG_CX))
    self.m_read_label_info.append(hardware.GetReadLabel().AsString().GetData())

    self.m_last_copied_instruction = max(
      hardware.GetMemory().GetFlagCopied(i) and i or self.m_last_copied_instruction \
      for i in range(self.m_last_copied_instruction, hardware.GetMemory().GetSize())
    )
    self.m_last_copy_info.append(self.m_last_copied_instruction)

  def recordGenotypeSummary(self, cpu_test_info, genotype):
    self.m_is_viable = genotype.GetViable()
    self.m_gestation_time = genotype.GetGestTime()
    self.m_fitness = genotype.GetFitness()
    self.m_size = genotype.GetLength()
