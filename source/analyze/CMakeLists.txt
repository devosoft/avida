INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libanalyze_a_SOURCES
  cAnalyze.cc
  cAnalyzeGenotype.cc
  cAnalyzeUtil.cc
  cAnalyzeJobQueue.cc
  cAnalyzeJobWorker.cc
  cMutationalNeighborhood.cc
)

ADD_LIBRARY(analyze ${libanalyze_a_SOURCES})

