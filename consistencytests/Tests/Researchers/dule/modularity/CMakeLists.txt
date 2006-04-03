
# Note :
#
# ${CMAKE_CURRENT_SOURCE_DIR} points to the current source directory,
# i.e., .../Tests/Researchers/dule/LocalSex/.
#
# ${CMAKE_CURRENT_BINARY_DIR} points to the equivalent binary directory,
# i.e., .../build/Tests/Researchers/dule/LocalSex/.
#

#
# Avida configuration
#
SET(ConfigDir "${CMAKE_CURRENT_SOURCE_DIR}/config")
SET(GenesisFile "${ConfigDir}/genesis")
SET(InstSetFile "${ConfigDir}/inst_set.sex")
SET(EventFile "${ConfigDir}/events.cfg")
SET(EnvironmentFile "${ConfigDir}/environment.cfg")
SET(StartCreatureFile "${ConfigDir}/organism.100.sex")
SET(AvidaArgs "-seed %d -set INST_SET ${InstSetFile} -set EVENT_FILE ${EventFile} -set ENVIRONMENT_FILE ${EnvironmentFile} -set START_CREATURE ${StartCreatureFile} -set BIRTH_METHOD 4")

#
# Consistency checks configuration
#

# This is Python list of Avida random seeds.
# E.g., "range(100,15)" is equivalent to the Python list "[100,101,102,103,104]".
# I.e., SET(SeedSet "range(100,105)") is equivalent to SET(SeedSet "[100,101,102,103,104]").
SET(SeedSet "range(100,110)")
# Avida will be run in, and store output files in, ${CMAKE_CURRENT_BINARY_DIR}/Test_100, and so on.
SET(RunSubdirPathBase "${CMAKE_CURRENT_BINARY_DIR}/GlobalSex_")
# I've written ${ConfigDir}/events.cfg to stop Avida after 100 updates,
# and to output a single detail-population file "detail_pop.100" per run,
# i.e., ${CMAKE_CURRENT_SOURCE_DIR}/Test_100/detail_pop.100, and so on.
SET(OutputFileName "detail_pop.100")
# Similar to above, except that this tells the testing scripts where to find
# expected last lines of detail_pop.100 files,
# i.e., ${CMAKE_CURRENT_SOURCE_DIR}/Test_100/detail_pop.100.expected_last_line, and so on.
SET(ExpectationFilePathBase "${CMAKE_CURRENT_SOURCE_DIR}/GlobalSex_")
SET(ExpectationFileName "detail_pop.100.expected_last_line")

#
# This generates the Python script that performs consistency checks.
# Several of the variables defined above make their ways into the script.
# See CMake's documentaton of the CONFIGURE_FILE command for details.

SET(CheckTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/check_tails.py.in") 
SET(CheckTailsScript "${CMAKE_CURRENT_BINARY_DIR}/sex_check_tails.py") 
CONFIGURE_FILE(${CheckTailsScriptTemplate} ${CheckTailsScript} IMMEDIATE)

IF(AVD_CONSISTENCY_TEST_PRIMITIVE)
  #ADD_TEST("GlobalSex" "${_PythonExePath}" "${CheckTailsScript}")
ENDIF(AVD_CONSISTENCY_TEST_PRIMITIVE)

#
# Generation of expected last lines of detail_pop.100 files.
#
# To trigger this script, use commands like the following :
#   $ pushd ${CMAKE_CURRENT_BINARY_DIR} # i.e., cd .../build/Tests/Researchers/dule/LocalSex/
#   $ tar cvfz Tests.tgz `find . -name detail_pop.100.expected_last_line`
#   $ cd ${CMAKE_CURRENT_SOURCE_DIR}
#   $ tar xvfz ${CMAKE_CURRENT_BINARY_DIR}/Tests.tgz
#   $ popd
#
# I think it's safer to require that these steps be performed by hand in order
# to reduce the chance of inadvertently clobbering existing consistency-check suites.
#
SET(GenerateTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/generate_tails.py.in") 
SET(GenerateTailsScript "${CMAKE_CURRENT_BINARY_DIR}/sex_generate_tails.py") 
CONFIGURE_FILE(${GenerateTailsScriptTemplate} ${GenerateTailsScript} IMMEDIATE)


#
# Reconfigure for GlobalAsex
#

SET(ConfigDir "${CMAKE_CURRENT_SOURCE_DIR}/config")
SET(GenesisFile "${ConfigDir}/genesis")
SET(InstSetFile "${ConfigDir}/inst_set.asex")
SET(EventFile "${ConfigDir}/events.cfg")
SET(EnvironmentFile "${ConfigDir}/environment.cfg")
#SET(StartCreatureFile "${ConfigDir}/organism.100.asex")
SET(StartCreatureFile "${ConfigDir}/creature.final.asex")
SET(AvidaArgs "-seed %d -set INST_SET ${InstSetFile} -set EVENT_FILE ${EventFile} -set ENVIRONMENT_FILE ${EnvironmentFile} -set START_CREATURE ${StartCreatureFile} -set BIRTH_METHOD 4")

SET(SeedSet "range(100,110)")
SET(RunSubdirPathBase "${CMAKE_CURRENT_BINARY_DIR}/GlobalAsex_")
SET(OutputFileName "detail_pop.100")
SET(ExpectationFilePathBase "${CMAKE_CURRENT_SOURCE_DIR}/GlobalAsex_")
SET(ExpectationFileName "detail_pop.100.expected_last_line")

SET(CheckTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/check_tails.py.in") 
SET(CheckTailsScript "${CMAKE_CURRENT_BINARY_DIR}/asex_check_tails.py") 
CONFIGURE_FILE(${CheckTailsScriptTemplate} ${CheckTailsScript} IMMEDIATE)

IF(AVD_CONSISTENCY_TEST_PRIMITIVE)
  ADD_TEST("GlobalAsex" "${_PythonExePath}" "${CheckTailsScript}")
ENDIF(AVD_CONSISTENCY_TEST_PRIMITIVE)

SET(GenerateTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/generate_tails.py.in") 
SET(GenerateTailsScript "${CMAKE_CURRENT_BINARY_DIR}/asex_generate_tails.py") 
CONFIGURE_FILE(${GenerateTailsScriptTemplate} ${GenerateTailsScript} IMMEDIATE)

