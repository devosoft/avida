# Uses PYTHON_EXE PYTHON_LIBRARY PYTHON_INCLUDE_PATH PY_BOOST_INCLUDE_PATH PY_BOOST_LIBRARIES PY_BOOST_LIBNAME

INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/AddPysteModule.cmake)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

#
# example unittest-style test suite.
#
SET(PY_UNITTEST_SUITES
  py_kaben_brainstorms_t
)   
FOREACH(PY_TEST_SUITE ${PY_UNITTEST_SUITES})
  # this constructs a command that looks something like:
  # $ python \
  # > /home/k/devel/source/bindings/CSwig/population/py_unittest_driver.py \
  # > py_kaben_brainstorms_t
  # > /home/k/devel/,builddir/source/bindings/Boost.Python
    
  ADD_TEST(
    ${PY_TEST_SUITE}
    ${PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/py_unittest_driver.py
    ${PY_TEST_SUITE}
    ${LIBRARY_OUTPUT_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}
  )   
ENDFOREACH(PY_TEST_SUITE)

#
# A test of python packaging.
#
SET(PyAvida_Pyste_Bases

  # XXX I can't figure out how it works.
  tArray::${CMAKE_CURRENT_SOURCE_DIR}/organism.pyste:${CMAKE_CURRENT_SOURCE_DIR}/merit.pyste:${CMAKE_CURRENT_SOURCE_DIR}/mutation.pyste:${CMAKE_CURRENT_SOURCE_DIR}/int_sum.pyste:

  cpu_head::
  phenotype::
  population_cell::
  merit::

  cCycleCheck::
  cID::
  cStringable::
  cTestCase::
  cTestProblem::
  cTestResult::
  cTextTestInterpreter::

  help_entry::
  help_alias::${CMAKE_CURRENT_SOURCE_DIR}/help_entry.pyste:
  help_full_entry::${CMAKE_CURRENT_SOURCE_DIR}/help_entry.pyste:
  help_manager::
  help_type::
  reaction::
  reaction_process::
  species::
  species_control::
  species_queue::
  task_entry::
  genebank::
  genotype::
  genotype_control::
  inject_genebank::
  inject_genotype_control::
  inject_genotype_element::
  inject_genotype_queue::
  lineage_control::
  mutation_lib::
  organism::
  cpu_test_info::
  hardware_factory::
  event_factory_manager::
  analyze_function::
  genotype_batch::
  inject_genotype::
  integrated_schedule_node::
  tBuffer::
  #tDictionary::

  ##### Each of these wrappers satisfies Pyste, compiles, and loads, but are untested.
  analyze_command_def_base:: # XXX XXX
  analyze_genotype:: # XXX XXX
  analyze_util:: # XXX XXX
  birth_chamber:: # XXX XXX
  schedule:: # XXX XXX
  const_schedule::${CMAKE_CURRENT_SOURCE_DIR}/schedule.pyste: # XXX XXX
  data_manager_base:: # XXX XXX
  event_list:: # XXX XXX
  fitness_matrix:: # XXX XXX
  genotype_birth_data:: # XXX XXX
  hardware_4stack_thread:: # XXX XXX
  hardware_cpu_thread:: # XXX XXX
  inject_genotype_birth_data:: # XXX XXX
  inst_util:: # XXX XXX
  integrated_schedule::${CMAKE_CURRENT_SOURCE_DIR}/schedule.pyste: # XXX XXX
  landscape:: # XXX XXX
  lineage:: # XXX XXX
  local_mutations:: # XXX XXX
  mx_code_array:: # XXX XXX
  prob_schedule::${CMAKE_CURRENT_SOURCE_DIR}/schedule.pyste: # XXX XXX
  reaction_requisite:: # XXX
  string_iterator:: # XXX
  test_util:: # XXX
  analyze:: # XXX XXX
  avida_driver_base:: # XXX XXX
  avida_driver_analyze::${CMAKE_CURRENT_SOURCE_DIR}/avida_driver_base.pyste: # XXX XXX
  event_triggers:: # XXX XXX
  avida_triggers::${CMAKE_CURRENT_SOURCE_DIR}/event_triggers.pyste: # XXX XXX
  code_label:: # XXX XXX
  cpu_stack:: # XXX XXX
  cpu_stats:: # XXX XXX
  event_list_iterator:: # XXX XXX
  fixed_block:: # XXX XXX
  fixed_coords:: # XXX XXX
  genome:: # XXX XXX
  cpu_memory::${CMAKE_CURRENT_SOURCE_DIR}/genome.pyste:

  genome_util:: # XXX XXX
  genotype_test_data:: # XXX XXX
  histogram:: # XXX XXX
  indexed_block_struct:: # XXX XXX
  instruction:: # XXX XXX
  int_sum:: # XXX XXX
  memory_flags:: # XXX XXX
  mutation:: # XXX XXX
  mutation_rates:: # XXX XXX
  my_code_array_less_than:: # XXX XXX
  org_message:: # XXX XXX
  reaction_result:: # XXX XXX
  ref_block:: # XXX XXX
  resource:: # XXX XXX
  running_average:: # XXX XXX
  spatial_count_elem:: # XXX XXX
  spatial_res_count:: # XXX XXX
  uint:: # XXX XXX
  weighted_index:: # XXX XXX

  ##### Each of these wrappers satisfies Pyste, compiles, and loads, but many have FIXME todo lists.
  avida_driver_population::${CMAKE_CURRENT_SOURCE_DIR}/avida_driver_base.pyste # XXX XXX XXX
  config:: # XXX XXX XXX
  count_tracker:: # XXX XXX XXX
  cpu_defs::
  data_entry:: # XXX XXX XXX
  data_file:: # XXX XXX XXX
  data_file_manager:: # XXX XXX XXX
  double_sum:: # XXX XXX XXX
  environment:: # XXX XXX XXX
  file::${CMAKE_CURRENT_SOURCE_DIR}/file-fixups.hh # XXX XXX XXX
  init_file::${CMAKE_CURRENT_SOURCE_DIR}/file.pyste # XXX XXX XXX
  genesis::${CMAKE_CURRENT_SOURCE_DIR}/init_file.pyste:${CMAKE_CURRENT_SOURCE_DIR}/genesis-fixups.hh # XXX XXX XXX
  hardware_base::
  hardware_4stack::${CMAKE_CURRENT_SOURCE_DIR}/hardware_base.pyste # XXX XXX XXX
  hardware_cpu::${CMAKE_CURRENT_SOURCE_DIR}/hardware_base.pyste:${CMAKE_CURRENT_SOURCE_DIR}/hardware_cpu-fixups.hh # XXX XXX XXX
  hardware_tracer::
  hardware_tracer_cpu::
  hardware_tracer_4stack::
  hardware_tracer_test_cpu::
  hardware_status_printer::
  hardware_util:: # XXX XXX XXX
  inst_lib_base:: # XXX XXX XXX
  inst_lib_cpu::${CMAKE_CURRENT_SOURCE_DIR}/inst_lib_base.pyste # XXX XXX XXX
  inst_set:: # XXX XXX XXX
  population:: # XXX XXX XXX
  population_interface:: # XXX XXX XXX
  py_avida_driver::${CMAKE_CURRENT_SOURCE_DIR}/avida_driver_population.pyste # XXX XXX XXX
  py_hardware_tracer::
  reaction_lib:: # XXX XXX XXX
  resource_count:: # XXX XXX XXX
  resource_lib:: # XXX XXX XXX
  stats:: # XXX XXX XXX
  string:: # XXX XXX XXX
  string_list:: # XXX XXX XXX
  task_lib:: # XXX XXX XXX
  test_cpu:: # XXX XXX XXX
  tList::${CMAKE_CURRENT_SOURCE_DIR}/string.pyste # XXX XXX XXX
  tVector::
  change_list::
)
SET(PyAvida_Extra_Cpp_Files
  AB.cc
  cpu_defs-fixups.cc
  cpu_head-fixups.cc
  cpu_memory-fixups.cc
  file-fixups.cc
  genesis-fixups.cc
  hardware_cpu-fixups.cc
  py_avida_driver-fixups.cc
  py_hardware_tracer-fixups.cc
  resource_count-fixups.cc
  test_cpu-fixups.cc
)

ADD_LIBRARY(PyAvidaRepairHacks SHARED ${PyAvida_Extra_Cpp_Files})
TARGET_LINK_LIBRARIES(PyAvidaRepairHacks main cpu event main tools yaktest)

STRING(REGEX REPLACE ";" ";-I;" PyAvida_Includes "${ALL_INC_DIRS}")
SET(PyAvida_Includes
  -I ${PyAvida_Includes}
  -I ${PROJECT_SOURCE_DIR}/source/third-party/yaktest
  -I ${CMAKE_CURRENT_SOURCE_DIR}
  -I ${BOOST_INCLUDE_PATH})
SET(PyAvida_Defines)
SET(PyAvida_ExtraDepends)
SET(PyAvida_LinkLibraries PyAvidaRepairHacks ${BOOST_LIBRARY} ${PYTHON_LIBRARY})
ADD_PYSTE_PACKAGE("AvidaCore"
  PyAvida_Pyste_Bases
  PyAvida_Includes
  PyAvida_Defines
  PyAvida_ExtraDepends
  PyAvida_Extra_Cpp_Files
  PyAvida_LinkLibraries
  /lib)

# vim: set ft=conf:
