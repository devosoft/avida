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
    print "pyOrganismScopeCtrl.construct()."
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.setAcceptDrops(1)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setDebugOrganismFileSig"),
      self.setDebugOrganismFileSlot)

  def dragEnterEvent( self, e ):
    e.acceptAction(True)
    if e.isAccepted():
      print "pyOrganismScopeCtrl.dragEnterEvent(e): isAccepted."
    else:
      print "pyOrganismScopeCtrl.dragEnterEvent(e): not isAccepted."

    freezer_item_name = QString()
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      if os.path.exists( str(freezer_item_name)) == False:
        print "pyOrganismScopeCtrl.dragEnterEvent(e): that was not a valid path."
      else:
        print "pyOrganismScopeCtrl.dragEnterEvent(e): that was a valid path."
        print "pyOrganismScopeCtrl.dragEnterEvent(e): freezer_item_name", freezer_item_name
        if str(freezer_item_name).endswith('.organism'):
          print "pyOrganismScopeCtrl.dragEnterEvent(e): freezer_item_name ends with .organism."
          e.accept()
        else:
          print "pyOrganismScopeCtrl.dragEnterEvent(e): freezer_item_name doesn't end with .organism."

  def dropEvent( self, e ):
    freezer_item_name = QString()
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      if os.path.exists( str(freezer_item_name)) == False:
        print "pyOrganismScopeCtrl.dropEvent(e): that was not a valid path."
      else:
        print "pyOrganismScopeCtrl.dropEvent(e): that was a valid path."
        if str(freezer_item_name).endswith('.organism'):
          print "pyOrganismScopeCtrl.dropEvent(e): freezer_item_name ends with .organism."
          e.accept()
          self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("setDebugOrganismFileSig"), (freezer_item_name,))
        else:
          print "pyOrganismScopeCtrl.dropEvent(e): freezer_item_name doesn't end with .organism."


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
      org_file = open(organism_filename.ascii())
      org_string = org_file.readline()
      org_string = org_string.rstrip()
      org_string = org_string.lstrip()
      org_file.close
      genome = cGenome(cString(org_string))
      analyze_genotype = cAnalyzeGenotype(genome, inst_set)
      hardware_tracer = pyHardwareTracer()
      hardware_tracer.traceAnalyzeGenotype(analyze_genotype, self.m_avida.m_environment, should_use_resources = False)

      # Translate from string genome representation to actual command names.
      inst_names = {}
      ops = {}
      inst_set = self.m_avida.m_environment.GetInstSet()
      instruction = cInstruction()
      for id in xrange(inst_set.GetSize()):
        instruction.SetOp(id)
        inst_names[instruction.GetSymbol()] = str(inst_set.GetName(instruction))
        ops[instruction.GetSymbol()] = id

      self.setInstNames(inst_names)
      self.setOps(ops)
      self.setFrames(hardware_tracer.m_hardware_trace)
