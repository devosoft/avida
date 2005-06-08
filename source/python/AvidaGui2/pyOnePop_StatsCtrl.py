
# -*- coding: utf-8 -*-

from qt import *
from pyOnePop_StatsView import pyOnePop_StatsView
from pyOrgSquareCtrl import pyOrgSquareCtrl


class pyOnePop_StatsCtrl(pyOnePop_StatsView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_StatsView.__init__(self,parent,name,fl)
       
  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.m_org_square_ctrl.construct(self.m_session_mdl)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("orgClickedOnSig"),
      self.updateOrgReportSlot)
    self.m_clicked_cell_number = -99

  def setAvidaSlot(self, avida):
    print "pyOnePop_StatsCtrl.setAvidaSlot() ..."
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pyOnePop_StatsCtrl.setAvidaSlot() disconnecting old_avida ..."
      self.disconnect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      del old_avida
    if(self.m_avida):
      print "pyOnePop_StatsCtrl.setAvidaSlot() connecting self.m_avida ..."
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)

  def avidaUpdatedSlot(self):
    stats = self.m_avida.m_population.GetStats()
             
    #STATISTICS WINDOW
    string_output_length = 7

    if stats.GetAveFitness()<100000 : 
      avg_fitness = str(stats.GetAveFitness())
      string_length = len(avg_fitness)
      while string_length < string_output_length:
        avg_fitness = avg_fitness + '0'
        string_length = string_length+1
      self.m_avg_fitness.setText(avg_fitness[0:string_output_length])
    else:
      avg_fitness = "%.2g" %(stats.GetAveFitness())
      self.m_avg_fitness.setText(avg_fitness)

    # i got rid of dominant stats, jmc
#    dom_fitness = str(stats.GetDomFitness())
#    string_length = len(dom_fitness)
#    while string_length < string_output_length:
#      dom_fitness = dom_fitness + '0'
#      string_length = string_length+1
#    self.m_dom_fitness.setText(dom_fitness[0:string_output_length])

    num_orgs = stats.GetNumCreatures()
    self.m_num_orgs.setText(QString("%1").arg(num_orgs))

    avg_gest = "%d" %(stats.GetAveGestation())
#    string_length = len(avg_gest)
#    while string_length < string_output_length:
#      avg_gest = avg_gest + '0'
#      string_length = string_length+1
    self.m_avg_gest.setText(QString("%1").arg(avg_gest))
 
    avg_merit = "%d" %(stats.GetAveMerit())
    self.m_avg_merit.setText(QString("%1").arg(avg_merit))

    avg_age = "%d" %(stats.GetAveCreatureAge())
    self.m_avg_age.setText(QString("%1").arg(avg_age))

    avg_genome_length = "%d" %(stats.GetAveSize())
    self.m_avg_genome_length.setText(QString("%1").arg(avg_genome_length))


    #TASK OUTLOOK 

#    #if num_orgs_doing_a_given_task is above this number, we say the pop is doing this task
#    m_org_threshold = 1   
 
    num_not = str(stats.GetTaskLastCount(0))
#    if num_not > m_org_threshold:
#      self.m_num_not.setText(QString("yes"))
#    else:
#      self.m_num_not.setText(QString("no"))
    self.m_num_not.setText(num_not)
    
    num_nand = str(stats.GetTaskLastCount(1))
#    if num_nand > m_org_threshold:
#      self.m_num_nand.setText(QString("yes"))
#    else:
#      self.m_num_nand.setText(QString("no"))
    self.m_num_nand.setText(num_nand)

    num_and = str(stats.GetTaskLastCount(2))
#    if num_and > m_org_threshold:
#      self.m_num_and.setText(QString("yes"))
#    else:
#      self.m_num_and.setText(QString("no"))
    self.m_num_and.setText(num_and)

    num_ornot = str(stats.GetTaskLastCount(3))
#    if num_ornot > m_org_threshold:
#      self.m_num_ornot.setText(QString("yes"))
#    else:
#      self.m_num_ornot.setText(QString("no"))
    self.m_num_ornot.setText(num_ornot)

    num_or = str(stats.GetTaskLastCount(4))
#    if num_or > m_org_threshold:
#      self.m_num_or.setText(QString("yes"))
#    else:
#      self.m_num_or.setText(QString("no"))
    self.m_num_or.setText(num_or)

    num_andnot = str(stats.GetTaskLastCount(5))
#    if num_andnot > m_org_threshold:
#      self.m_num_andnot.setText(QString("yes"))
#    else:
#      self.m_num_andnot.setText(QString("no"))
    self.m_num_andnot.setText(num_andnot)

    num_nor = str(stats.GetTaskLastCount(6))
#    if num_nor > m_org_threshold:
#      self.m_num_nor.setText(QString("yes"))
#    else:
#      self.m_num_nor.setText(QString("no"))
    self.m_num_nor.setText(num_nor)

    num_xor = str(stats.GetTaskLastCount(7))
