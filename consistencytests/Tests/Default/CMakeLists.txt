
# Note :
#
# ${CMAKE_CURRENT_SOURCE_DIR} points to the current source directory,
# i.e., .../Tests/Default/.
#
# ${CMAKE_CURRENT_BINARY_DIR} points to the equivalent binary directory,
# i.e., .../build/Tests/Default/.
#

SET(TestsName "Default-CPU")

#
# Avida configuration

# Number of updates per test
SET(RunLength "500")
# Event file generation. events.cfg.in uses the variable RunLength.
SET(EventFileTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/events.cfg.in")
SET(EventFile "${CMAKE_CURRENT_BINARY_DIR}/events.cfg")
CONFIGURE_FILE(${EventFileTemplate} ${EventFile} IMMEDIATE)

# Where to find default configure files.
SET(ConfigDir "${AVIDA_BINARY_DIR}/work")
# Configure files (other than the event file, generated above).
SET(GenesisFile "${ConfigDir}/avida.cfg")
SET(InstSetFile "${ConfigDir}/instset-classic.cfg")
SET(EnvironmentFile "${ConfigDir}/environment.cfg")
SET(StartCreatureFile "${ConfigDir}/default-classic.org")
# Command-line arguments passed to Avida.
SET(AvidaArgs "-c ${GenesisFile} -seed %d -set WORLD_X 40 -set WORLD_Y 40 -set INST_SET ${InstSetFile} -set EVENT_FILE ${EventFile} -set ENVIRONMENT_FILE ${EnvironmentFile} -set START_CREATURE ${StartCreatureFile} -set DATA_DIR .")

#
# Consistency checks configuration
#
SET(TestDirBaseName "Seed")

# This is Python list of Avida random seeds.
# E.g., "range(100,105)" is equivalent to the Python list "[100,101,102,103,104]".
# I.e., SET(SeedSet "range(100,105)") is equivalent to SET(SeedSet "[100,101,102,103,104]").
SET(SeedSet "range(100,105)")

SET(OutputFileName "detail_pop.${RunLength}")
# Similar to above, except that this tells the testing scripts where to find
# expected last lines of detail_pop.${RunLength} files,
# i.e., ${CMAKE_CURRENT_SOURCE_DIR}/Seed_100/detail_pop.500.expected_last_line, and so on.
SET(ExpectationFileName "detail_pop.${RunLength}.expected_last_line")
# Avida will be run in, and store output files in,
# ${CMAKE_CURRENT_BINARY_DIR}/Seed_100, and so on.
SET(ExpectationFilePathBase "${CMAKE_CURRENT_SOURCE_DIR}/${TestDirBaseName}_")
SET(RunSubdirPathBase "${CMAKE_CURRENT_BINARY_DIR}/${TestDirBaseName}_")

#
# This generates the Python script that performs consistency checks.
# Several of the variables defined above make their ways into the script.
# See CMake's documentaton of the CONFIGURE_FILE command for details.
SET(CheckTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/check_tails.py.in") 
SET(CheckTailsScript "${CMAKE_CURRENT_BINARY_DIR}/check_tails.py") 
CONFIGURE_FILE(${CheckTailsScriptTemplate} ${CheckTailsScript} IMMEDIATE)

IF(AVD_CONSISTENCY_TEST_PRIMITIVE)
  ADD_TEST("Consistency-Check--${TestsName}" "${_PythonExePath}" "${CheckTailsScript}")
ENDIF(AVD_CONSISTENCY_TEST_PRIMITIVE)

#
# Generation of expected last lines of detail_pop files.
#
SET(GenerateTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/generate_tails.py.in") 
SET(GenerateTailsScript "${CMAKE_CURRENT_BINARY_DIR}/generate_tails.py") 
CONFIGURE_FILE(${GenerateTailsScriptTemplate} ${GenerateTailsScript} IMMEDIATE)
#
# To regenerate expected last lines of detail_pop files, use commands like the
# following :
#   $ pushd ${CMAKE_CURRENT_BINARY_DIR} # i.e., cd .../build/consistencytests/Tests/Default/
#   $ python ./generate_tails.py
#   $ tar cvfz Tests.tgz `find . -name detail_pop.*.expected_last_line`
#   $ cd ${CMAKE_CURRENT_SOURCE_DIR}
#   $ tar xvfz ${CMAKE_CURRENT_BINARY_DIR}/Tests.tgz
#   $ popd
#
# I think it's safer to require that these steps be performed by hand in order
# to reduce the chance of inadvertently clobbering existing consistency-check suites.
#


# Fast version of test, checking only first seed.
SET(SeedSet "range(100,101)")

SET(CheckTailsScriptTemplate "${PROJECT_SOURCE_DIR}/CMakeModules/Templates/check_tails.py.in") 
SET(CheckTailsScript "${CMAKE_CURRENT_BINARY_DIR}/fast_check_tails.py") 
CONFIGURE_FILE(${CheckTailsScriptTemplate} ${CheckTailsScript} IMMEDIATE)

IF(AVD_CONSISTENCY_TEST_PRIMITIVE)
  ADD_TEST("Consistency-Check--Fast--${TestsName}" "${_PythonExePath}" "${CheckTailsScript}")
ENDIF(AVD_CONSISTENCY_TEST_PRIMITIVE)

# Vim modeline to tell Vim that this is a configuration script.
# vim: set ft=config:
