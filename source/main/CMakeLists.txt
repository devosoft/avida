INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libmain_a_SOURCES
  analyze.cc
  analyze_genotype.cc
  analyze_util.cc
  avida.cc
  avida_driver_analyze.cc
  avida_driver_base.cc
  avida_driver_population.cc
  birth_chamber.cc
  callback_util.cc
  config.cc
  environment.cc
  fitness_matrix.cc
  genebank.cc
  genome.cc
  genome_util.cc
  genotype.cc
  genotype_birth_data.cc
  genotype_control.cc
  genotype_test_data.cc
  inject_genebank.cc
  inject_genotype.cc
  inject_genotype_birth_data.cc
  inject_genotype_control.cc
  inject_genotype_queue.cc
  instruction.cc
  inst_set.cc
  inst_util.cc
  landscape.cc
  lineage.cc
  lineage_control.cc
  local_mutations.cc
  mutation_lib.cc
  mutation_rates.cc
  mutation.cc
  mx_code_array.cc
  org_message.cc
  organism.cc
  phenotype.cc
  population.cc
  population_cell.cc
  population_interface.cc
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
