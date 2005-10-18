# -*- coding: utf-8 -*-


from AvidaCore import cHardwareDefs, cHardwareCPUDefs

from descr import descr

from qt import *
from qtcanvas import *
import math


class ringHead(QCanvasEllipse):
  def __init__(self, anim, color):
    QCanvasEllipse.__init__(self, anim.canvas)
    descr()
    self.anim = anim
    if hasattr(anim, 'r'):
      diam = 2 * (anim.r + 1)
      self.setSize(diam, diam)
      self.setBrush(QBrush(color))
  def move(self, displayed_genome_size, head_pos):
    circle_pts = self.anim.circles[displayed_genome_size]
    if len(circle_pts) <= head_pos: head_pos = -1
    circle_pt = circle_pts[head_pos]
    self.setX(circle_pt[0])
    self.setY(circle_pt[1])
    self.show()

class textHead(QCanvasText):
  def __init__(self, anim, text):
    QCanvasText.__init__(self, anim.canvas)
    descr()
    self.anim = anim
    self.setTextFlags(Qt.AlignCenter)
    self.setFont(anim.font)
    self.setText(text)
  def move(self, displayed_genome_size, head_pos):
    circle_pts = self.anim.head_circles[displayed_genome_size]
    if len(circle_pts) <= head_pos: head_pos = -1
    circle_pt = circle_pts[head_pos]
    self.setX(circle_pt[0])
    self.setY(circle_pt[1])
    self.show()

class headsDotShapePolicy:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def createHeads(self):
    a = self.anim
    return (ringHead(a, Qt.blue), ringHead(a, Qt.green), ringHead(a, Qt.red), textHead(a, 'F'))
  

class headsLetterShapePolicy:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def createHeads(self):
    a = self.anim
    return (textHead(a, 'I'), textHead(a, 'R'), textHead(a, 'W'), textHead(a, 'F'))


class headsAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
    anim.ihead_item = anim.rhead_item = anim.whead_item = anim.fhead_item = None

  def resetHeads(self, head_items):
    descr()
    a = self.anim
    for head_item in head_items:
      if head_item is not None: head_item.setCanvas(None)
    return a.heads_shape_policy.createHeads()

  def setFrames(self, frames):
    descr()
    a = self.anim
    (a.ihead_item, a.rhead_item, a.whead_item, a.fhead_item
    ) = self.resetHeads((a.ihead_item, a.rhead_item, a.whead_item, a.fhead_item))

  def showFrame(self, frame_number):
    descr()
    a = self.anim
    if a.frames is not None and frame_number < a.frames.m_gestation_time:
      displayed_genome_size = max(a.last_copied_instruction_cache[frame_number] + 1, a.frames.m_size)
      circle_pts = a.head_circles[displayed_genome_size]
      hw = a.frames.getHardwareSnapshotAt(frame_number)
      for id, item in (
        (cHardwareDefs.s_HEAD_IP, a.ihead_item),
        (cHardwareDefs.s_HEAD_READ, a.rhead_item),
        (cHardwareDefs.s_HEAD_WRITE, a.whead_item),
        (cHardwareDefs.s_HEAD_FLOW, a.fhead_item)
      ):
        head_pos = hw.GetHead(id).GetPosition()
        item.move(displayed_genome_size, head_pos)


class instructionsAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
    anim.canvas_ellipses = []
    anim.current_genome_str = None
    anim.current_frame_number = 0
    anim.current_radius = None
    anim.color = QColor()

  def setFrames(self, frames):
    descr("frames", frames)
    a = self.anim
    # canvas items must be detached from canvas before attempt is made to delete them.
    for item in a.canvas_ellipses: item.setCanvas(None)
    # create new canvas items, deleting old ones.
    if frames is None: a.canvas_ellipses = []
    else:
      a.canvas_ellipses = [QCanvasEllipse(a.canvas) for i in xrange(frames.getSnapshotCount())]
      for ellipse in a.canvas_ellipses:
        ellipse.setSize(2*a.r, 2*a.r)
        ellipse.setZ(10.)

  def _placeEllipses(self, circle_pts, rng):
    a = self.anim
    for i in rng:
      ellipse = a.canvas_ellipses[i]
      ellipse.setX(circle_pts[i][0])
      ellipse.setY(circle_pts[i][1])

  def _brushEllipses(self, circle_pts, rng):
    a = self.anim
    for i in rng:
      ellipse = a.canvas_ellipses[i]
      a.color.setHsv((a.ops_dict[a.current_genome_str[i]] * 360) / len(a.ops_dict), 85, 248)
      ellipse.setBrush(QBrush(a.color))
      ellipse.show()

  def showFrame(self, frame_number):
    descr("frame number", frame_number)

    a = self.anim

    old_genome_str = a.current_genome_str
    old_frame_number = a.current_frame_number
    old_radius = a.current_radius

    a.current_frame_number = 0
    a.current_radius = None
    a.current_genome_str = None

    if frame_number < 0:
      descr("bad frame_number %d reset to zero." % frame_number)
      frame_number = 0

    if a.frames is not None and frame_number < a.frames.m_gestation_time:
      a.current_frame_number = frame_number
      a.current_genome_str = a.frames.getMemorySnapshotAt(frame_number).AsString().GetData()
      displayed_genome_size = max(a.last_copied_instruction_cache[frame_number] + 1, a.frames.m_size)
      a.current_radius = a.circle_radii[displayed_genome_size - 1]
      circle_pts = a.circles[displayed_genome_size]
      if old_genome_str is None:
        rng = xrange(displayed_genome_size)
        self._placeEllipses(circle_pts, rng)
        self._brushEllipses(circle_pts, rng)
      else:
        old_length = max(a.last_copied_instruction_cache[old_frame_number] + 1, a.frames.m_size)
        new_length = max(a.last_copied_instruction_cache[frame_number] + 1, a.frames.m_size)
        compare_max = min(old_length, new_length)
        range_end = max(old_length, new_length)
        if old_radius != a.current_radius:
          self._placeEllipses(circle_pts, xrange(compare_max))
          if old_length < new_length:
            self._placeEllipses(circle_pts, xrange(compare_max, range_end))
        for i in xrange(compare_max):
          if old_genome_str[i] != a.current_genome_str[i]:
            a.color.setHsv((a.ops_dict[a.current_genome_str[i]] * 360) / len(a.ops_dict), 85, 248)
            a.canvas_ellipses[i].setBrush(QBrush(a.color))
        if old_length < new_length:
          self._brushEllipses(circle_pts, xrange(compare_max, range_end))
        else:
          for i in xrange(compare_max, range_end):
            a.canvas_ellipses[i].hide()

class instFactoryAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim


class segAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim

class pathAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim

class indicator(QCanvasRectangle):
  def __init__(self, anim, x, y, width, height):
    QCanvasRectangle.__init__(self, x, y, width, height, anim.canvas)
    descr()
    self.anim = anim
  def setBit(self, bit):
    self.setBrush(bit and self.anim.on_brush or self.anim.off_brush)
    self.setPen(bit and self.anim.on_pen or self.anim.off_pen)

class indicatorPolicy:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def createIndicators(self):
    a = self.anim
    hw_anim = a.hw_anim
    x = hw_anim.regs_anim.x
    y = hw_anim.regs_anim.y
    w = a.hardware_indicator_size
    h = a.hardware_indicator_size
    return [indicator(a, x + i*w, y, w, h) for i in xrange(32)]

class bufAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
    self.indicator_items = []
    self.indicator_policy = indicatorPolicy()
  def width(self):
    return 32 * anim.hardware_indicator_size
  def height(self):
    return anim.hardware_indicator_size

  def setReadFnc(self, read_fnc):
    self.read_fnc = read_fnc
  def resetIndicators(self, indicator_items):
    descr()
    a = self.anim
    for item in indicator_items:
      if item is not None: item.setCanvas(None)
    return self.indicator_policy.createIndicators()
  def setX(self, x):
    descr()
  def setY(self, y):
    descr()
  def setZ(self, z):
    descr()
  def show(self):
    descr()
  def hide(self):
    descr()
  def setFrames(self, frames):
    descr()
    self.indicator_items = self.resetIndicators()
  def showFrame(self, frame_number):
    descr()


class regsAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def size(self):
    descr()
    if not self.anim.show_registers_flag:
      return 0, 0
  def setFrames(self, frames):
    descr()
    if self.anim.show_hardware_flag and self.anim.show_registers_flag:
      pass
    else:
      pass
  def showFrame(self, frame_number):
    descr()
    if self.anim.show_hardware_flag and self.anim.show_registers_flag:
      pass
    else:
      pass

class stackAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def size(self):
    descr()
    return 0, 0

class ioAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def size(self):
    descr()
    return 0, 0

class taskTestsAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
  def size(self):
    descr()
    return 0, 0


class hwAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim
    self.regs_anim = regsAnimator(anim)
    self.stack_a_anim = stackAnimator(anim)
    self.stack_b_anim = stackAnimator(anim)
    self.io_anim = ioAnimator(anim)
    self.task_tests_anim = taskTestsAnimator(anim)

    self.x = None
    self.y = None

    self.regs_anim.x = None
    self.regs_anim.y = None

  def size(self):
    descr()
    if not self.anim.show_hardware_flag:
      return 0, 0

    width = height = self.anim.layout_margin
    for a in (self.regs_anim, self.stack_a_anim, self.stack_b_anim, self.io_anim, self.task_tests_anim):
      a_width, a_height = a.size()
      width, height = (max(width, a_width), height + a_height)
    return width, height

  def setFrames(self, frames):
    descr()
    self.x = self.anim.layout_margin
    self.y = self.anim.layout_margin

    self.regs_anim.x = self.x
    self.regs_anim.y = self.y

    self.regs_anim.setFrames(frames)

  def showFrame(self, frame_number):
    descr()
    self.regs_anim.showFrame(frame_number)

class dataAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim


class divideAnimator:
  def __init__(self, anim):
    descr()
    self.anim = anim


class layoutManager:
  def __init__(self, anim):
    descr()
    self.anim = anim
    anim.MAX_ORG_LEN = 200
  def checkDimensions(self, width, height):
    descr(width, height)
    a = self.anim
    hw_width, hw_height = a.hw_anim.size()
    w = width - hw_width
    w = w - 2 * a.layout_margin

    h = height - hw_width
    h = h - 2 * a.layout_margin

    # FIXME: remove hard-coded 200 - the maximum (expected) length of an organism. @kgn
    sin_n = math.sin(math.pi/a.MAX_ORG_LEN)
    sin_n_3 = 3 * sin_n
    R = (h / 2.) / (1 + sin_n_3)
    r = R * sin_n
    rr = r - a.layout_spacing / 2.

    c_x = width - w / 2.
    c_y = height / 2.

    descr(c_x, c_y, R, r, rr)
    return c_x, c_y, R, r, rr

  def setDimensions(self, layout_dims):
    descr()
    a = self.anim
    c_x, c_y, R, r, rr = layout_dims
    descr(c_x, c_y, R, r, rr)
    descr(layout_dims)
    a.circles = []
    a.head_circles = []
    a.circle_radii = []
    a.head_circle_radii = []

    angle_offset = math.pi/2
    for i in xrange(1, a.MAX_ORG_LEN + 2):
      radius = R*i/a.MAX_ORG_LEN
      head_radius = radius + 3*r
      dt = 2*math.pi/i
      circle_pts = []
      head_circle_pts = []
      for j in xrange(i):
        theta = j * dt + angle_offset
        c = math.cos(theta)
        s = -math.sin(theta)
        x = radius * c + c_x
        y = radius * s + c_y
        h_x = head_radius * c + c_x
        h_y = head_radius * s + c_y
        circle_pts.append((x,y))
        head_circle_pts.append((h_x,h_y))
      a.circles.append(circle_pts)
      a.head_circles.append(head_circle_pts)
      a.circle_radii.append(radius)
      a.head_circle_radii.append(head_radius)


