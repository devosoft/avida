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
