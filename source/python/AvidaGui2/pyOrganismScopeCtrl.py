# -*- coding: utf-8 -*-

from pyOrganismScopeView import pyOrganismScopeView

from AvidaCore import cAnalyzeGenotype, cGenome, cInstruction, cInstUtil, cString

from pyHardwareTracer import pyHardwareTracer

from qt import *

import os
import tempfile


class pyOrganismScopeCtrl(pyOrganismScopeView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOrganismScopeView.__init__(self,parent,name,fl)

    if not name: self.setName("pyOrganismScopeCtrl")

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setDebugOrganismFileSig"),
      self.setDebugOrganismFileSlot)

  def setAvidaSlot(self, avida):
    print "pyOrganismScopeCtrl.setAvidaSlot() ..."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pyOrganismScopeCtrl.setAvidaSlot() deleting old_avida ..."
      del old_avida

  def setDebugOrganismFileSlot(self, organism_filename):
    print "pyOrganismScopeCtrl.setDebugOrganismFileSlot"
    if self.m_avida:
      inst_set = self.m_avida.m_environment.GetInstSet()
      genome = cGenome(cInstUtil.LoadGenome(cString(organism_filename.ascii()), inst_set) )
      analyze_genotype = cAnalyzeGenotype(genome, inst_set)
      hardware_tracer = pyHardwareTracer()
      hardware_tracer.traceAnalyzeGenotype(analyze_genotype, self.m_avida.m_environment, should_use_resources = False)

      # Translate from string genome representation to actual command names.
      inst_names = {}
      inst_set = self.m_avida.m_environment.GetInstSet()
      instruction = cInstruction()
      for id in range(inst_set.GetSize()):
        instruction.SetOp(id)
        inst_names[instruction.GetSymbol()] = str(inst_set.GetName(instruction))

      self.setInstNames(inst_names)
      self.setFrames(hardware_tracer.m_hardware_trace)
