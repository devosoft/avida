# -*- coding: utf-8 -*-


from qt import *
from qtcanvas import *


class pyOrganismScopeView(QCanvasView):
  def __init__(self,parent = None,name = None,fl = 0):
    QCanvasView.__init__(self,parent,name,fl)
    if not name: self.setName("pyOrganismScopeView")

    self.setVScrollBarMode(QScrollView.AlwaysOff)
    self.setHScrollBarMode(QScrollView.AlwaysOff)
    self.m_canvas = QCanvas()
    self.setCanvas(self.m_canvas)
    self.reset()

  def reset(self):
    self.m_instruction_items = None
    self.m_task_items = None
    self.m_ihead_item = None
    self.m_rhead_item = None
    self.m_whead_item = None
    self.m_fhead_item = None

    self.m_task_names = None
    self.m_inst_names = None

    self.m_frames = None
    self.m_max_genome_size = 0
    self.m_current_frame_number = 0
    self.m_current_genome = None
    self.m_current_tasks = None
    self.m_current_ihead = None
    self.m_current_rhead = None
    self.m_current_whead = None
    self.m_current_fhead = None

    # Hmm; can't emit gestationTimeChangedSig(0) without causing absurd slider values. @kgn
    self.emit(PYSIGNAL("gestationTimeChangedSig"),(1,))
    self.emit(PYSIGNAL("executionStepResetSig"),(0,))

  def setTaskNames(self, task_names = None):
    old_task_names = self.m_task_names
    self.m_task_names = task_names

  def setInstNames(self, inst_names = None):
    old_inst_names = self.m_inst_names
    self.m_inst_names = inst_names

  def setFrames(self, frames = None):
    self.reset()
    self.m_frames = frames
    if self.m_frames is not None:
      if self.m_frames.m_genome_info is not None:
        self.m_max_genome_size = max([len(genome) for genome in self.m_frames.m_genome_info])
        self.m_instruction_items = [QCanvasText(self.m_canvas) for i in range(self.m_max_genome_size)]
      self.updateCircle()
      self.emit(PYSIGNAL("gestationTimeChangedSig"),(self.m_frames.m_gestation_time,))
      self.showFrame(0)

  def resizeEvent(self, resize_event):
    new_size = resize_event.size()
    self.m_canvas.resize(new_size.width(), new_size.height())
    QCanvasView.resizeEvent(self, resize_event)

  def updateCircle(self):
    if self.m_instruction_items is not None:
      x = 0
      for instruction_item in self.m_instruction_items:
        x += 5
        instruction_item.setX(x)
        instruction_item.setY(100)
        instruction_item.setText('@')
        instruction_item.hide()

  def showFrame(self, frame_number = 0):
    old_frame_number = self.m_current_frame_number
    old_genome = self.m_current_genome
    old_tasks = self.m_current_tasks
    old_ihead = self.m_current_ihead
    old_rhead = self.m_current_rhead
    old_whead = self.m_current_whead
    old_fhead = self.m_current_fhead

    self.m_current_frame_number = 0
    self.m_current_genome = None
    self.m_current_tasks = None
    self.m_current_ihead = None
    self.m_current_rhead = None
    self.m_current_whead = None
    self.m_current_fhead = None

    if self.m_frames is not None and frame_number < self.m_frames.m_gestation_time:
      self.m_current_frame_number = frame_number
      if self.m_frames.m_genome_info is not None:
        self.m_current_genome = self.m_frames.m_genome_info[frame_number]
        if old_genome is None:
          # Update all instruction_items.
          for i in range(len(self.m_current_genome)):
            self.m_instruction_items[i].setText(self.m_current_genome[i])
            self.m_instruction_items[i].show()
        else:
          # Update changed instruction_items.
          old_length = len(old_genome)
          new_length = len(self.m_current_genome)
          compare_max = min(old_length, new_length)
          range_end = max(old_length, new_length)
          for i in range(compare_max):
            if old_genome[i] == self.m_current_genome[i]:
              pass
            else:
              self.m_instruction_items[i].setText(self.m_current_genome[i])

          if old_length < new_length:
            for i in range(compare_max, range_end):
              self.m_instruction_items[i].setText(self.m_current_genome[i])
              self.m_instruction_items[i].show()
          else:
            for i in range(compare_max, range_end):
              self.m_instruction_items[i].hide()

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
