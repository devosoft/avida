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
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida): del old_avida

  def setDebugOrganismFileSlot(self, organism_filename):
    print "pyOrganismScopeCtrl.setDebugOrganismFileSlot"
    if self.m_avida:
      inst_set = self.m_avida.m_environment.GetInstSet()
      genome = cGenome(cInstUtil.LoadGenome(cString(organism_filename.ascii()), inst_set) )
      analyze_genotype = cAnalyzeGenotype(genome, inst_set)
      hardware_tracer = pyHardwareTracer()
      hardware_tracer.traceAnalyzeGenotype(analyze_genotype, self.m_avida.m_environment, should_use_resources = False)

      for i in range(len(hardware_tracer.m_hardware_trace.m_genome_info)):
        print "step %d:" % i,
        print "  genome", hardware_tracer.m_hardware_trace.m_genome_info[i]
        #print "  ihead", hardware_tracer.m_hardware_trace.m_ihead_info[i]
        #print "  rhead", hardware_tracer.m_hardware_trace.m_rhead_info[i]
        #print "  whead", hardware_tracer.m_hardware_trace.m_whead_info[i]
        #print "  fhead", hardware_tracer.m_hardware_trace.m_fhead_info[i]
        #print "  ax", hardware_tracer.m_hardware_trace.m_register_ax_info[i]
        #print "  bx", hardware_tracer.m_hardware_trace.m_register_bx_info[i]
        #print "  cx", hardware_tracer.m_hardware_trace.m_register_cx_info[i]
        #print "  read_label", hardware_tracer.m_hardware_trace.m_read_label_info[i]
        print "  last_copy", hardware_tracer.m_hardware_trace.m_last_copy_info[i]

        #print "  tasks:"
        #for j in range(hardware_tracer.m_hardware_trace.m_tasks_info[i].GetSize()):
        #  print "    %d" % j, hardware_tracer.m_hardware_trace.m_tasks_info[i][j]

      print "is_viable", hardware_tracer.m_hardware_trace.m_is_viable
      print "gestation_time", hardware_tracer.m_hardware_trace.m_gestation_time
      print "fitness", hardware_tracer.m_hardware_trace.m_fitness
      print "size", hardware_tracer.m_hardware_trace.m_size


      # Translate from string genome representation to actual command names.
      inst_names = {}
      inst_set = self.m_avida.m_environment.GetInstSet()
      instruction = cInstruction()
      for id in range(inst_set.GetSize()):
        instruction.SetOp(id)
        inst_names[instruction.GetSymbol()] = str(inst_set.GetName(instruction))

      self.setInstNames(inst_names)
      self.setFrames(hardware_tracer.m_hardware_trace)


      # Organism viability.

      # Gestation time.

      # Fitness.

      # Genome frames.
        # Position of last-used instruction. Before allocate, this is
        # the position of the last instruction of the organism. After
        # allocate, at each frame this is the position of the final
        # copied instruction. Usually as each new instructions is copied
        # it becomes the final copied instruction in the genome. I'm
        # tracking this information because I want to animate addition
        # of new instructions at the end of the genome as the organism
        # copies itself.

      # Task frames.

      # Head positions at each frame.

      pass
    #if self.m_avida:
    #  # Analysis files.
    #  genebank_dirname = self.m_session_mdl.m_tempdir
    #  organism_file_basename = os.path.basename(str(organism_filename))
    #  analyze_filename = '%s/organism_analyze.cfg' % self.m_session_mdl.m_tempdir
    #  trace_filename = '%s/%s.trace' % (genebank_dirname, organism_file_basename)
    #  genotype_filename = '%s/%s.gen' % (genebank_dirname, organism_file_basename)

    #  # Analysis results files.
    #  is_viable_info_filename = '%s/is_viable.info' % self.m_session_mdl.m_tempdir
    #  gestation_time_info_filename = '%s/gestation_time.info' % self.m_session_mdl.m_tempdir
    #  fitness_info_filename = '%s/fitness.info' % self.m_session_mdl.m_tempdir
    #  size_info_filename = '%s/size.info' % self.m_session_mdl.m_tempdir
    #  genome_info_filename = '%s/genome.info' % self.m_session_mdl.m_tempdir
    #  tasks_info_filename = '%s/tasks.info' % self.m_session_mdl.m_tempdir
    #  ihead_info_filename = '%s/ihead.info' % self.m_session_mdl.m_tempdir
    #  rhead_info_filename = '%s/rhead.info' % self.m_session_mdl.m_tempdir
    #  whead_info_filename = '%s/whead.info' % self.m_session_mdl.m_tempdir
    #  fhead_info_filename = '%s/fhead.info' % self.m_session_mdl.m_tempdir

    #  # Create a temporary analyze file using organism file name.
    #  analyze_file = open(analyze_filename, 'w')
    #  analyze_file.write('LOAD_ORGANISM %s\n' % organism_filename)
    #  analyze_file.write('RECALCULATE\n')
    #  analyze_file.write('TRACE %s\n' % genebank_dirname)
    #  analyze_file.write('PRINT %s\n' % genebank_dirname)
    #  analyze_file.close()

    #  # Create an analyze object using the temporary analyze file and
    #  # the environment of the current avida object.
    #  AvidaCore.cConfig.SetAnalyzeFilename(AvidaCore.cString(analyze_filename))
    #  avida_driver_analyze = AvidaCore.cAvidaDriver_Analyze(False, self.m_avida.m_environment)
    #  avida_driver_analyze.Run()

    #  # Extract organism viability, gestation time, fitness, and size numbers.
    #  os.system( 'cat ' + genotype_filename + ' | grep Is | grep Viable | cut -d\  -f4-20  > ' + is_viable_info_filename)
    #  os.system( 'cat ' + genotype_filename + ' | grep Gestation | cut -d\  -f4-20  > ' + gestation_time_info_filename)
    #  os.system( 'cat ' + genotype_filename + ' | grep Fitness | cut -d\  -f4-20  > ' + fitness_info_filename)
    #  os.system( 'cat ' + genotype_filename + ' | grep Genome | cut -d\  -f4-20  > ' + size_info_filename)
    #  # Extract genome info.
    #  os.system( 'cat ' + trace_filename + ' | grep Mem | cut -d\  -f6 > ' + genome_info_filename)
    #  # Extract the tasks done at each point in execution.
    #  os.system( 'cat ' + trace_filename + ' | grep Task | cut -d\  -f9-17 > ' + tasks_info_filename)
    #  # Extract the instruction pointer locations from the trace file.
    #  os.system( 'cat ' + trace_filename + ' | grep IP | cut -d\  -f2 | cut -b4-7 > ' + ihead_info_filename)
    #  # Extract the read/write/flow head info.
    #  os.system( 'cat ' + trace_filename + ' | grep "R-Head" | cut -d\  -f3 | cut -d\: -f2 > ' + rhead_info_filename)
    #  os.system( 'cat ' + trace_filename + ' | grep "W-Head" | cut -d\  -f4 | cut -d\: -f2 > ' + whead_info_filename)
    #  os.system( 'cat ' + trace_filename + ' | grep "F-Head" | cut -d\  -f5 | cut -d\: -f2 > ' + fhead_info_filename)

    #  # Load organism viability, gestation time, fitness, and size.
    #  def load_info(info_filename):
    #    info_file = file(info_filename, 'rU')
    #    info = info_file.read()
    #    info_file.close()
    #    return info.strip()

    #  ( is_viable_info_string,
    #    gestation_time_info_string,
    #    fitness_info_string,
    #    size_info_string,
    #  ) = [load_info(info_filename) for info_filename in (
    #    is_viable_info_filename,
    #    gestation_time_info_filename,
    #    fitness_info_filename,
    #    size_info_filename,)]
    #  is_viable_info = bool(int(is_viable_info_string))
    #  gestation_time_info = int(gestation_time_info_string)
    #  fitness_info = float(fitness_info_string)
    #  size_info = int(size_info_string)

    #  # Load genome, tasks, and heads at each point in execution.
    #  def load_info_lines(info_filename):
    #    info_file = file(info_filename, 'rU')
    #    info_lines = info_file.readlines()
    #    info_file.close()
    #    return [info_line.strip() for info_line in info_lines]

    #  ( genome_info,
    #    tasks_info_strings,
    #    ihead_info_strings,
    #    rhead_info_strings,
    #    whead_info_strings,
    #    fhead_info_strings,
    #  ) = [load_info_lines(info_filename) for info_filename in (
    #    genome_info_filename,
    #    tasks_info_filename,
    #    ihead_info_filename,
    #    rhead_info_filename,
    #    whead_info_filename,
    #    fhead_info_filename,)]
    #  tasks_info = [[bool(int(task)) for task in tasks] for tasks in [line.split() for line in tasks_info_strings]]
    #  ihead_info = [int(line) for line in ihead_info_strings]
    #  rhead_info = [int(line) for line in rhead_info_strings]
    #  whead_info = [int(line) for line in whead_info_strings]
    #  fhead_info = [int(line) for line in fhead_info_strings]

    #  # Delete analyze files and analysis results files.
    #  #for filename in (
    #  #  analyze_filename,
    #  #  trace_filename,
    #  #  genotype_filename,
    #  #  is_viable_info_filename,
    #  #  gestation_time_info_filename,
    #  #  fitness_info_filename,
    #  #  size_info_filename,
    #  #  genome_info_filename,
    #  #  tasks_info_filename,
    #  #  ihead_info_filename,
    #  #  rhead_info_filename,
    #  #  whead_info_filename,
    #  #  fhead_info_filename,
    #  #):
    #  #  os.remove(filename)

    #  # load task info
    #  #   determine points at which tasks appear, both in genome and
    #  #   frame number
