# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/svn/avida2/trunk/source/python/AvidaGui2/pyGradientScaleView.ui'
#
# Created: Thu Mar 31 16:57:36 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *
from math import exp

class pyGradientScaleView(QWidget):
  s_left_margin = 6
  s_top_margin = 6
  s_bottom_margin = 6
  s_right_margin = 6
  s_spacing = 6
  s_stripe_width = 20
  s_stripes = 100
  s_step = 10

  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    self.m_min_value = 0.0
    self.m_max_value = 0.0
    self.m_color_vector = []
    self.m_descr_vector = []
    self.m_continuous = False
    self.m_activated = False
    self.m_was_activated = False
    self.m_color_lookup = None

    self.recalcSize()

    font = QFont(self.font())
    font.setPointSize(10)
    self.setFont(font)

    if not name:
      setName("pyGradientScaleView")

  def doubleToColor(self, x): return self.m_color_lookup and self.m_color_lookup(x) or QColor(Qt.black)

  def setRange(self, min, max):
    self.m_min_value = min
    self.m_max_value = max
    self.m_continuous = True

  def setColorLookup(self, clu):
    self.m_color_lookup = clu
    self.activate(True)

  #def setColorList(self, color_vector, descr_vector):
  #  pass

  def activate(self, should_activate):
    self.m_activated = should_activate
    if should_activate:
      self.recalcSize()
      self.repaint()

  def paintEvent(self, QPaintEvent):
    if not self.m_activated:
      return

    if self.m_continuous:
      self.drawContinuousScale()
    else:
      pass
      #self.drawColorList()

  def drawContinuousScale(self):
    w = self.width()
    h = self.height()
    p = QPainter(self)
  
    stripe_height = (h-self.s_top_margin-self.s_bottom_margin)/self.s_stripes + 1
    plot_height = h-self.s_top_margin-self.s_bottom_margin-stripe_height+1
  
    text_width = w - self.s_left_margin - self.s_stripe_width - self.s_spacing
  
    for i in range(self.s_stripes):
      x = float(i) / float(self.s_stripes);
      p.fillRect(
        self.s_left_margin,
        (self.s_top_margin + (1-x)*plot_height),
        self.s_stripe_width,
        stripe_height,
        QBrush(self.doubleToColor(x))
        )
      if i%self.s_step == 0:
        p.drawText(
          self.s_left_margin+self.s_stripe_width + self.s_spacing,
          (self.s_top_margin + (1-x)*plot_height-(self.s_step-1)*stripe_height/2),
          text_width,
          self.s_step*stripe_height,
          Qt.AlignVCenter | Qt.AlignLeft,
          self.getLabelString( self.m_min_value+x*(self.m_max_value - self.m_min_value))
          )

  #def drawColorList(self):
  #  QValueVector<QString>::const_iterator str_it = m_descr_vector.begin();
  #  QValueVector<QColor>::const_iterator color_it = m_color_vector.begin();
  #
  #  w = self.width()
  #  h = self.height()
  #  p = QPainter(self)
  #
  #  stripe_height = (h-self.s_top_margin-self.s_bottom_margin)/len(m_color_vector)
  #  plot_height = h-self.s_top_margin-self.s_bottom_margin-stripe_height
  #
  #  text_width = w - self.s_left_margin - self.s_stripe_width - self.s_spacing;
  #
  #  int i,j;
  #  i = j = m_color_list_length-1;
  #  for ( ; i>=0; i--, str_it++, color_it++ ){
  #    if ( !(*str_it).isNull() ){ // draw only if description exists
  #      double x = (double) j / (double)(m_color_list_length-1);
  #      // the colored stripe
  #      p.fillRect( m_left_margin, (int) (m_top_margin + (1-x)*plot_height + .1*stripe_height), m_stripe_width, (int) (.8*stripe_height), *color_it );
  #      // the label
  #      p.drawText( m_left_margin+m_stripe_width + m_spacing, (int) (m_top_margin + (1-x)*plot_height), text_width, stripe_height, Qt::AlignVCenter | Qt::AlignLeft, *str_it );
  #      j--;
  #    } 
  #  } 
  

  def recalcSize(self):
    w = 1
    h = 1
    if self.m_activated:
      if self.m_continuous:
        w = (
          self.s_left_margin +
          self.s_stripe_width +
          self.s_spacing +
          self.s_right_margin +
          self.fontMetrics().width("0.0e+02")
          #self.fontMetrics().width(self.getLabelString(self.m_max_value))
          )
        h = self.fontMetrics().height() * self.s_stripes / self.s_step
      else:
        min_label_width = 0
        for desc in self.m_descr_vector: 
          x = self.fontMetrics().width(desc)
          if min_label_width < x:
            min_label_width = x
        w = (
          self.s_left_margin +
          self.s_stripe_width +
          self.s_spacing +
          self.s_right_margin +
          min_label_width
          )
        h = self.fontMetrics().height() * len(self.m_color_vector)

    self.setMinimumWidth(w)
    self.setMaximumWidth(w)

    self.setMinimumHeight(h)

  def getLabelString(self, x):
    return QString("%1").arg(x, 0, 'g', 2)
