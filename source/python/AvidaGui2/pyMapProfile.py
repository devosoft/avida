
from qt import PYSIGNAL, QColor, QObject, Qt
from math import exp

class pyMapProfile:
  def __init__(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_color_cache_size = 201

    def continuousIndexer(idx_functor):
      def continuousIndexingFunction(population_cell_item, min, range):
        population_cell = population_cell_item.m_population_cell
        if 0.0 < range:
          index = population_cell.IsOccupied() and ((float(idx_functor(population_cell)) - min) / range) or -1.0
        else: index = -1.0

        if population_cell_item.m_index == index: return False
        else:
          population_cell_item.m_index = index
          return True
      return continuousIndexingFunction

    # Indexing functors
    NullIdx = lambda c: 0.0
    MeritIdx = lambda c: c.GetOrganism().GetPhenotype().GetMerit().GetDouble()
    FitnessIdx = lambda c: c.GetOrganism().GetPhenotype().GetFitness()
    GestationTimeIdx = lambda c: c.GetOrganism().GetPhenotype().GetGestationTime()
    SizeIdx = lambda c: c.GetOrganism().GetPhenotype().GetGenomeLength()
    #GenotypeIdx = lambda c: c.GetOrganism().GetGenotype()
    #LineageIdx = lambda c: c.GetOrganism().GetLineageLabel()


    class gradualLinScaleUpdater:
      def __init__(self, range):
        self.m_range = range
        self.m_inf = 0.0
        self.m_sup = 0.0
        self.m_target_inf = 0.0
        self.m_target_sup = 0.0
        self.m_inf_tolerance_coeff = 0.1
        self.m_sup_tolerance_coeff = 0.1
        self.m_inf_rescale_rate = 0.0
        self.m_sup_rescale_rate = 0.0
        self.m_max_rescale_factor = 0.03
        self.m_updates_to_rescale = 40
        self.m_should_reset = True

      def reset(self, should_reset):
        self.m_should_reset = should_reset

      def shouldReset(self):
        return self.m_should_reset 

      def getRange(self):
        return self.m_inf, self.m_sup

      def resetRange(self, population):
        (inf, sup) = self.m_range.getRange()
        #(self.m_target_inf, self.m_target_sup) = (self.m_inf, self.m_sup) = (inf, sup)
        (self.m_target_inf, self.m_target_sup) = (self.m_inf, self.m_sup) = (0, sup)
        self.m_inf_rescale_rate = self.m_sup_rescale_rate = 0

        return self.getRange()

      def updateRange(self, population):
        if self.m_should_reset:
          return self.resetRange(population)

        (inf, sup) = self.m_range.getRange()
        if (sup < (1 - self.m_sup_tolerance_coeff) * self.m_target_sup) or (self.m_target_sup < sup):
          new_target_sup = sup * (1 + self.m_sup_tolerance_coeff)
          #self.m_sup_rescale_rate = float(new_target_sup - self.m_sup)
          self.m_sup_rescale_rate = float(new_target_sup - self.m_sup) / self.m_updates_to_rescale
          self.m_target_sup = new_target_sup

        #if (inf > (1 + self.m_inf_tolerance_coeff) * self.m_target_inf) or (self.m_target_inf > inf):
        #  new_target_inf = inf * (1 - self.m_inf_tolerance_coeff)
        #  #self.m_sup_rescale_rate = float(new_target_sup - self.m_sup)
        #  self.m_inf_rescale_rate = float(self.m_inf - new_target_inf) / self.m_updates_to_rescale
        #  self.m_target_inf = new_target_inf

        if self.m_sup_rescale_rate != 0:
          if inf <= self.m_sup:
            #self.m_sup = self.m_target_sup
            self.m_sup += self.m_sup_rescale_rate
          else:
            max_rate = self.m_sup * self.m_max_rescale_factor
            self.m_sup += min(self.m_sup_rescale_rate, max_rate)
          if abs(self.m_target_sup - self.m_sup) < abs(self.m_sup_rescale_rate):
            self.m_sup = self.m_target_sup
            self.m_sup_rescale_rate = 0

        #if self.m_inf_rescale_rate != 0:
        #  if self.m_inf <= inf:
        #    #self.m_sup = self.m_target_sup
        #    self.m_inf -= self.m_sup_rescale_rate
        #  else:
        #    max_rate = self.m_inf * self.m_max_rescale_factor
        #    self.m_inf -= min(self.m_inf_rescale_rate, max_rate)
        #  if abs(self.m_target_inf - self.m_inf) < abs(self.m_inf_rescale_rate):
        #    self.m_inf = self.m_target_inf
        #    self.m_inf_rescale_rate = 0

        return self.getRange()

    class RangeReport(QObject):
      def __init__(self, range_functor, session_mdl):
        self.m_range_functor = range_functor
        self.m_session_mdl = session_mdl
        self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
        self.m_avida = None
        self.m_range = (0, 0)
      def setAvidaSlot(self, avida):
        print "RangeReport.setAvidaSlot() ..."
        old_avida = self.m_avida
        self.m_avida = avida
        if(old_avida):
          print "RangeReport.setAvidaSlot() unsetting old_avida ..."
          if hasattr(old_avida, "m_avida_thread_mdtr"):
            print "RangeReport.setAvidaSlot() disconnect old_avida ..."
            self.disconnect(
              old_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
              self.avidaUpdatedSlot)
          print "RangeReport.setAvidaSlot() deleting old_avida ..."
          del old_avida
          print "RangeReport.setAvidaSlot() done ..."
        if(self.m_avida):
          print "RangeReport.setAvidaSlot() connecting self.m_avida ..."
          self.connect(
            self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
            self.avidaUpdatedSlot)
      def avidaUpdatedSlot(self):
        if self.m_avida and self.m_avida.m_population:
          self.m_range = self.m_range_functor(self.m_avida.m_population)
      def getRange(self):
        return self.m_range

    NullRng = lambda p: (0, 0)
    MeritRng = lambda p: (p.GetStats().GetMinMerit(), p.GetStats().GetMaxMerit())
    FitnessRng = lambda p: (p.GetStats().GetMinFitness(), p.GetStats().GetMaxFitness())
    GestationTimeRng = lambda p: (p.GetStats().GetMinGestationTime(), p.GetStats().GetMaxGestationTime())
    SizeRng = lambda p: (p.GetStats().GetMinGenomeLength(), p.GetStats().GetMaxGenomeLength())


    def sigmoid(x, midpoint, steepness):
      val = steepness * (x - midpoint)
      return  exp(val)/(1 + exp(val))
    def sigmoidDoubleToColor(x):
      #x = max(0, min(x, 1)) * (1 - 0.1) + 0.1
      x = 1 < x and 1 or x
      x = x < 0 and 0 or x
      x = 0.1 + 0.8*x
      h = (x * 360 + 100) % 360
      v = sigmoid(x, 0.3, 10) * 255
      s = sigmoid(1 - x, 0.1, 30) * 255
      return QColor(h, s, v, QColor.Hsv)

    self.m_color_cache = [sigmoidDoubleToColor(float(n)/(self.m_color_cache_size - 1)) for n in range(self.m_color_cache_size)]
    self.m_empty_color = QColor(Qt.black)
    self.m_off_scale_color = QColor(Qt.white)
    def sigmoidColorLookup(x):
      sup = self.m_color_cache_size - 1
      x *= sup
      if sup < x: return self.m_off_scale_color
      if x < 0: return self.m_empty_color
      return self.m_color_cache[int(x)]

    self.m_entries = (
    #  Mode Name,        Indexer
      ('None',
        continuousIndexer(NullIdx),
        gradualLinScaleUpdater(RangeReport(NullRng, self.m_session_mdl)),
        None
        ),
      ('Merit',
        continuousIndexer(MeritIdx),
        gradualLinScaleUpdater(RangeReport(MeritRng, self.m_session_mdl)),
        sigmoidColorLookup
        ),
      ('Fitness',
        continuousIndexer(FitnessIdx),
        gradualLinScaleUpdater(RangeReport(FitnessRng, self.m_session_mdl)),
        sigmoidColorLookup
        ),
      ('Gestation Time',
        continuousIndexer(GestationTimeIdx),
        gradualLinScaleUpdater(RangeReport(GestationTimeRng, self.m_session_mdl)),
        sigmoidColorLookup
        ),
      ('Size',
        continuousIndexer(SizeIdx),
        gradualLinScaleUpdater(RangeReport(SizeRng, self.m_session_mdl)),
        sigmoidColorLookup
        ),
      #('Genotype',       GenotypeIdx,),
      #('Lineage',        LineageIdx,),
    )

  def getSize(self):                return len(self.m_entries)
  def getModeName(self, index):     return self.m_entries[index][0]
  def getIndexer(self, index):      return self.m_entries[index][1]
  def getUpdater(self, index):      return self.m_entries[index][2]
  def getColorLookup(self, index):  return self.m_entries[index][3]
