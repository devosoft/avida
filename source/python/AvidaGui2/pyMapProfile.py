
from qt import QColor
from math import exp

class pyMapProfile:
  def __init__(self):

    def continuousIndexer(idx_functor):
      def continuousIndexingFunction(population_cell_item, min, range):
        population_cell = population_cell_item.m_population_cell
        if 0.0 < range:
          index = population_cell.IsOccupied() and ((float(idx_functor(population_cell)) - min) / range) or 0.0
        else: index = 0.0
        if index < 0.0: index = 0.0
        elif 1.0 < index: index = 1.0

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
      def __init__(self, rng_functor):
        self.m_rng_functor = rng_functor
        self.m_inf = 0.0
        self.m_sup = 0.0
        self.m_target_inf = 0.0
        self.m_target_sup = 0.0
        self.m_inf_tol_coef = 0.1
        self.m_sup_tol_coef = 0.1
        self.m_inf_rescale_rate = 0.0
        self.m_sup_rescale_rate = 0.0
        self.m_updates_to_rescale = 10
        self.m_should_reset = True

      def reset(self, should_reset):
        self.m_should_reset = should_reset

      def shouldReset(self):
        return self.m_should_reset 

      def getRange(self):
        return self.m_inf, self.m_sup

      def resetRange(self, population):
        (inf, sup) = population and self.m_rng_functor(population) or (0.0, 0.0)
        (self.m_target_inf, self.m_target_sup) = (self.m_inf, self.m_sup) = (inf, sup)
        self.m_inf_rescale_rate = self.m_sup_rescale_rate = 0

        return self.getRange()

      def updateRange(self, population):
        if self.m_should_reset:
          return self.resetRange(population)

        (inf, sup) = population and self.m_rng_functor(population) or (0.0, 0.0)
        if (sup < (1 - self.m_sup_tol_coef) * self.m_target_sup) or (self.m_target_sup < sup):
          new_target_sup = sup * (1 + self.m_sup_tol_coef)
          self.m_sup_rescale_rate = float(new_target_sup - self.m_sup) / self.m_updates_to_rescale
          self.m_target_sup = new_target_sup

        if self.m_sup_rescale_rate != 0:
          self.m_sup += self.m_sup_rescale_rate
          if abs(self.m_target_sup - self.m_sup) < abs(self.m_sup_rescale_rate):
            self.m_sup = self.m_target_sup
            self.m_sup_rescale_rate = 0

        return self.getRange()

    # Range functors
    NullRng = lambda p: (0, 0)
    MeritRng = lambda p: (0, p.GetStats().GetMaxMerit())
    FitnessRng = lambda p: (0, p.GetStats().GetMaxFitness())
    def GestationTimeRng(p):
      return 0, max(
        p.GetCell(n).IsOccupied() and p.GetCell(n).GetOrganism().GetPhenotype().GetGestationTime() or 0
        for n in range(p.GetSize()))
    def SizeRng(p):
      return 0, max(
        p.GetCell(n).IsOccupied() and p.GetCell(n).GetOrganism().GetPhenotype().GetGenomeLength() or 0
        for n in range(p.GetSize()))


    def sigmoid(x, midpoint, steepness):
      val = steepness * (x - midpoint)
      return  exp(val)/(1 + exp(val))
    def sigmoidDoubleToColor(x):
      #x = max(0, min(x, 1)) * (1 - 0.1) + 0.1
      x = 1 < x and 1 or x
      x = x < 0 and 0 or x
      x = 0.1 + 0.9*x
      h = (x * 360 + 100) % 360
      v = sigmoid(x, 0.3, 10) * 255
      s = sigmoid(1 - x, 0.1, 30) * 255
      return QColor(h, s, v, QColor.Hsv)

    self.m_entries = (
    #  Mode Name,        Indexer
      ('None',
        continuousIndexer(NullIdx),
        gradualLinScaleUpdater(NullRng),
        None
        ),
      ('Merit',
        continuousIndexer(MeritIdx),
        gradualLinScaleUpdater(MeritRng),
        sigmoidDoubleToColor
        ),
      ('Fitness',
        continuousIndexer(FitnessIdx),
        gradualLinScaleUpdater(FitnessRng),
        sigmoidDoubleToColor
        ),
      ('Gestation Time',
        continuousIndexer(GestationTimeIdx),
        gradualLinScaleUpdater(GestationTimeRng),
        sigmoidDoubleToColor
        ),
      ('Size',
        continuousIndexer(SizeIdx),
        gradualLinScaleUpdater(SizeRng),
        sigmoidDoubleToColor
        ),
      #('Genotype',       GenotypeIdx,),
      #('Lineage',        LineageIdx,),
    )

  def getSize(self):                return len(self.m_entries)
  def getModeName(self, index):     return self.m_entries[index][0]
  def getIndexer(self, index):      return self.m_entries[index][1]
  def getUpdater(self, index):      return self.m_entries[index][2]
  def getColorLookup(self, index):  return self.m_entries[index][3]
