
import os

classes_table = [
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "defs", [],],
# cpu/
  ["c4StackHead", "4stack_head", ["cpu_head"],],
  ["cCodeLabel", "code_label", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "cpu_defs", [],],
  ["cCPUHead", "cpu_head", [],],
  ["cCPUMemory", "cpu_memory", ["genome"],],
  ["cCPUStack", "cpu_stack", [],],
  ["sCPUStats", "cpu_stats", [],],
  ["cCPUTestInfo", "cpu_test_info", [],],
  ["cHardware4Stack", "hardware_4stack", ["hardware_base"],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "hardware_4stack_constants", [],],
  ["cHardware4Stack_Thread", "hardware_4stack_thread", [],],
  ["cHardwareBase", "hardware_base", [],],
  ["cHardwareCPU", "hardware_cpu", ["hardware_base"],],
  ["cHardwareCPU_Thread", "hardware_cpu_thread", [],],
  ["cHardwareFactory", "hardware_factory", [],],
  ["cHardwareUtil", "hardware_util", [],],
  ["cInstLibCPU", "inst_lib_cpu", ["inst_lib_base"],],
  ["cMemoryFlags", "memory_flags", [],],
  ["cTestCPU", "test_cpu", [],],
  ["cTestUtil", "test_util", [],],
# event/
  ["cEvent", "event", [],],
  ["cEventFactory", "event_factory", [],],
  ["cEventFactoryManager", "event_factory_manager", [],],
  ["cEventList", "event_list", [],],
  ["cEventListEntry", "event_list_entry", [],],
  ["cEventListIterator", "event_list_iterator", [],],
  ["cEventTriggers", "event_triggers", [],],
  ["cPopulationEvent", "population_event", ["event"],],
  ["cPopulationEventFactory", "population_event_factory", ["event_factory"],],
# main/
  ["cAnalyze", "analyze", [],],
  ["cAnalyzeCommand", "analyze_command", [],],
  ["cAnalyzeCommandDef", "analyze_command_def", ["analyze_command_def_base"],],
  ["cAnalyzeCommandDefBase", "analyze_command_def_base", [],],
  ["cAnalyzeFlowCommand", "analyze_flow_command", ["analyze_command"],],
  ["cAnalyzeFlowCommandDef", "analyze_flow_command_def", ["analyze_command_def_base"],],
  ["cAnalyzeFunction", "analyze_function", [],],
  ["cAnalyzeGenotype", "analyze_genotype", [],],
  ["cAnalyzeUtil", "analyze_util", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "avida", [],],
  ["cAvidaDriver_Analyze", "avida_driver_analyze", ["avida_driver_base"],],
  ["cAvidaDriver_Base", "avida_driver_base", [],],
  ["cAvidaDriver_Population", "avida_driver_population", ["avida_driver_base"],],
  ["cAvidaTriggers", "avida_triggers", ["event_triggers"],],
  ["cBirthChamber", "birth_chamber", [],],
  ["cCallbackUtil", "callback_util", [],],
  ["cConfig", "config", [],],
  ["cEnvironment", "environment", [],],
  ["cFitnessMatrix", "fitness_matrix", [],],
  ["cGenebank", "genebank", [],],
  ["cGenome", "genome", [],],
  ["cGenomeUtil", "genome_util", [],],
  ["cGenotype", "genotype", [],],
  ["cGenotypeBatch", "genotype_batch", [],],
  ["cGenotype_BirthData", "genotype_birth_data", [],],
  ["cGenotypeControl", "genotype_control", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "genotype_macros", [],],
  ["cGenotype_TestData", "genotype_test_data", [],],
  ["cInjectGenebank", "inject_genebank", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "inject_genebank_macros", [],],
  ["cInjectGenotype", "inject_genotype", [],],
  ["cInjectGenotype_BirthData", "inject_genotype_birth_data", [],],
  ["cInjectGenotypeControl", "inject_genotype_control", [],],
  ["cInjectGenotypeElement", "inject_genotype_element", [],],
  ["cInjectGenotypeQueue", "inject_genotype_queue", [],],
  ["cInstLibBase", "inst_lib_base", [],],
  ["cInstSet", "inst_set", [],],
  ["cInstUtil", "inst_util", [],],
  ["cInstruction", "instruction", [],],
  ["cLandscape", "landscape", [],],
  ["cLineage", "lineage", [],],
  ["cLineageControl", "lineage_control", [],],
  ["cLocalMutations", "local_mutations", [],],
  ["cMutation", "mutation", [],],
  ["cMutationLib", "mutation_lib", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "mutation_macros", [],],
  ["cMutationRates", "mutation_rates", [],],
  ["cMxCodeArray", "mx_code_array", [],],
  ["MyCodeArrayLessThan", "my_code_array_less_than", [],],
  ["cOrgMessage", "org_message", [],],
  ["cOrganism", "organism", [],],
  ["cPhenotype", "phenotype", [],],
  ["cPopulation", "population", [],],
  ["cPopulationCell", "population_cell", [],],
  ["cPopulationInterface", "population_interface", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "primitive", [],],
  ["cReaction", "reaction", [],],
  ["cReactionLib", "reaction_lib", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "reaction_macros", [],],
  ["cReactionProcess", "reaction_process", [],],
  ["cReactionRequisite", "reaction_requisite", [],],
  ["cReactionResult", "reaction_result", [],],
  ["cResource", "resource", [],],
  ["cResourceCount", "resource_count", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "resource_count_macros", [],],
  ["cResourceLib", "resource_lib", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "resource_macros", [],],
  ["cSpatialCountElem", "spatial_count_elem", [],],
  ["cSpatialResCount", "spatial_res_count", [],],
  ["cSpecies", "species", [],],
  ["cSpeciesControl", "species_control", [],],
  ["cSpeciesQueue", "species_queue", [],],
  ["cStats", "stats", [],],
  ["cTaskEntry", "task_entry", [],],
  ["cTaskLib", "task_lib", [],],
