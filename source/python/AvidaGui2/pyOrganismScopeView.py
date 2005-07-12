# -*- coding: utf-8 -*-


from qt import PYSIGNAL, QBrush, QColor, QFont, QFontMetrics, QPen, QPointArray, QPoint, QScrollView, Qt, QWMatrix
from qtcanvas import QCanvas, QCanvasEllipse, QCanvasLine, QCanvasSpline, QCanvasText, QCanvasView
import math

class pyHeadPath:
  def __init__(self, canvas):
    self.m_canvas = canvas
    self.m_line1 = QCanvasLine(canvas)
    self.m_line2 = QCanvasLine(canvas)
  def setCanvas(self, canvas):
    self.m_canvas = canvas
    self.m_line1.setCanvas(canvas)
    self.m_line2.setCanvas(canvas)
  def setControlPoints(self, point_array, ignored_bool):
    x0 = point_array.point(0)[0]
    x1 = point_array.point(1)[0]
    x2 = point_array.point(2)[0]
    y0 = point_array.point(0)[1]
    y1 = point_array.point(1)[1]
    y2 = point_array.point(2)[1]
    self.m_line1.setPoints(x0, y0, x1, y1)
    self.m_line2.setPoints(x1, y1, x2, y2)
  def setPen(self, pen):
    self.m_line1.setPen(pen)
    self.m_line2.setPen(pen)
  def show(self):
    self.m_line1.show()
    self.m_line2.show()
  def hide(self):
    self.m_line1.hide()
    self.m_line2.hide()

