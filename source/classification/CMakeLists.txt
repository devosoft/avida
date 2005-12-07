INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libclassification_a_SOURCES
  cClassificationManager.cc
  cGenotype.cc
  cGenotype_BirthData.cc
  cGenotypeControl.cc
  cInjectGenotype.cc
  cInjectGenotypeControl.cc
  cInjectGenotypeQueue.cc
  cLineage.cc
  cSpecies.cc
  cSpeciesControl.cc
  cSpeciesQueue.cc
)

ADD_LIBRARY(classification ${libclassification_a_SOURCES})

