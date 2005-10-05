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
  reaction.cc
  reaction_lib.cc
  reaction_process.cc
  reaction_requisite.cc
  reaction_result.cc
  resource.cc
  resource_count.cc
  resource_lib.cc
  spatial_count_elem.cc
  spatial_res_count.cc
  species.cc
  species_control.cc
  species_queue.cc
  stats.cc
  task_entry.cc
  task_lib.cc
)

ADD_LIBRARY(main ${libmain_a_SOURCES})

IF(AVD_PRIMITIVE)
  ADD_EXECUTABLE(primitive primitive.cc)
  TARGET_LINK_LIBRARIES(primitive main cpu event main tools)
  LINK_DIRECTORIES(${ALL_LIB_DIRS})
  INSTALL_TARGETS(/work primitive)
ENDIF(AVD_PRIMITIVE)