# tools/
  ["cBlockStruct", "block_struct", [],],
  ["cConstSchedule", "const_schedule", ["cSchedule"],],
  ["cCountTracker", "count_tracker", [],],
  ["cDataEntry", "data_entry", [],],
  ["cDataFile", "data_file", [],],
  ["cDataFileManager", "data_file_manager", [],],
  ["cDataManager_Base", "data_manager_base", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "default_message_display", [],],
  ["cDoubleSum", "double_sum", [],],
  ["cFile", "file", [],],
  ["cFixedBlock", "fixed_block", [],],
  ["cFixedCoords", "fixed_coords", [],],
  # XXX # ["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "functions", [],],
  ["cGenesis", "genesis", ["init_file"],],
  ["cHelpAlias", "help_alias", ["help_entry"],],
  ["cHelpEntry", "help_entry", [],],
  ["cHelpFullEntry", "help_full_entry", ["help_entry"],],
  ["cHelpManager", "help_manager", [],],
  ["cHelpType", "help_type", [],],
  ["cHistogram", "histogram", [],],
  ["cIndexedBlockStruct", "indexed_block_struct", [],],
  ["cInitFile", "init_file", ["file"],],
  ["cIntSum", "int_sum", [],],
  ["cIntegratedSchedule", "integrated_schedule", ["schedule"],],
  ["cIntegratedScheduleNode", "integrated_schedule_node", [],],
  ["cMerit", "merit", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "message_class", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "message_closure", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "message_display", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "message_display_hdrs", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "message_type", [],],
  ["cProbSchedule", "prob_schedule", ["schedule"],],
  ["cRandom", "random", [],],
  ["cRefBlock", "ref_block", [],],
  ["cRunningAverage", "running_average", [],],
  ["cScaledBlock", "scaled_block", [],],
  ["cSchedule", "schedule", [],],
  ["cString", "string", [],],
  ["cStringIterator", "string_iterator", [],],
  ["cStringList", "string_list", [],],
  ["cStringUtil", "string_util", [],],
  ["cTools", "tools", [],],
  ["cUInt", "uint", [],],
  ["cWeightedIndex", "weighted_index", [],],
  #["cCLASSNAMEBLAHBLAHBLAHBLAHBLAH", "win32_mkdir_hack", [],],
]

