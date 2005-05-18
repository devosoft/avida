# -*- coding: utf-8 -*-

from AvidaCore import cInitFile, cString
from Numeric import *
from pyAvidaStatsInterface import pyAvidaStatsInterface
from pyOneAna_GraphView import pyOneAna_GraphView
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


class pyOneAna_GraphCtrl(pyOneAna_GraphView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyOneAna_GraphView.__init__(self,parent,name,fl)
    self.m_avida_stats_interface = pyAvidaStatsInterface()

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_avida = None

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

    # Start with second graph mode -- "Average Fitness".
    self.m_combo_box.setCurrentItem(2)
    self.modeActivatedSlot(self.m_combo_box.currentItem())

    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("printGraphSig"),
      self.printGraphSlot)

  def load(self, filename, colx, coly):
    print "pyOneAna_GraphCtrl.load - loading from file"
    init_file = cInitFile(cString('default.workspace/freezer/2k.full/' + filename))
    init_file.Load()
    init_file.Compress()

    x_array = zeros(init_file.GetNumLines(), Float)
    y_array = zeros(init_file.GetNumLines(), Float)

    for line_id in range(init_file.GetNumLines()):
      line = init_file.GetLine(line_id)
      x_array[line_id] = line.GetWord(colx - 1).AsDouble()
      y_array[line_id] = line.GetWord(coly - 1).AsDouble()
##
#      self.m_y_array_2[line_id] = line.GetWord(2).AsDouble()
##
    return x_array, y_array

  def modeActivatedSlot(self, index):
    self.m_graph_ctrl.setTitle(self.m_avida_stats_interface.m_entries[index][0])
    self.m_graph_ctrl.clear()
    if index:

      fake_index_1 = 1
      fake_index_2 = 2

      self.m_graph_ctrl.setAxisTitle(QwtPlot.yLeft, self.m_avida_stats_interface.m_entries[fake_index_1][0])
      self.m_graph_ctrl.setAxisTitle(QwtPlot.yRight, self.m_avida_stats_interface.m_entries[fake_index_2][0])

      self.m_graph_ctrl.setAxisAutoScale(QwtPlot.xBottom)
      self.m_graph_ctrl.setAxisAutoScale(QwtPlot.yLeft)
      self.m_graph_ctrl.setAxisAutoScale(QwtPlot.yRight)

      self.m_curve_1_arrays = self.load(
        self.m_avida_stats_interface.m_entries[fake_index_1][1],
        1,
        self.m_avida_stats_interface.m_entries[fake_index_1][2]
      )
      self.m_curve_2_arrays = self.load(
        self.m_avida_stats_interface.m_entries[fake_index_2][1],
        1,
        self.m_avida_stats_interface.m_entries[fake_index_2][2]
      )

      self.m_graph_ctrl.m_curve_1 = self.m_graph_ctrl.insertCurve(self.m_avida_stats_interface.m_entries[fake_index_1][0])
      self.m_graph_ctrl.m_curve_2 = self.m_graph_ctrl.insertCurve(self.m_avida_stats_interface.m_entries[fake_index_2][0])

      self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve_1, self.m_curve_1_arrays[0], self.m_curve_1_arrays[1])
      self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve_2, self.m_curve_2_arrays[0], self.m_curve_2_arrays[1])

      self.m_graph_ctrl.setCurvePen(self.m_graph_ctrl.m_curve_1, QPen(Qt.red))
      self.m_graph_ctrl.setCurvePen(self.m_graph_ctrl.m_curve_2, QPen(Qt.green))

      self.m_graph_ctrl.setCurveYAxis(self.m_graph_ctrl.m_curve_1, QwtPlot.yLeft)
      self.m_graph_ctrl.setCurveYAxis(self.m_graph_ctrl.m_curve_2, QwtPlot.yRight)

      bounding_rect_1 = self.m_graph_ctrl.curve(self.m_graph_ctrl.m_curve_1).boundingRect()
      bounding_rect_2 = self.m_graph_ctrl.curve(self.m_graph_ctrl.m_curve_2).boundingRect()
      bounding_rect = bounding_rect_1.unite(bounding_rect_2)
      self.m_graph_ctrl.m_zoomer.setZoomBase(bounding_rect)

    self.m_graph_ctrl.replot()
      
  def printGraphSlot(self):
    printer = QPrinter()
    if printer.setup():
      filter = PrintFilter()
      if (QPrinter.GrayScale == printer.colorMode()):
        filter.setOptions(QwtPlotPrintFilter.PrintAll & ~QwtPlotPrintFilter.PrintCanvasBackground)
      self.m_graph_ctrl.printPlot(printer, filter)


  def avidaUpdatedSlot(self):
    if self.m_combo_box.currentItem():
      fake_index_1 = 1
      fake_index_2 = 2

      self.m_curve_1_arrays[0] = concatenate(
        (self.m_x_array, [self.m_avida.m_population.GetStats().GetUpdate()]),
        1
      )
      self.m_curve_2_arrays[0] = concatenate(
        (self.m_x_array, [self.m_avida.m_population.GetStats().GetUpdate()]),
        1
      )

      self.m_curve_1_arrays[1] = concatenate(
        (self.m_curve_1_arrays[1],
          [self.m_avida_stats_interface.getValue(
            fake_index_1,
            self.m_avida.m_population.GetStats()
          )]
        ),
        1
      )
      self.m_curve_2_arrays[1] = concatenate(
        (self.m_curve_2_arrays[1],
          [self.m_avida_stats_interface.getValue(
            fake_index_2,
            self.m_avida.m_population.GetStats()
          )]
        ),
        1
      )

      if hasattr(self.m_graph_ctrl, "m_curve_1") and hasattr(self.m_graph_ctrl, "m_curve_2"):
        self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve_1, self.m_curve_1_arrays[0], self.m_curve_1_arrays[1])
        self.m_graph_ctrl.setCurveData(self.m_graph_ctrl.m_curve_2, self.m_curve_2_arrays[0], self.m_curve_2_arrays[1])

        # Quick hack: Cause the zoomer to limit zooming-out to the
        # boundaries of the displayed curve.
        bounding_rect_1 = self.m_graph_ctrl.curve(self.m_graph_ctrl.m_curve_1).boundingRect()
        bounding_rect_2 = self.m_graph_ctrl.curve(self.m_graph_ctrl.m_curve_2).boundingRect()
        bounding_rect = bounding_rect_1.unite(bounding_rect_2)
        self.m_graph_ctrl.m_zoomer.setZoomBase(bounding_rect)
        # Quick hack: If the user has zoomed-in on or -out from the graph,
        # autoscaling will have been disabled. This reenables it.
        self.m_graph_ctrl.setAxisAutoScale(QwtPlot.xBottom)
        self.m_graph_ctrl.setAxisAutoScale(QwtPlot.yLeft)
        self.m_graph_ctrl.setAxisAutoScale(QwtPlot.yRight)
        self.m_graph_ctrl.replot()

