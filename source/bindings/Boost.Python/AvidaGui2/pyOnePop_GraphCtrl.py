# -*- coding: utf-8 -*-

from AvidaCore import cInitFile, cString
from Numeric import *
from pyAvidaStatsInterface import pyAvidaStatsInterface
from pyOnePop_GraphView import pyOnePop_GraphView
from qt import *
from qwt import *


class pyOnePop_GraphCtrl(pyOnePop_GraphView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_GraphView.__init__(self,parent,name,fl)
    self.m_avida_stats_interface = pyAvidaStatsInterface()

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_graph_ctrl.construct(self.m_session_mdl)
    self.m_combo_box.clear()
    self.m_combo_box.setInsertionPolicy(QComboBox.AtBottom)
    for entry in self.m_avida_stats_interface.m_entries:
      self.m_combo_box.insertItem(entry[0])

    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr,
      PYSIGNAL("AvidaUpdatedSig"), self.avidaUpdatedSlot)
    self.connect(self.m_combo_box, SIGNAL("activated(int)"), self.modeActivatedSlot)

    self.m_x_array = zeros(2, Float)
    self.m_y_array = zeros(2, Float)

    self.m_graph_ctrl.setAxisTitle(QwtPlot.xBottom, "Time (updates)")
    self.modeActivatedSlot(self.m_combo_box.currentItem())

  def load(self, filename, colx, coly):
    init_file = cInitFile(cString(filename))
    init_file.Load()
    init_file.Compress()

    self.m_x_array = zeros(init_file.GetNumLines(), Float)
    self.m_y_array = zeros(init_file.GetNumLines(), Float)

    for line_id in range(init_file.GetNumLines()):
      line = init_file.GetLine(line_id)
      self.m_x_array[line_id] = line.GetWord(colx - 1).AsDouble()
      self.m_y_array[line_id] = line.GetWord(coly - 1).AsDouble()

  def modeActivatedSlot(self, index):
    self.m_graph_ctrl.setTitle(self.m_avida_stats_interface.m_entries[index][0])
    if index:
      self.m_graph_ctrl.clear()
      self.load(self.m_avida_stats_interface.m_entries[index][1], 1, self.m_avida_stats_interface.m_entries[index][2])
      self.m_graph_ctrl.m_curve = self.m_graph_ctrl.insertCurve(self.m_avida_stats_interface.m_entries[index][0])
      self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve, self.m_x_array, self.m_y_array)
      self.m_graph_ctrl.setCurvePen(self.m_graph_ctrl.m_curve, QPen(Qt.red))
      self.m_graph_ctrl.replot()
      

  def avidaUpdatedSlot(self):
    if self.m_combo_box.currentItem():
      self.m_x_array = concatenate(
        (self.m_x_array, [self.m_session_mdl.m_population.GetStats().GetUpdate()]),
        1
      )
      self.m_y_array = concatenate(
        (self.m_y_array,
          [self.m_avida_stats_interface.getValue(self.m_combo_box.currentItem(), self.m_session_mdl.m_population)]),
        1
      )
      self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve, self.m_x_array, self.m_y_array)
      self.m_graph_ctrl.replot()