class pyOrganismScopeView(QCanvasView):
  def __init__(self,parent = None,name = None,fl = 0):
    QCanvasView.__init__(self,parent,name,fl)
    if not name: self.setName("pyOrganismScopeView")

    self.setVScrollBarMode(QScrollView.AlwaysOff)
    self.setHScrollBarMode(QScrollView.AlwaysOff)
    self.m_canvas = QCanvas()
    self.setCanvas(self.m_canvas)

    font = QFont(self.font())
    font_metrics = QFontMetrics(font)
    self.m_font_points_per_pixel = font.pointSizeFloat()/font_metrics.height()

    self.m_organism_circle_margin = 10
    self.m_font_oversize_factor = 1.2

    self.m_visible_width = 0
    self.m_visible_height = 0

    self.reset()

  def reset(self):
    if hasattr(self, "m_instruction_items") and self.m_instruction_items is not None:
      for item in self.m_instruction_items:
        item.setCanvas(None)
    if hasattr(self, "m_instruction_bg_items") and self.m_instruction_bg_items is not None:
      for item in self.m_instruction_bg_items:
        item.setCanvas(None)
    if hasattr(self, "m_ihead_move_items") and self.m_ihead_move_items is not None:
      for item in self.m_ihead_move_items:
        item.setCanvas(None)
    self.m_instruction_items = None
    self.m_task_items = None
    self.m_ihead_item = None
    self.m_rhead_item = None
    self.m_whead_item = None
    self.m_fhead_item = None
    self.m_ihead_move_items = None

    self.m_task_names = None
    self.m_inst_names = None

    self.m_frames = None
    self.m_max_genome_size = 0
    self.m_current_frame_number = 0
    self.m_current_radius = None
    self.m_current_genome = None
    self.m_current_tasks = None
    self.m_current_ihead = None
    self.m_current_rhead = None
    self.m_current_whead = None
    self.m_current_fhead = None
    self.m_current_ihead_move = None

    # Hmm; can't emit gestationTimeChangedSig(0) without causing absurd slider values. @kgn
    self.emit(PYSIGNAL("gestationTimeChangedSig"),(1,))
    self.emit(PYSIGNAL("executionStepResetSig"),(0,))

  def setTaskNames(self, task_names = None):
    self.m_task_names = task_names

  def setInstNames(self, inst_names = None):
    self.m_inst_names = inst_names

  def setOps(self, ops = None):
    self.m_ops_dict = ops

  def setFrames(self, frames = None):
    self.reset()
    self.m_frames = frames
    if self.m_frames is not None:
      if self.m_frames.m_genome_info is not None:
        self.m_max_genome_size = max([len(genome) for genome in self.m_frames.m_genome_info])
        self.m_instruction_items = [QCanvasText(self.m_canvas) for i in xrange(self.m_max_genome_size)]
        self.m_instruction_bg_items = [QCanvasEllipse(self.m_canvas) for i in xrange(self.m_max_genome_size)]
        for item in self.m_instruction_items:
          item.setTextFlags(Qt.AlignCenter)
          item.setZ(1.)
        for item in self.m_instruction_bg_items:
          item.setZ(0.)
      if self.m_frames.m_ihead_moves is not None:
        #self.m_ihead_move_items = [QCanvasSpline(self.m_canvas) for i in xrange(len(self.m_frames.m_ihead_moves))]
        self.m_ihead_move_items = [pyHeadPath(self.m_canvas) for i in xrange(len(self.m_frames.m_ihead_moves))]
      self.emit(PYSIGNAL("gestationTimeChangedSig"),(self.m_frames.m_gestation_time,))
      self.updateCircle()
      self.showFrame(0)

  def viewportResizeEvent(self, resize_event):
    QCanvasView.viewportResizeEvent(self, resize_event)
    # XXX Fragility: If the top-level Avida-ED window is allowed to change height, this is going to cause a painful slow-down
    # whenever the window height changes.  But having chosen a fixed window height, we're okay for now.
    #
    # @kgn
    if self.m_visible_height != resize_event.size().height() or self.m_visible_width != resize_event.size().width():
      self.m_canvas.resize(resize_event.size().width(), resize_event.size().height())
      self.m_visible_width = resize_event.size().width()
      self.m_visible_height = resize_event.size().height()
      self.updateCircle()
      self.showFrame(self.m_current_frame_number)

  def updateCircle(self):
    self.m_circle_center_x = self.m_visible_width / 2
    self.m_circle_center_y = self.m_visible_height / 2
    self.m_max_circle_radius = (self.m_visible_height / 2) - self.m_organism_circle_margin
    if self.m_instruction_items is not None:
      text_height = 2 * 3.14159 * self.m_max_circle_radius / self.m_max_genome_size
      font = QFont(self.font())
      point_size_float = self.m_font_oversize_factor * text_height * self.m_font_points_per_pixel
      font.setPointSizeFloat(point_size_float)
      for item in self.m_instruction_items:
        item.setFont(font)
      for item in self.m_instruction_bg_items:
        item.setSize(point_size_float, point_size_float)
      self.m_circles = []
      self.m_circle_radii = []
      for frame_no in xrange(self.m_frames.m_gestation_time):
        organism_current_size = max(self.m_frames.m_last_copy_info[frame_no] + 1, self.m_frames.m_size)
        circumference = text_height * organism_current_size
        radius = circumference / (2 * 3.14159)
        dt = 2 * 3.14159 / (organism_current_size + 1)
        angle_offset = 3.14159 / 2
        circle_pts = []
        for i in xrange(organism_current_size):
          theta = i * dt + angle_offset
          c = math.cos(theta)
          s = -math.sin(theta)
          x = radius * c + self.m_circle_center_x
          y = radius * s + self.m_circle_center_y
          circle_pts.append((x,y))
        self.m_circles.append(circle_pts)
        self.m_circle_radii.append(radius)

  def showFrame(self, frame_number = 0):
    old_frame_number = self.m_current_frame_number
    old_radius = self.m_current_radius
    old_genome = self.m_current_genome
    old_tasks = self.m_current_tasks
    old_ihead = self.m_current_ihead
    old_rhead = self.m_current_rhead
    old_whead = self.m_current_whead
    old_fhead = self.m_current_fhead
    old_ihead_move = self.m_current_ihead_move

    self.m_current_frame_number = 0
    self.m_current_radius = None
    self.m_current_genome = None
    self.m_current_tasks = None
    self.m_current_ihead = None
    self.m_current_rhead = None
    self.m_current_whead = None
    self.m_current_fhead = None
    self.m_current_ihead_move = None

    circle_pts = None

    if self.m_frames is not None and frame_number < self.m_frames.m_gestation_time:
      self.m_current_frame_number = frame_number
      self.m_current_radius = self.m_circle_radii[frame_number]
      circle_pts = self.m_circles[frame_number]
      if self.m_frames.m_genome_info is not None:
        self.m_current_genome = self.m_frames.m_genome_info[frame_number]
        if old_genome is None:
          displayed_genome_size = max(self.m_frames.m_last_copy_info[self.m_current_frame_number], self.m_frames.m_size)
          # Update all instruction_items.
          color = QColor()
          for i in xrange(displayed_genome_size):
            item = self.m_instruction_items[i]
            item.setX(circle_pts[i][0])
            item.setY(circle_pts[i][1])
            item.setText(self.m_current_genome[i])
            item.show()

            bg_item = self.m_instruction_bg_items[i]
            bg_item.setX(circle_pts[i][0])
            bg_item.setY(circle_pts[i][1])
            color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
            bg_item.setBrush(QBrush(color))
            bg_item.show()
        else:
          # Update changed instruction_items.
          old_length = max(self.m_frames.m_last_copy_info[old_frame_number] + 1, self.m_frames.m_size)
          new_length = max(self.m_frames.m_last_copy_info[self.m_current_frame_number] + 1, self.m_frames.m_size)
          compare_max = min(old_length, new_length)
          range_end = max(old_length, new_length)

          if old_radius != self.m_current_radius:
            for i in xrange(compare_max):
              item = self.m_instruction_items[i]
              item.setX(circle_pts[i][0])
              item.setY(circle_pts[i][1])
              bg_item = self.m_instruction_bg_items[i]
              bg_item.setX(circle_pts[i][0])
              bg_item.setY(circle_pts[i][1])
            if old_length < new_length:
              for i in xrange(compare_max, range_end):
                item = self.m_instruction_items[i]
                item.setX(circle_pts[i][0])
                item.setY(circle_pts[i][1])
                bg_item = self.m_instruction_bg_items[i]
                bg_item.setX(circle_pts[i][0])
                bg_item.setY(circle_pts[i][1])

          for i in xrange(compare_max):
            instruction_item = self.m_instruction_items[i]
            if old_genome[i] == self.m_current_genome[i]:
              pass
            else:
              self.m_instruction_items[i].setText(self.m_current_genome[i])
              color = QColor()
              color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
              self.m_instruction_bg_items[i].setBrush(QBrush(color))

          if old_length < new_length:
            for i in xrange(compare_max, range_end):
              item = self.m_instruction_items[i]
              item.setText(self.m_current_genome[i])
              item.show()
              bg_item = self.m_instruction_bg_items[i]
              color = QColor()
              color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
              bg_item.setBrush(QBrush(color))
              bg_item.show()
          else:
            for i in xrange(compare_max, range_end):
              self.m_instruction_items[i].hide()
              self.m_instruction_bg_items[i].hide()

      if self.m_frames.m_ihead_moves_info is not None:
        self.m_current_ihead_move = self.m_frames.m_ihead_moves_info[frame_number]
        if old_ihead_move is None:
          # Update all ihead_move_items.
          for i in xrange(self.m_current_ihead_move):
            ihead_move_item = self.m_ihead_move_items[self.m_current_ihead_move]
            anchor_radius = float(m_current_radius - 10)
            anchor_radii_ratio = anchor_radius / self.m_current_radius
            control_radii_ratio = 0.4 + 0.5 * pow(2., -float(self.m_frames.m_ihead_moves[i][2])/25)
            #control_radii_ratio = 0.4 + 0.5 * pow(2., -float((1 + self.m_frames.m_ihead_moves[i][2]))/8)
            point_array = QPointArray(3)
            from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[ihead_move_item][0]]
            to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[ihead_move_item][1]]
            point_array[0] = QPoint(
              self.m_circle_center_x + anchor_radii_ratio * (from_circle_pt[0] - self.m_circle_center_x),
              self.m_circle_center_y + anchor_radii_ratio * (from_circle_pt[1] - self.m_circle_center_y)
            )
            point_array[1] = QPoint(
              self.m_circle_center_x + control_radii_ratio * ((from_circle_pt[0] + to_circle_pt[0])/2 - self.m_circle_center_x),
              self.m_circle_center_y + control_radii_ratio * ((from_circle_pt[1] + to_circle_pt[1])/2 - self.m_circle_center_y),
            )
            point_array[2] = QPoint(
              self.m_circle_center_x + anchor_radii_ratio * (to_circle_pt[0] - self.m_circle_center_x),
              self.m_circle_center_y + anchor_radii_ratio * (to_circle_pt[1] - self.m_circle_center_y)
            )
            ihead_move_item.setControlPoints(point_array, False)
            if self.m_frames.m_ihead_moves[i][0] < self.m_frames.m_ihead_moves[i][1]:
              ihead_move_item.setPen(QPen(Qt.blue))
            else:
              ihead_move_item.setPen(QPen(Qt.red))
            ihead_move_item.show()
        else:
          # Update changed ihead_move_items.
          for i in xrange(self.m_current_ihead_move):
            ihead_move_item = self.m_ihead_move_items[i]
            anchor_radius = float(self.m_current_radius - 10)
            anchor_radii_ratio = anchor_radius / self.m_current_radius
            control_radii_ratio = 0.4 + 0.5 * pow(2., -float(self.m_frames.m_ihead_moves[i][2])/25)
            #control_radii_ratio = 0.4 + 0.5 * pow(2., -float((1 + self.m_frames.m_ihead_moves[i][2]))/8)
            point_array = QPointArray(3)
            from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][0]]
            to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][1]]
            point_array.setPoint(0, QPoint(
              self.m_circle_center_x + anchor_radii_ratio * (from_circle_pt[0] - self.m_circle_center_x),
              self.m_circle_center_y + anchor_radii_ratio * (from_circle_pt[1] - self.m_circle_center_y)
            ) )
            point_array.setPoint(1, QPoint(
              self.m_circle_center_x + control_radii_ratio * ((from_circle_pt[0] + to_circle_pt[0])/2 - self.m_circle_center_x),
              self.m_circle_center_y + control_radii_ratio * ((from_circle_pt[1] + to_circle_pt[1])/2 - self.m_circle_center_y),
            ) )
            point_array.setPoint(2, QPoint(
              self.m_circle_center_x + anchor_radii_ratio * (to_circle_pt[0] - self.m_circle_center_x),
              self.m_circle_center_y + anchor_radii_ratio * (to_circle_pt[1] - self.m_circle_center_y)
            ) )
            ihead_move_item.setControlPoints(point_array, False)
            if self.m_frames.m_ihead_moves[i][0] < self.m_frames.m_ihead_moves[i][1]:
              ihead_move_item.setPen(QPen(Qt.blue))
            else:
              ihead_move_item.setPen(QPen(Qt.red))
            ihead_move_item.show()
          if self.m_current_ihead_move < old_ihead_move:
            for i in xrange(self.m_current_ihead_move, old_ihead_move):
              self.m_ihead_move_items[i].hide()
          pass

      if self.m_frames.m_tasks_info is not None:
        self.m_current_tasks = self.m_frames.m_tasks_info[frame_number]
        # Update tasks_item.

      if self.m_frames.m_ihead_info is not None:
        self.m_current_ihead = self.m_frames.m_ihead_info[frame_number]
        # Update changed ihead_item.

      if self.m_frames.m_rhead_info is not None:
        self.m_current_rhead = self.m_frames.m_rhead_info[frame_number]
        # Update changed rhead_item.

      if self.m_frames.m_whead_info is not None:
        self.m_current_whead = self.m_frames.m_whead_info[frame_number]
        # Update changed whead_item.

      if self.m_frames.m_fhead_info is not None:
        self.m_current_fhead = self.m_frames.m_fhead_info[frame_number]
        # Update changed fhead_item.

    self.m_canvas.update()
