# -*- coding: utf-8 -*-

from AvidaCore import cInitFile, cString
from Numeric import *
from pyAvidaStatsInterface import pyAvidaStatsInterface
from pyOnePop_GraphView import pyOnePop_GraphView
from qt import *
from qwt import *

class PrintFilter(QwtPlotPrintFilter):
  def __init__(self):
    QwtPlotPrintFilter.__init__(self)
  def color(self, c, item, i):
    if not (self.options() & QwtPlotPrintFilter.PrintCanvasBackground):
      if item == QwtPlotPrintFilter.MajorGrid:
        return Qt.darkGray
      elif item == QwtPlotPrintFilter.MinorGrid:
        return Qt.gray
    if item == QwtPlotPrintFilter.Title:
      return Qt.black
    elif item == QwtPlotPrintFilter.AxisScale:
      return Qt.black
    elif item == QwtPlotPrintFilter.AxisTitle:
      return Qt.black
    return Qt.black
  def font(self, f, item, i):
    result = QFont(f)
    result.setPointSize(int(f.pointSize()*1.25))
    return result


class pyOnePop_GraphCtrl(pyOnePop_GraphView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOnePop_GraphView.__init__(self,parent,name,fl)
    self.m_avida_stats_interface = pyAvidaStatsInterface()

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)

    self.m_graph_ctrl.construct(self.m_session_mdl)
    self.m_combo_box.clear()
    self.m_combo_box.setInsertionPolicy(QComboBox.AtBottom)
    for entry in self.m_avida_stats_interface.m_entries:
      self.m_combo_box.insertItem(entry[0])
    self.connect(
      self.m_combo_box, SIGNAL("activated(int)"), self.modeActivatedSlot)

    self.m_x_array = zeros(2, Float)
    self.m_y_array = zeros(2, Float)

    self.m_graph_ctrl.setAxisTitle(QwtPlot.xBottom, "Time (updates)")
    self.m_graph_ctrl.setAxisAutoScale(QwtPlot.xBottom)
    self.modeActivatedSlot(self.m_combo_box.currentItem())

    self.connect(
      self.m_session_mdl.m_session_mdtr.m_workspace_mdtr, PYSIGNAL("printGraphSig"),
      self.printGraphSlot)

  def load(self, filename, colx, coly):
    if (self.m_avida is None) or (self.m_avida.m_population.GetStats().GetUpdate() == 0):
      self.m_x_array = zeros(0, Float)
      self.m_y_array = zeros(0, Float)
    else:
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
    self.m_graph_ctrl.clear()
    if index:
      self.load(
        self.m_avida_stats_interface.m_entries[index][1],
        1,
        self.m_avida_stats_interface.m_entries[index][2]
      )
      self.m_graph_ctrl.m_curve = self.m_graph_ctrl.insertCurve(self.m_avida_stats_interface.m_entries[index][0])
      self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve, self.m_x_array, self.m_y_array)
      self.m_graph_ctrl.setCurvePen(self.m_graph_ctrl.m_curve, QPen(Qt.red))
    self.m_graph_ctrl.replot()
      
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
    if self.m_combo_box.currentItem():
      self.m_x_array = concatenate(
        (self.m_x_array, [self.m_avida.m_population.GetStats().GetUpdate()]),
        1
      )
      self.m_y_array = concatenate(
        (self.m_y_array,
          [self.m_avida_stats_interface.getValue(
            self.m_combo_box.currentItem(),
            self.m_avida.m_population.GetStats()
          )]
        ),
        1
      )
      if hasattr(self.m_graph_ctrl, "m_curve"):
        self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve, self.m_x_array, self.m_y_array)
        # Quick hack: Cause the zoomer to limit zooming-out to the
        # boundaries of the displayed curve.
        self.m_graph_ctrl.m_zoomer.setZoomBase(self.m_graph_ctrl.curve(self.m_graph_ctrl.m_curve).boundingRect())
      # Quick hack: If the user has zoomed-in on or -out from the graph,
      # autoscaling will have been disabled. This reenables it.
      self.m_graph_ctrl.setAxisAutoScale(QwtPlot.xBottom)
      self.m_graph_ctrl.setAxisAutoScale(QwtPlot.yLeft)
      self.m_graph_ctrl.replot()

  def printGraphSlot(self):
    printer = QPrinter()
    if printer.setup():
      filter = PrintFilter()
      if (QPrinter.GrayScale == printer.colorMode()):
        filter.setOptions(QwtPlotPrintFilter.PrintAll & ~QwtPlotPrintFilter.PrintCanvasBackground)
      self.m_graph_ctrl.printPlot(printer, filter)

