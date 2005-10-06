INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libmain_a_SOURCES
  cAnalyze.cc
  cAnalyzeGenotype.cc
  cAnalyzeUtil.cc
  avida.cc
  cAvidaDriver_Analyze.cc
  cAvidaDriver_Base.cc
  cAvidaDriver_Population.cc
  cBirthChamber.cc
  cCallbackUtil.cc
  cConfig.cc
  cEnvironment.cc
  cFitnessMatrix.cc
  cGenebank.cc
  cGenome.cc
  cGenomeUtil.cc
  cGenotype.cc
  cGenotype_BirthData.cc
  cGenotypeControl.cc
  cGenotype_TestData.cc
  cInjectGenebank.cc
  cInjectGenotype.cc
  cInjectGenotype_BirthData.cc
  cInjectGenotypeControl.cc
  cInjectGenotypeQueue.cc
  cInstruction.cc
  cInstSet.cc
  cInstUtil.cc
  cLandscape.cc
  cLineage.cc
  cLineageControl.cc
  cLocalMutations.cc
  cMutationLib.cc
  cMutationRates.cc
  cMutation.cc
  cMxCodeArray.cc
  cOrgMessage.cc
  cOrganism.cc
  cPhenotype.cc
  cPopulation.cc
  cPopulationCell.cc
  cPopulationInterface.cc
  cReaction.cc
  cReactionLib.cc
  cReactionProcess.cc
  cReactionRequisite.cc
  cReactionResult.cc
  cResource.cc
  cResourceCount.cc
  cResourceLib.cc
  cSpatialCountElem.cc
  cSpatialResCount.cc
  cSpecies.cc
  cSpeciesControl.cc
  cSpeciesQueue.cc
  cStats.cc
  cTaskEntry.cc
  cTaskLib.cc
)

ADD_LIBRARY(main ${libmain_a_SOURCES})

IF(AVD_PRIMITIVE)
  ADD_EXECUTABLE(primitive primitive.cc)
  TARGET_LINK_LIBRARIES(primitive main cpu event main tools)
  LINK_DIRECTORIES(${ALL_LIB_DIRS})
  INSTALL_TARGETS(/work primitive)
ENDIF(AVD_PRIMITIVE)
