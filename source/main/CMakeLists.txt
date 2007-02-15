INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libmain_a_SOURCES
  avida.cc
  cAvidaConfig.cc
  cBirthChamber.cc
  cDeme.cc
  cEnvironment.cc
  cEventList.cc
  cFitnessMatrix.cc
  cGenome.cc
  cGenomeUtil.cc
  cInstruction.cc
  cLandscape.cc
  cLocalMutations.cc
  cMutationLib.cc
  cMutationRates.cc
  cMxCodeArray.cc
  cOrganism.cc
  cPhenotype.cc
  cPopulation.cc
  cPopulationCell.cc
  cPopulationInterface.cc
  cReaction.cc
  cReactionLib.cc
  cReactionResult.cc
  cResource.cc
  cResourceCount.cc
  cResourceLib.cc
  cSpatialCountElem.cc
  cSpatialResCount.cc
  cStats.cc
  cTaskLib.cc
  cWorld.cc
)

ADD_LIBRARY(main ${libmain_a_SOURCES})
