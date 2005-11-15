INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libclassification_a_SOURCES
  cClassificationManager.cc
  cGenotype.cc
  cGenotype_BirthData.cc
  cGenotype_TestData.cc
  cGenotypeControl.cc
  cInjectGenotype.cc
  cInjectGenotype_BirthData.cc
  cInjectGenotypeControl.cc
  cInjectGenotypeQueue.cc
  cLineage.cc
  cSpecies.cc
  cSpeciesControl.cc
  cSpeciesQueue.cc
)

ADD_LIBRARY(classification ${libclassification_a_SOURCES})

