
from AvidaCore import cPopulation, cStats

class pyAvidaStatsInterface:
  def __init__(self):
    self.m_entries = (
      ('None',                        None,           0, None),
      ('Average Merit',               'average.dat',  2, cStats.GetAveMerit),
      ('Average Gestation Time',      'average.dat',  3, cStats.GetAveGestation),
      ('Average Fitness',             'average.dat',  4, cStats.GetAveFitness),
      ('Average Size',                'average.dat',  6, cStats.GetAveSize),
    )
  def getValue(self, entry_index, population):
    if entry_index:
      return self.m_entries[entry_index][3](population.GetStats())
