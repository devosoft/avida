# -*- coding: utf-8 -*-

from pyOrganismScopeView import pyOrganismScopeView, pyOrganismAnimator
from AvidaCore import cAnalyzeGenotype, cGenome, cInstruction, cInstUtil, cString
from pyHardwareTracer import pyHardwareTracer

from descr import descr

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

    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setDebugOrganismFileSig"), self.setDebugOrganismFileSlot)

    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_HeadsTypeCBActivatedSig"),
      self.HeadsTypeCBActivatedSlot)

    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowTaskTestsCBToggledSig"),
      self.ShowTaskTestsCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowRegistersCBToggledSig"),
      self.ShowRegistersCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_AnimateHeadMovementCBToggledSig"),
      self.AnimateHeadMovementCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowStacksCBToggledSig"),
      self.ShowStacksCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowHeadsAsLettersCBToggledSig"),
      self.ShowHeadsAsLettersCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowInstructionNamesCBToggledSig"),
      self.ShowInstructionNamesCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowInputsAndOutputsCBToggledSig"),
      self.ShowInputsAndOutputsCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowFullStacksCBToggledSig"),
      self.ShowFullStacksCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_AnimateInstructionCopyCBToggledSig"),
      self.AnimateInstructionCopyCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_ShowHardwareCBToggledSig"),
      self.ShowHardwareCBToggledSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_AnimateOrganismDivideCBToggledSig"),
      self.AnimateOrganismDivideCBToggledSlot)

    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_LayoutSpacingSBValueChangedSig"),
      self.LayoutSpacingSBValueChangedSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("ScopeConfig_HardwareIndicatorSBValueChangedSig"),
      self.HardwareIndicatorSBValueChangedSlot)


  def dragEnterEvent( self, e ):
    descr(e)
    e.acceptAction(True)
    if e.isAccepted():
      descr("isAccepted.")
    else:
      descr("not isAccepted.")

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
    descr(e)
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
      self.setFrames()

      inst_set = self.m_avida.m_environment.GetInstSet()
      org_file = open(organism_filename.ascii())
      org_string = org_file.readline()
      org_string = org_string.rstrip()
      org_string = org_string.lstrip()
      org_file.close
      genome = cGenome(cString(org_string))
      analyze_genotype = cAnalyzeGenotype(genome, inst_set)


      class ProgressCallback:
        def __init__(self, qobj):
          self.qobj = qobj
          # FIXME : remove hard-coding of 2000 below. @kgn
          self.progress_bar = QProgressBar(2000)
          self.qobj.emit(PYSIGNAL("addStatusBarWidgetSig"), (self.progress_bar, 0, True))
        def clear(self):
          self.qobj.emit(PYSIGNAL("removeStatusBarWidgetSig"), (self.progress_bar,))
          pt = QPoint()
          self.progress_bar.reparent(None, pt)
          del self.progress_bar
        def __call__(self, step):
          self.progress_bar.setProgress(step)
          qApp.processEvents()

      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("setOneOrganismViewNameLabelTextSig"), (organism_filename.ascii(),))
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("statusBarMessageSig"), ("Analyzing organism...",))
      progress_callback = ProgressCallback(self.m_session_mdl.m_session_mdtr)

      hardware_tracer = pyHardwareTracer(progress_callback)
      hardware_tracer.traceAnalyzeGenotype(analyze_genotype, self.m_avida.m_environment, should_use_resources = False)

      progress_callback(2000)
      #del progress_callback

      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("statusBarMessageSig"), ("Setting up organism scope...",))
      qApp.processEvents()

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

      progress_callback.clear()
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("statusBarClearSig"), ())

    else:
      # XXX Temporary. @kgn
      warning_message_box = QMessageBox.warning(
        self,
        "Avida-ED Beta Limitation",
        """In this beta version, you must start an Avida population before you can analyze an organism.
(After you start a population, if you want you can pause it.)""",
        QMessageBox.Ok,
        QMessageBox.NoButton,
        QMessageBox.NoButton
      )

  def HeadsTypeCBActivatedSlot(self, index):
    descr(index)
    self.anim.setDisplayHeadsAs(index)

  def ShowTaskTestsCBToggledSlot(self, bool):
    descr(bool)
  def ShowRegistersCBToggledSlot(self, bool):
    descr(bool)
  def AnimateHeadMovementCBToggledSlot(self, bool):
    descr(bool)
  def ShowStacksCBToggledSlot(self, bool):
    descr(bool)
  def ShowHeadsAsLettersCBToggledSlot(self, bool):
    descr(bool)
  def ShowInstructionNamesCBToggledSlot(self, bool):
    descr(bool)
  def ShowInputsAndOutputsCBToggledSlot(self, bool):
    descr(bool)
  def ShowFullStacksCBToggledSlot(self, bool):
    descr(bool)
  def AnimateInstructionCopyCBToggledSlot(self, bool):
    descr(bool)
  def ShowHardwareCBToggledSlot(self, bool):
    descr(bool)
  def AnimateOrganismDivideCBToggledSlot(self, bool):
    descr(bool)

  def LayoutSpacingSBValueChangedSlot(self, value):
    descr(value)
  def HardwareIndicatorSBValueChangedSlot(self, value):
    descr(value)

