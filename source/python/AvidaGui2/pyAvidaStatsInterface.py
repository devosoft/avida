
from AvidaCore import cPopulation, cStats

class pyAvidaStatsInterface:
  def __init__(self):
    self.m_entries = (
      ('None',                          None,            0, None),
      ('Average Merit',                 'average.dat',   2, lambda s: s.GetAveMerit()),
      ('Average Fitness',               'average.dat',   4, lambda s: s.GetAveFitness()),
      ('Average Gestation Time',        'average.dat',   3, lambda s: s.GetAveGestation()),
      ('Average Size',                  'average.dat',   6, lambda s: s.GetAveSize()),
#      ('Average Generation',            'average.dat',  13, lambda s: s.SumGeneration().Average()),
#      ('Average Neutral Metric',        'average.dat',  14, lambda s: s.GetAveNeutralMetric()),
#      ('Average Lineage Label',         'average.dat',  15, lambda s: s.GetAveLineageLabel()),
#      ('Dominant Merit',                'dominant.dat',  2, lambda s: s.GetDomMerit()),
#      ('Dominant Gestation Time',       'dominant.dat',  3, lambda s: s.GetDomGestation()),
#      ('Dominant Fitness',              'dominant.dat',  4, lambda s: s.GetDomFitness()),
#      ('Dominant Size',                 'dominant.dat',  6, lambda s: s.GetDomSize()),
      ('Number of Organisms',           'count.dat',     3, lambda s: s.GetNumCreatures()),
#      ('Number of Genotypes',           'count.dat',     4, lambda s: s.GetNumGenotypes()),
#      ('Number of Threshold Genotypes', 'count.dat',     5, lambda s: s.GetNumThreshold()),
#      ('Number of Births',              'count.dat',     9, lambda s: s.GetNumBirths()),
#      ('Number of Deaths',              'count.dat',    10, lambda s: s.GetNumDeaths()),
    )
  def getValue(self, entry_index, stats):
    if entry_index:
      return self.m_entries[entry_index][3](stats)