class pyOrganismAnimator:
  def __init__(self):
    descr()
    self.heads_anim = headsAnimator(self)
    self.instructions_anim = instructionsAnimator(self)
    self.ihead_path_anim = pathAnimator(self)
    self.hw_anim = hwAnimator(self)
    self.divide_anim = divideAnimator(self)

    self.layout_manager = layoutManager(self)

    self.font = QFont(qApp.font())

    self.on_brush = QBrush(Qt.white)
    self.off_brush = QBrush(Qt.black)
    self.on_pen = QPen(Qt.white)
    self.off_pen = QPen(Qt.black)

    self.hardware_indicator_size = 4
    self.layout_spacing = 2
    self.layout_margin = 20

    self.layout_dims = None

    self.canvas = None
    self.ops_dict = None

    self.frames = None
    
    self.animate_head_movement_flag = False
    self.animate_instruction_copy_flag = False
    self.animate_organism_divide_flag = False
    self.show_hardware_flag = None
    self.show_registers_flag = False
    self.show_stacks_flag = False
    self.show_full_stacks_flag = False
    self.show_io_flag = False
    self.show_task_tests_flag = False
    self.show_instruction_names_flag = False

    self.display_heads_as = None

    self.setDisplayHeadsAs(0)
    self.setShowHardwareCBToggled(True)

  def setOps(self, ops = None):
    self.ops_dict = ops

  def setCanvas(self, canvas = None):
    descr()
    self.canvas = canvas
  def setFrames(self, frames = None):
    descr()
    self.frames = frames
    if frames is None:
      self.last_copied_instruction_cache = None
    else:
      self.last_copied_instruction_cache = []
      last_copied_instruction = 0
      for i in xrange(frames.getSnapshotCount()):
        ms = frames.getMemorySnapshotAt(i)
        last_copied_instruction = max(
          [ms.GetFlagCopied(j) and j or last_copied_instruction for j in xrange(last_copied_instruction, ms.GetSize())]
        )
        self.last_copied_instruction_cache.append(last_copied_instruction)
      descr("self.last_copied_instruction_cache", self.last_copied_instruction_cache)

    self.instructions_anim.setFrames(frames)
    self.heads_anim.setFrames(frames)
    self.hw_anim.setFrames(frames)
  def showFrame(self, frame_number):
    descr()
    self.instructions_anim.showFrame(frame_number)
    self.heads_anim.showFrame(frame_number)
    self.hw_anim.showFrame(frame_number)
    self.canvas.update()
  def animateFrame(self, frame_number):
    descr()
    pass


  def setSize(self, width, height):
    descr()
    self.canvas_width = width
    self.canvas_height = height

    layout_dims = self.layout_manager.checkDimensions(width, height)
    for num in layout_dims:
      if num <= 0:
        return
    if layout_dims != self.layout_dims:
      self.layout_dims = layout_dims
      self.layout_manager.setDimensions(layout_dims)
      descr()
      self.c_x, self.c_y, self.R, self.r, self.rr = layout_dims
      text_height = 3 * self.r
      font_metrics = QFontMetrics(self.font)
      font_points_per_pixel = self.font.pointSizeFloat()/font_metrics.height()
      point_size_float = text_height * font_points_per_pixel
      self.font.setPointSizeFloat(point_size_float)


  def setAnimateHeadMovementCBToggled(self, bool):
    descr()
    if self.animate_head_movement_flag != bool:
      self.animate_head_movement_flag = bool
  def setAnimateInstructionCopyCBToggled(self, bool):
    descr()
    if self.animate_instruction_copy_flag != bool:
      self.animate_instruction_copy_flag = bool
  def setAnimateOrganismDivideCBToggled(self, bool):
    descr()
    if self.animate_organism_divide_flag != bool:
      self.animate_organism_divide_flag = bool


  def setDisplayHeadsAs(self, head_type_index):
    descr()
    heads_shape_policy_table = (
      headsDotShapePolicy,
      headsLetterShapePolicy
    )
    if self.display_heads_as != head_type_index:
      self.display_heads_as = head_type_index
      self.heads_shape_policy = heads_shape_policy_table[head_type_index](self)
      self.heads_anim.setFrames(self.frames)
      self.heads_anim.showFrame(self.current_frame_number)

  def setShowHardwareCBToggled(self, bool):
    descr()
    if self.show_hardware_flag != bool:
      self.show_hardware_flag = bool
      self.hw_anim.setFrames(self.frames)
      self.hw_anim.showFrame(self.current_frame_number)
  def setShowRegistersCBToggled(self, bool):
    descr()
    if self.show_registers_flag != bool:
      self.show_registers_flag = bool
  def setShowStacksCBToggled(self, bool):
    descr()
    if self.show_stacks_flag != bool:
      self.show_stacks_flag = bool
  def setShowFullStacksCBToggled(self, bool):
    descr()
    if self.show_full_stacks_flag != bool:
      self.show_full_stacks_flag = bool
  def setShowInputsAndOutputsCBToggled(self, bool):
    descr()
    if self.show_io_flag != bool:
      self.show_io_flag = bool
  def setShowTaskTestsCBToggled(self, bool):
    descr()
    if self.show_task_tests_flag != bool:
      self.show_task_tests_flag = bool
  def setShowInstructionNamesCBToggled(self, bool):
    descr()
    if self.show_instruction_names_flag != bool:
      self.show_instruction_names_flag = bool


  def setHardwareIndicatorSBValueChanged(self, value):
    descr()
    if self.hardware_indicator_size != value:
      self.hardware_indicator_size = value


  def setLayoutSpacingSBValueChanged(self, value):
    descr()
    if self.layout_spacing != value:
      self.layout_spacing = value
  def setLayoutMarginSBValueChanged(self, value):
    descr()
    if self.layout_margin != value:
      self.layout_margin = value




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

    self.m_visible_width = 1
    self.m_visible_height = 1

    self.anim = pyOrganismAnimator()
    self.anim.setSize(1, 1)
    self.anim.setCanvas(self.m_canvas)

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
    if hasattr(self, "m_ihead_item") and self.m_ihead_item is not None:
      self.m_ihead_item.setCanvas(None)
    if hasattr(self, "m_rhead_item") and self.m_rhead_item is not None:
      self.m_rhead_item.setCanvas(None)
    if hasattr(self, "m_whead_item") and self.m_whead_item is not None:
      self.m_whead_item.setCanvas(None)
    if hasattr(self, "m_fhead_item") and self.m_fhead_item is not None:
      self.m_fhead_item.setCanvas(None)

    self.m_instruction_items = None
    self.m_instruction_bg_items = None
    self.m_ihead_move_items = None
    self.m_ihead_item = None
    self.m_rhead_item = None
    self.m_whead_item = None
    self.m_fhead_item = None
    self.m_task_items = None

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

    self.anim.setFrames()

    # Hmm; can't emit gestationTimeChangedSig(0) without causing absurd slider values. @kgn
    self.emit(PYSIGNAL("gestationTimeChangedSig"),(1,))
    self.emit(PYSIGNAL("executionStepResetSig"),(0,))

  def setTaskNames(self, task_names = None):
    self.m_task_names = task_names

  def setInstNames(self, inst_names = None):
    self.m_inst_names = inst_names

  def setOps(self, ops = None):
    self.m_ops_dict = ops
    self.anim.setOps(ops)

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
      text_height = 2 * 3.14159 * self.m_max_circle_radius / self.m_max_genome_size
      font = QFont(self.font())
      point_size_float = self.m_font_oversize_factor * text_height * self.m_font_points_per_pixel
      font.setPointSizeFloat(point_size_float)
      if self.m_frames.m_ihead_info is not None:
        self.m_ihead_item = QCanvasText(self.m_canvas)
        self.m_ihead_item.setTextFlags(Qt.AlignCenter)
        self.m_ihead_item.setZ(1.)
        self.m_ihead_item.setFont(font)
        self.m_ihead_item.setText('i')
      if self.m_frames.m_rhead_info is not None:
        self.m_rhead_item = QCanvasText(self.m_canvas)
        self.m_rhead_item.setTextFlags(Qt.AlignCenter)
        self.m_rhead_item.setZ(1.)
        self.m_rhead_item.setFont(font)
        self.m_rhead_item.setText('r')
      if self.m_frames.m_whead_info is not None:
        self.m_whead_item = QCanvasText(self.m_canvas)
        self.m_whead_item.setTextFlags(Qt.AlignCenter)
        self.m_whead_item.setZ(1.)
        self.m_whead_item.setFont(font)
        self.m_whead_item.setText('w')
      if self.m_frames.m_fhead_info is not None:
        self.m_fhead_item = QCanvasText(self.m_canvas)
        self.m_fhead_item.setTextFlags(Qt.AlignCenter)
        self.m_fhead_item.setZ(1.)
        self.m_fhead_item.setFont(font)
        self.m_fhead_item.setText('f')
      if self.m_frames.m_ihead_moves is not None:
        #self.m_ihead_move_items = [QCanvasSpline(self.m_canvas) for i in xrange(len(self.m_frames.m_ihead_moves))]
        self.m_ihead_move_items = [pyHeadPath(self.m_canvas) for i in xrange(len(self.m_frames.m_ihead_moves))]

      if self.m_frames.m_is_viable:
        self.emit(PYSIGNAL("gestationTimeChangedSig"),(self.m_frames.m_gestation_time,))
      else:
        self.emit(PYSIGNAL("gestationTimeChangedSig"),(len(self.m_frames.m_genome_info),))
      self.updateCircle()
      self.showFrame(0)

    # XXX XXX
    self.anim.setFrames(frames)
    self.anim.showFrame(0)

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

    # XXX XXX
    self.anim.setSize(self.m_visible_width, self.m_visible_height)
    self.anim.showFrame(self.m_current_frame_number)

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
      self.m_head_circles = []
      self.m_circle_radii = []
      for frame_no in xrange(self.m_frames.m_gestation_time):
        organism_current_size = max(self.m_frames.m_last_copy_info[frame_no] + 1, self.m_frames.m_size)
        circumference = text_height * organism_current_size
        radius = circumference / (2 * 3.14159)
        dt = 2 * 3.14159 / (organism_current_size + 1)
        angle_offset = 3.14159 / 2
        circle_pts = []
        head_circle_pts = []
        for i in xrange(organism_current_size + 1):
          theta = i * dt + angle_offset
          c = math.cos(theta)
          s = -math.sin(theta)
          x = radius * c + self.m_circle_center_x
          y = radius * s + self.m_circle_center_y
          h_x = (radius + 10) * c + self.m_circle_center_x
          h_y = (radius + 10) * s + self.m_circle_center_y
          circle_pts.append((x,y))
          head_circle_pts.append((h_x,h_y))
        self.m_circles.append(circle_pts)
        self.m_head_circles.append(head_circle_pts)
        self.m_circle_radii.append(radius)

  def debug_displayHardwareCPUSnapshot(self, frame_number):
    print "pyOrganismScopeView.debug_displayHardwareCPUSnapshot()..."
    hardware_snapshot = self.m_frames.getHardwareSnapshotAt(frame_number)
    memory_snapshot = self.m_frames.getMemorySnapshotAt(frame_number)
    threads_snapshot = self.m_frames.getThreadsSnapshotAt(frame_number)

    print "hardware_snapshot", hardware_snapshot
    print "memory_snapshot", memory_snapshot
    print "threads_snapshot", threads_snapshot
    memory_size = memory_snapshot.GetSize()
    memory_string = str(memory_snapshot.AsString())
    print "memory_snapshot.GetSize()", memory_size
    print "memory:"
    for i in xrange(memory_size):
      print " ", memory_string[i], memory_snapshot.GetFlagCopied(i), memory_snapshot.GetFlagExecuted(i)
    threads_count = len(threads_snapshot)
    print "thread:"
    for i in xrange(threads_count):
      thread = threads_snapshot[i]
      print " .cur_head", thread.cur_head
      print " .cur_stack", thread.cur_stack
      print " .next_label", thread.next_label
      print " .read_label", thread.read_label
      #print " .stack", thread.stack
      for j in xrange(cHardwareDefs.s_STACK_SIZE):
        print " .stack[%d]" % j, thread.stack.Get(j)
      print " .reg[s_REG_AX]", thread.GetRegister(cHardwareCPUDefs.s_REG_AX)
      print " .reg[s_REG_BX]", thread.GetRegister(cHardwareCPUDefs.s_REG_BX)
      print " .reg[s_REG_CX]", thread.GetRegister(cHardwareCPUDefs.s_REG_CX)
      #print " .reg[s_REG_DX]", thread.GetRegister(cHardwareCPUDefs.s_REG_DX)
      print " .heads[s_HEAD_IP]", thread.GetHead(cHardwareDefs.s_HEAD_IP).GetPosition()
      print " .heads[s_HEAD_READ]", thread.GetHead(cHardwareDefs.s_HEAD_READ).GetPosition()
      print " .heads[s_HEAD_WRITE]", thread.GetHead(cHardwareDefs.s_HEAD_WRITE).GetPosition()
      print " .heads[s_HEAD_FLOW]", thread.GetHead(cHardwareDefs.s_HEAD_FLOW).GetPosition()


    #print "ss", hardware_snapshot
    #print "ss.GetMemory()", hardware_snapshot.GetMemory()
    #print "ss.GetMemory().AsString()", hardware_snapshot.GetMemory().AsString()

    # cCPUMemory memory : cCPUMemory & GetMemory()
    # cCPUStack global_stack : cCPUStack pyGetGlobalStack()
    # int thread_time_used : int pyGetThreadTimeUsed()
    ### assume a single thread
    # cHardwareCPU_Thread thread : tArray<cHardwareCPU_Thread> pyGetThreads()
    # bool mal_active : bool GetMalActive()
    # bool advance_ip : bool pyGetAdvanceIP()

  def showFrame(self, frame_number = 0):
    # XXX
    self.anim.showFrame(frame_number)

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

    if frame_number < 0:
      print "pyOrganismScopeView.showFrame() : bad frame_number", frame_number, "reset to zero."
      frame_number = 0

    if self.m_frames is not None and frame_number < self.m_frames.m_gestation_time:
      # XXX
      #self.debug_displayHardwareCPUSnapshot(frame_number)

      self.m_current_frame_number = frame_number
      self.m_current_radius = self.m_circle_radii[frame_number]
      circle_pts = self.m_circles[frame_number]
      head_circle_pts = self.m_head_circles[frame_number]
      #if self.m_frames.m_genome_info is not None:
      #  self.m_current_genome = self.m_frames.m_genome_info[frame_number]
      #  if old_genome is None:
      #    displayed_genome_size = max(self.m_frames.m_last_copy_info[self.m_current_frame_number], self.m_frames.m_size)
      #    # Update all instruction_items.
      #    color = QColor()
      #    for i in xrange(displayed_genome_size):
      #      item = self.m_instruction_items[i]
      #      item.setX(circle_pts[i][0])
      #      item.setY(circle_pts[i][1])
      #      item.setText(self.m_current_genome[i])
      #      item.show()

      #      bg_item = self.m_instruction_bg_items[i]
      #      bg_item.setX(circle_pts[i][0])
      #      bg_item.setY(circle_pts[i][1])
      #      color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
      #      bg_item.setBrush(QBrush(color))
      #      bg_item.show()
      #  else:
      #    # Update changed instruction_items.
      #    old_length = max(self.m_frames.m_last_copy_info[old_frame_number] + 1, self.m_frames.m_size)
      #    new_length = max(self.m_frames.m_last_copy_info[self.m_current_frame_number] + 1, self.m_frames.m_size)
      #    compare_max = min(old_length, new_length)
      #    range_end = max(old_length, new_length)

      #    if old_radius != self.m_current_radius:
      #      for i in xrange(compare_max):
      #        item = self.m_instruction_items[i]
      #        item.setX(circle_pts[i][0])
      #        item.setY(circle_pts[i][1])
      #        bg_item = self.m_instruction_bg_items[i]
      #        bg_item.setX(circle_pts[i][0])
      #        bg_item.setY(circle_pts[i][1])
      #      if old_length < new_length:
      #        for i in xrange(compare_max, range_end):
      #          item = self.m_instruction_items[i]
      #          item.setX(circle_pts[i][0])
      #          item.setY(circle_pts[i][1])
      #          bg_item = self.m_instruction_bg_items[i]
      #          bg_item.setX(circle_pts[i][0])
      #          bg_item.setY(circle_pts[i][1])

      #    for i in xrange(compare_max):
      #      instruction_item = self.m_instruction_items[i]
      #      if old_genome[i] == self.m_current_genome[i]:
      #        pass
      #      else:
      #        self.m_instruction_items[i].setText(self.m_current_genome[i])
      #        color = QColor()
      #        color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
      #        self.m_instruction_bg_items[i].setBrush(QBrush(color))

      #    if old_length < new_length:
      #      for i in xrange(compare_max, range_end):
      #        item = self.m_instruction_items[i]
      #        item.setText(self.m_current_genome[i])
      #        item.show()
      #        bg_item = self.m_instruction_bg_items[i]
      #        color = QColor()
      #        color.setHsv((self.m_ops_dict[self.m_current_genome[i]] * 360) / len(self.m_ops_dict), 85, 248)
      #        bg_item.setBrush(QBrush(color))
      #        bg_item.show()
      #    else:
      #      for i in xrange(compare_max, range_end):
      #        self.m_instruction_items[i].hide()
      #        self.m_instruction_bg_items[i].hide()





      # if self.m_frames.m_ihead_moves_info is not None:
      #   self.m_current_ihead_move = self.m_frames.m_ihead_moves_info[frame_number]
      #   if old_ihead_move is None:
      #     # Update all ihead_move_items.
      #     for i in xrange(self.m_current_ihead_move):
      #       if len(self.m_ihead_move_items) < i:
      #         print "pyOrganismScopeView.showFrame(): i", i, ", len(self.m_ihead_move_items)", len(self.m_ihead_move_items)
      #       ihead_move_item = self.m_ihead_move_items[self.m_current_ihead_move]
      #       anchor_radius = float(m_current_radius - 10)
      #       anchor_radii_ratio = anchor_radius / self.m_current_radius
      #       control_radii_ratio = 0.4 + 0.5 * pow(2., -float(self.m_frames.m_ihead_moves[i][2])/25)
      #       #control_radii_ratio = 0.4 + 0.5 * pow(2., -float((1 + self.m_frames.m_ihead_moves[i][2]))/8)
      #       point_array = QPointArray(3)
      #       #from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][0]]
      #       #to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][1]]
      #       if len(circle_pts) > self.m_frames.m_ihead_moves[i][0]:
      #         from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][0]]
      #       else:
      #         from_circle_pt = circle_pts[-1]
      #       if len(circle_pts) > self.m_frames.m_ihead_moves[i][1]:
      #         to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][1]]
      #       else:
      #         to_circle_pt = circle_pts[-1]
      #       point_array[0] = QPoint(
      #         self.m_circle_center_x + anchor_radii_ratio * (from_circle_pt[0] - self.m_circle_center_x),
      #         self.m_circle_center_y + anchor_radii_ratio * (from_circle_pt[1] - self.m_circle_center_y)
      #       )
      #       point_array[1] = QPoint(
      #         self.m_circle_center_x + control_radii_ratio * ((from_circle_pt[0] + to_circle_pt[0])/2 - self.m_circle_center_x),
      #         self.m_circle_center_y + control_radii_ratio * ((from_circle_pt[1] + to_circle_pt[1])/2 - self.m_circle_center_y),
      #       )
      #       point_array[2] = QPoint(
      #         self.m_circle_center_x + anchor_radii_ratio * (to_circle_pt[0] - self.m_circle_center_x),
      #         self.m_circle_center_y + anchor_radii_ratio * (to_circle_pt[1] - self.m_circle_center_y)
      #       )
      #       ihead_move_item.setControlPoints(point_array, False)
      #       if self.m_frames.m_ihead_moves[i][0] < self.m_frames.m_ihead_moves[i][1]:
      #         ihead_move_item.setPen(QPen(Qt.blue))
      #       else:
      #         ihead_move_item.setPen(QPen(Qt.red))
      #       ihead_move_item.show()
      #   else:
      #     # Update changed ihead_move_items.
      #     for i in xrange(self.m_current_ihead_move):
      #       ihead_move_item = self.m_ihead_move_items[i]
      #       anchor_radius = float(self.m_current_radius - 10)
      #       anchor_radii_ratio = anchor_radius / self.m_current_radius
      #       control_radii_ratio = 0.4 + 0.5 * pow(2., -float(self.m_frames.m_ihead_moves[i][2])/25)
      #       #control_radii_ratio = 0.4 + 0.5 * pow(2., -float((1 + self.m_frames.m_ihead_moves[i][2]))/8)
      #       point_array = QPointArray(3)
      #       #from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][0]]
      #       #to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][1]]
      #       if len(circle_pts) > self.m_frames.m_ihead_moves[i][0]:
      #         from_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][0]]
      #       else:
      #         from_circle_pt = circle_pts[-1]
      #       if len(circle_pts) > self.m_frames.m_ihead_moves[i][1]:
      #         to_circle_pt = circle_pts[self.m_frames.m_ihead_moves[i][1]]
      #       else:
      #         to_circle_pt = circle_pts[-1]
      #       point_array.setPoint(0, QPoint(
      #         self.m_circle_center_x + anchor_radii_ratio * (from_circle_pt[0] - self.m_circle_center_x),
      #         self.m_circle_center_y + anchor_radii_ratio * (from_circle_pt[1] - self.m_circle_center_y)
      #       ) )
      #       point_array.setPoint(1, QPoint(
      #         self.m_circle_center_x + control_radii_ratio * ((from_circle_pt[0] + to_circle_pt[0])/2 - self.m_circle_center_x),
      #         self.m_circle_center_y + control_radii_ratio * ((from_circle_pt[1] + to_circle_pt[1])/2 - self.m_circle_center_y),
      #       ) )
      #       point_array.setPoint(2, QPoint(
      #         self.m_circle_center_x + anchor_radii_ratio * (to_circle_pt[0] - self.m_circle_center_x),
      #         self.m_circle_center_y + anchor_radii_ratio * (to_circle_pt[1] - self.m_circle_center_y)
      #       ) )
      #       ihead_move_item.setControlPoints(point_array, False)
      #       if self.m_frames.m_ihead_moves[i][0] < self.m_frames.m_ihead_moves[i][1]:
      #         ihead_move_item.setPen(QPen(Qt.blue))
      #       else:
      #         ihead_move_item.setPen(QPen(Qt.red))
      #       ihead_move_item.show()
      #     if self.m_current_ihead_move < old_ihead_move:
      #       for i in xrange(self.m_current_ihead_move, old_ihead_move):
      #         self.m_ihead_move_items[i].hide()
      #     pass





      if self.m_frames.m_tasks_info is not None:
        self.m_current_tasks = self.m_frames.m_tasks_info[frame_number]
        # Update tasks_item.

      #if self.m_frames.m_ihead_info is not None:
      #  self.m_current_ihead = self.m_frames.m_ihead_info[frame_number]
      #  if len(head_circle_pts) < self.m_current_ihead:
      #    self.m_current_ihead = -1
      #  self.m_ihead_item.setX(head_circle_pts[self.m_current_ihead][0])
      #  self.m_ihead_item.setY(head_circle_pts[self.m_current_ihead][1])
      #  self.m_ihead_item.show()

      #if self.m_frames.m_rhead_info is not None:
      #  self.m_current_rhead = self.m_frames.m_rhead_info[frame_number]
      #  if len(head_circle_pts) <= self.m_current_rhead:
      #    self.m_current_rhead = -1
      #  self.m_rhead_item.setX(head_circle_pts[self.m_current_rhead][0])
      #  self.m_rhead_item.setY(head_circle_pts[self.m_current_rhead][1])
      #  self.m_rhead_item.show()
      #  # Update changed rhead_item.

      #if self.m_frames.m_whead_info is not None:
      #  self.m_current_whead = self.m_frames.m_whead_info[frame_number]
      #  if len(head_circle_pts) <= self.m_current_whead:
      #    self.m_current_whead = -1
      #  self.m_whead_item.setX(head_circle_pts[self.m_current_whead][0])
      #  self.m_whead_item.setY(head_circle_pts[self.m_current_whead][1])
      #  self.m_whead_item.show()
      #  # Update changed whead_item.

      #if self.m_frames.m_fhead_info is not None:
      #  self.m_current_fhead = self.m_frames.m_fhead_info[frame_number]
      #  if len(head_circle_pts) <= self.m_current_fhead:
      #    self.m_current_fhead = -1
      #  self.m_fhead_item.setX(head_circle_pts[self.m_current_fhead][0])
      #  self.m_fhead_item.setY(head_circle_pts[self.m_current_fhead][1])
      #  self.m_fhead_item.show()
      #  # Update changed fhead_item.

    self.m_canvas.update()
