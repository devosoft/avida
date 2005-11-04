INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libanalyze_a_SOURCES
  cAnalyze.cc
  cAnalyzeGenotype.cc
  cAnalyzeUtil.cc
)

ADD_LIBRARY(analyze ${libanalyze_a_SOURCES})