#    if num_xor > m_org_threshold:
#      self.m_num_xor.setText(QString("yes"))
#    else:
#      self.m_num_xor.setText(QString("no"))
    self.m_num_xor.setText(num_xor)

    num_equals = str(stats.GetTaskLastCount(8))
#    if num_equals > m_org_threshold:
#      self.m_num_equals.setText(QString("yes"))
#    else:
#      self.m_num_equals.setText(QString("no"))
    self.m_num_equals.setText(num_equals)
    
    if self.m_clicked_cell_number>= 0: 
      self.updateOrgReportSlot(self.m_clicked_cell_item)



  def updateOrgReportSlot(self, clicked_cell_item):

    self.m_clicked_cell_item = clicked_cell_item
    if clicked_cell_item:
      clicked_cell_num = clicked_cell_item.m_population_cell.GetID()
#the_item.brush().color()
#the_item.m_population_cell.GetID()
    if clicked_cell_item:
      self.m_clicked_cell_number = clicked_cell_num
#    if clicked_cell_num is None or not self.m_avida.m_population.GetCell(int(clicked_cell_num)).IsOccupied():
    if clicked_cell_item is None or not self.m_avida.m_population.GetCell(int(clicked_cell_num)).IsOccupied():
      #PAINT the stats fields empty
      self.m_org_name.setText('empty cell')
      self.m_org_fitness.setText('-')
      self.m_org_merit.setText('-')
      self.m_org_genome_length.setText('-')
      self.m_org_gestation_time.setText('-')
      self.m_org_age.setText('-')
   
      #the tasks 
      self.m_num_not_clickedOrg.setText('-')
      self.m_num_nand_clickedOrg.setText('-')
      self.m_num_and_clickedOrg.setText('-')
      self.m_num_ornot_clickedOrg.setText('-')
      self.m_num_or_clickedOrg.setText('-')
      self.m_num_andnot_clickedOrg.setText('-')
      self.m_num_nor_clickedOrg.setText('-')
      self.m_num_xor_clickedOrg.setText('-')
      self.m_num_equals_clickedOrg.setText('-')
      self.m_org_square_ctrl.paint(Qt.black)

      return

    self.m_org_square_ctrl.paint(clicked_cell_item.brush().color())

    clicked_cell = self.m_avida.m_population.GetCell(int(clicked_cell_num))

    organism = clicked_cell.GetOrganism()
    phenotype = organism.GetPhenotype()
    genotype = organism.GetGenotype()

    # print info about the org clicked on 

    m_org_name = str(genotype.GetName())
    hyphen_position = m_org_name.find('-')
    m_org_name = m_org_name[hyphen_position+1:]   
    self.m_org_name.setText(str(m_org_name))

    m_org_fitness = phenotype.GetFitness()
    self.m_org_fitness.setText(QString("%1").arg(m_org_fitness))    

    m_org_merit = phenotype.GetMerit().GetDouble()
    self.m_org_merit.setText(str(m_org_merit))    

    m_org_genome_length = phenotype.GetGenomeLength()
    self.m_org_genome_length.setText(str(m_org_genome_length))

    m_org_gestation_time = phenotype.GetGestationTime()
    self.m_org_gestation_time.setText(QString("%1").arg(m_org_gestation_time))

    m_org_age = phenotype.GetAge()
    self.m_org_age.setText(QString("%1").arg(m_org_age))
    


    #   print the tasks the clicked on organism is doing 
 
    # get the Tarray of tasks
    m_clickedOrg_task_count = phenotype.GetCurTaskCount()

    num_not_clickedOrg = m_clickedOrg_task_count[0]
    self.m_num_not_clickedOrg.setText(str(num_not_clickedOrg))

    num_nand_clickedOrg = m_clickedOrg_task_count[1]
    self.m_num_nand_clickedOrg.setText(str(num_nand_clickedOrg))

    num_and_clickedOrg = m_clickedOrg_task_count[2]
    self.m_num_and_clickedOrg.setText(str(num_and_clickedOrg))

    num_ornot_clickedOrg = m_clickedOrg_task_count[3]
    self.m_num_ornot_clickedOrg.setText(str(num_ornot_clickedOrg))

    num_or_clickedOrg = m_clickedOrg_task_count[4]
    self.m_num_or_clickedOrg.setText(str(num_or_clickedOrg))

    num_andnot_clickedOrg = m_clickedOrg_task_count[5]
    self.m_num_andnot_clickedOrg.setText(str(num_andnot_clickedOrg))

    num_nor_clickedOrg = m_clickedOrg_task_count[6]
    self.m_num_nor_clickedOrg.setText(str(num_nor_clickedOrg))

    num_xor_clickedOrg = m_clickedOrg_task_count[7]
    self.m_num_xor_clickedOrg.setText(str(num_xor_clickedOrg))

    num_equals_clickedOrg = m_clickedOrg_task_count[8]
    self.m_num_equals_clickedOrg.setText(str(num_equals_clickedOrg))