templates_table = [
  #["tArgDataEntry", "tArgDataEntry", ["tDataEntryBase"],
  #  [
  #  ],],
  ["tArray", "tArray", [],
    [
      ["double", "NONE"],
      ["cOrganism", "organism"],
      ["cMerit", "merit"],
      ["int", "NONE"],
      #["cInstSet::cInstEntry2", "inst_set"],
      ["cMutation", "mutation"],
      #["tArray<double>", "NONE"],
      ["cIntSum", "int_sum"],
    ],],
  ["tBuffer", "tBuffer", [],
    [
      ["int", "NONE"],
    ],],
  #["tDataEntry", "tDataEntry", ["tDataEntryBase"],
  #  [
  #  ],],
  #["tDataEntryBase", "tDataEntryBase", ["data_entry"],
  #  [
  #  ],],
  #["tDataEntryCommand", "tDataEntryCommand", [],
  #  [
  #  ],],
  #["tDataManager", "tDataManager", ["data_manager_base"],
  #  [
  #  ],],
  ["tDictionary", "tDictionary", [],
    [
      ["int", "NONE"],
    ],],
  ["tList", "tList", [],
    [
      ["cAnalyzeCommand", "analyze_command"],
      ["cAnalyzeGenotype", "analyze_genotype"],
      ["cMutation", "mutation"],
      ["cPopulationCell", "population_cell"],
      ["cReactionProcess", "reaction_process"],
      ["cReactionRequisite", "reaction_requisite"],
      ["cReaction", "reaction"],
      ["cString", "string"],
    ],],
  ["tMatrix", "tMatrix", [],
    [
      ["double", "NONE"],
    ],],
  #["tMemTrack", "tMemTrack", [],
  #  [
  #  ],],
  #["tVector", "tVector", [],
  #  [
  #  ],],
]

print """

Classes Table :
"""
print classes_table

print """

Templates Table :
"""
print templates_table

cmake_entries_fd = file("cmake_entries", "wU")

for class_entry in classes_table:
  cmake_entry = "  " + class_entry[1] + "::"
  filename = class_entry[1] + ".pyste"
  if os.path.isfile(filename):
    continue
  fd = file(filename, "wU")
  for import_entry in class_entry[2]:
    import_line = "Import(\"" + import_entry + ".pyste\")\n"
    fd.write(import_line)
    cmake_entry += "${CMAKE_CURRENT_SOURCE_DIR}/" + import_entry + ".pyste:"
  class_line = class_entry[0] + " = Class(\"" + class_entry[0] + "\", \"" + class_entry[1] + ".hh\")\n"
  fd.write(class_line)
  fd.close()
  cmake_entry += "\n"
  cmake_entries_fd.write(cmake_entry)


for template_entry in templates_table:
  cmake_entry = "  " + template_entry[1] + "::"
  filename = template_entry[1] + ".pyste"
  header_filename = template_entry[1] + "_instantiations.hh"
  if os.path.isfile(filename):
    continue
  fd = file(filename, "wU")
  header_fd = file(header_filename, "wU")
  for specialization_entry in template_entry[3]:
    if specialization_entry[1] != "NONE":
      import_line = "Import(\"" + specialization_entry[1] + ".pyste\")\n"
      fd.write(import_line)
      include_line = "#include \"" + specialization_entry[1] + ".hh\"\n"
      header_fd.write(include_line)
      cmake_entry += "${CMAKE_CURRENT_SOURCE_DIR}/" + specialization_entry[1] + ".pyste:"
  fd.write("\n")
  for import_entry in template_entry[2]:
    import_line = "Import(\"" + import_entry + ".pyste\")\n"
    fd.write(import_line)
    cmake_entry += "${CMAKE_CURRENT_SOURCE_DIR}/" + import_entry + ".pyste:"
  fd.write("\n")
  template_line = template_entry[0] + " = Template(\"" + template_entry[0] + "\", \"" + template_entry[1] + "_instantiations.hh\")\n"
  fd.write(template_line)
  fd.write("\ntemplate_specializers = [")
  for specialization_entry in template_entry[3]:
    specialization_string = "\"" + specialization_entry[0] + "\","
    fd.write(specialization_string)
  fd.write("]\n")
  fd.write("\n")
  fd.write("for specialization in template_specializers:\n")
  fd.write("  specialized_type = " + template_entry[0] + "(specialization)\n")
  fd.close()
  header_fd.close()
  cmake_entry += "\n"
  cmake_entries_fd.write(cmake_entry)

cmake_entries_fd.close()
