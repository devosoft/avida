# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_StatsView import pyOnePop_StatsView


class pyOnePop_StatsCtrl(pyOnePop_StatsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_StatsView.__init__(self,parent,name,fl)
        
  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("orgClickedOnSig"),
      self.updateOrgReportSlot)
    self.clickedCellNumber = -99

  def setAvidaSlot(self, avida):
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)

  def avidaUpdatedSlot(self):
    stats = self.m_avida.m_population.GetStats()
             
    #STATISTICS WINDOW
    avg_merit = stats.GetAveMerit()
    self.m_avg_merit.setText(QString("%1").arg(avg_merit))

    dom_merit = stats.GetDomMerit()
    self.m_dom_merit.setText(QString("%1").arg(dom_merit))

    num_orgs = stats.GetNumCreatures()
    self.m_num_orgs.setText(QString("%1").arg(num_orgs))

    avg_gest = stats.GetAveGestation()
    self.m_avg_gest.setText(QString("%1").arg(avg_gest))

    #TASK OUTLOOK 
    
    #if num_orgs_doing_a_given_task is above this number, we say the pop is doing this task
    m_org_threshold = 1   
 
    num_not = stats.GetTaskLastCount(0)
    if num_not > m_org_threshold:
      self.m_num_not.setText(QString("yes"))
    else:
      self.m_num_not.setText(QString("no"))

    num_nand = stats.GetTaskLastCount(1)
    if num_nand > m_org_threshold:
      self.m_num_nand.setText(QString("yes"))
    else:
      self.m_num_nand.setText(QString("no"))

    num_and = stats.GetTaskLastCount(2)
    if num_and > m_org_threshold:
      self.m_num_and.setText(QString("yes"))
    else:
      self.m_num_and.setText(QString("no"))

    num_ornot = stats.GetTaskLastCount(3)
    if num_ornot > m_org_threshold:
      self.m_num_ornot.setText(QString("yes"))
    else:
      self.m_num_ornot.setText(QString("no"))

    num_or = stats.GetTaskLastCount(4)
    if num_or > m_org_threshold:
      self.m_num_or.setText(QString("yes"))
    else:
      self.m_num_or.setText(QString("no"))

    num_andnot = stats.GetTaskLastCount(5)
    if num_andnot > m_org_threshold:
      self.m_num_andnot.setText(QString("yes"))
    else:
      self.m_num_andnot.setText(QString("no"))

    num_nor = stats.GetTaskLastCount(6)
    if num_nor > m_org_threshold:
      self.m_num_nor.setText(QString("yes"))
    else:
      self.m_num_nor.setText(QString("no"))

    num_xor = stats.GetTaskLastCount(7)
    if num_xor > m_org_threshold:
      self.m_num_xor.setText(QString("yes"))
    else:
      self.m_num_xor.setText(QString("no"))

    num_equals = stats.GetTaskLastCount(8)
    if num_equals > m_org_threshold:
      self.m_num_equals.setText(QString("yes"))
    else:
      self.m_num_equals.setText(QString("no"))
    
    if self.clickedCellNumber>= 0: 
      self.updateOrgReportSlot(self.clickedCellNumber)



  def updateOrgReportSlot(self, clickedCellNum):
    print "an organism was just clicked on!"
  
    self.clickedCellNumber = clickedCellNum
    
    clickedCell = self.m_avida.m_population.GetCell(int(clickedCellNum))

    print "clickedCell.IsOccupied() returns " 
    print clickedCell.IsOccupied()

    if not clickedCell.IsOccupied():
      #PAINT the stats fields empty
      self.m_org_name.setText('empty cell')
      self.m_org_fitness.setText('-')
#      self.m_cur_task_count.setText('-')
#      self.m_org_genome_length.setText('-')
      self.m_org_gestation_time.setText('-')
      self.m_org_age.setText('-')
      return
 
    organism = clickedCell.GetOrganism()
    phenotype = organism.GetPhenotype()
    genotype = organism.GetGenotype()

    m_org_fitness = phenotype.GetFitness()
    self.m_org_fitness.setText(QString("%1").arg(m_org_fitness))    

    m_org_name = genotype.GetName()
    self.m_org_name.setText(str(m_org_name))

#    self.m_org_name.setText(('-'))

#    m_cur_task_count = phenotype.GetCurTaskCount().GetSize()
#    print "m_cur_task_count is "
#    print m_cur_task_count(1)

#    if we want to display length
#    m_org_genome_length = phenotype.GetGenomeLength()
#    print "m_org_genome_length is %f" %(m_org_genome_length)
#    self.m_org_genome_length.setText(QString("%1").arg(m_org_genome_length))

    m_org_gestation_time = phenotype.GetGestationTime()
    self.m_org_gestation_time.setText(QString("%1").arg(m_org_gestation_time))

    m_org_age = phenotype.GetAge()
    self.m_org_age.setText(QString("%1").arg(m_org_age))
    


