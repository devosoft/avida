PROJECT(AVIDA)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# The following three variables must be after the PROJECT statement, otherwise
# newer versions of cmake will (correctly) use an empty value of
# PROJECT_BINARY_DIR, since the project didn't exist yet.

# Default location for installed software/configs/ docs is the build directory.
SET(CMAKE_INSTALL_PREFIX
  "${PROJECT_BINARY_DIR}"
  CACHE PATH
  "Install path prefix, prepended onto install directories."
  FORCE
)

# Final software is built directly into the work subdirectory.
SET(EXECUTABLE_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/bin"
  CACHE PATH
  "Single output directory for building all executables."
)

SET(LIBRARY_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/lib"
  CACHE PATH
  "Built libraries are placed here before installation."
)



# This section defines default builtin compiler options
# ------------------------------------------------------------------------------
IF(UNIX)
  IF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    SET(COMPILER_WARNING_FLAGS "")
    SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -OPT:Olimit=0")
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      SET(COMPILER_WARNING_FLAGS "")
      SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -mp1 -pc64")
    ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      IF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
        SET(COMPILER_WARNING_FLAGS "-Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-trigraphs")
        SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -fvisibility-inlines-hidden")
      ELSE (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
        SET(COMPILER_WARNING_FLAGS "-Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-trigraphs")
        SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -ftree-vectorize -fvisibility-inlines-hidden")
      ENDIF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
    ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")

  INCLUDE(CheckCSourceCompiles)
  IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(HAVE_FUSED_MADD FALSE)
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    IF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
      SET(HAVE_FUSED_MADD FALSE)
    ELSE (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
      SET(CMAKE_REQUIRED_FLAGS "-mno-fused-madd")
      CHECK_C_SOURCE_COMPILES("int main() { return 0; }" HAVE_FUSED_MADD)
    ENDIF (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  SET(CMAKE_REQUIRED_FLAGS "")

  IF(HAVE_FUSED_MADD)
    SET(COMPILER_OPTIMIZATION_FLAGS "-mno-fused-madd ${COMPILER_OPTIMIZATION_FLAGS}")
  ENDIF(HAVE_FUSED_MADD)

  # Four types of c++ compilations:
  # - debug (Debug)
  # - minimum-size release (MinSizeRel)
  # - release (Release)
  # - release with debug info (RelWithDebInfo)
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "-Os ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O2 -g ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE
    "-O3 ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
ENDIF(UNIX)


# Default build mode compiles c++ and c code with debug info and no
# optimizations.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE
    Release
    CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
ENDIF(NOT CMAKE_BUILD_TYPE)



# Build Instructions for the Avida Core functionality
# - Below are groups of sources, based on directory.  Each appends the source
# - files to the master AVIDA_CORE_SOURCES list that is used to build a
# - static library that is linked into relevant targets.
# ------------------------------------------------------------------------------
SET(AVIDA_CORE_SOURCES)
SET(ALL_INC_DIRS
  ${PROJECT_SOURCE_DIR}/source
)


# The actions directory
SET(ACTIONS_DIR ${PROJECT_SOURCE_DIR}/source/actions)
SET(ACTIONS_SOURCES
  ${ACTIONS_DIR}/cActionLibrary.cc
  ${ACTIONS_DIR}/DriverActions.cc
  ${ACTIONS_DIR}/EnvironmentActions.cc
  ${ACTIONS_DIR}/LandscapeActions.cc
  ${ACTIONS_DIR}/PopulationActions.cc
  ${ACTIONS_DIR}/PrintActions.cc
  ${ACTIONS_DIR}/SaveLoadActions.cc
)
SOURCE_GROUP(actions FILES ${ACTIONS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${ACTIONS_SOURCES})
LIST(APPEND ALL_INC_DIRS ${ACTIONS_DIR})


# The analyze directory
SET(ANALYZE_DIR ${PROJECT_SOURCE_DIR}/source/analyze)
SET(ANALYZE_SOURCES
  ${ANALYZE_DIR}/cAnalyze.cc
  ${ANALYZE_DIR}/cAnalyzeGenotype.cc
  ${ANALYZE_DIR}/cAnalyzeTreeStats_CumulativeStemminess.cc
  ${ANALYZE_DIR}/cAnalyzeTreeStats_Gamma.cc
  ${ANALYZE_DIR}/cAnalyzeJobQueue.cc
  ${ANALYZE_DIR}/cAnalyzeJobWorker.cc
  ${ANALYZE_DIR}/cGenotypeBatch.cc
  ${ANALYZE_DIR}/cGenotypeData.cc
  ${ANALYZE_DIR}/cModularityAnalysis.cc
  ${ANALYZE_DIR}/cMutationalNeighborhood.cc
)
SOURCE_GROUP(analyze FILES ${ANALYZE_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${ANALYZE_SOURCES})
LIST(APPEND ALL_INC_DIRS ${ANALYZE_DIR})


# The core directory
SET(CORE_DIR ${PROJECT_SOURCE_DIR}/source/core)
SET(CORE_SOURCES
  ${CORE_DIR}/Avida.cc
  ${CORE_DIR}/GeneticRepresentation.cc
  ${CORE_DIR}/Genome.cc
  ${CORE_DIR}/GlobalObject.cc
  ${CORE_DIR}/InstructionSequence.cc
  ${CORE_DIR}/Properties.cc
  ${CORE_DIR}/Types.cc
  ${CORE_DIR}/Version.cc
  ${CORE_DIR}/World.cc
)
SOURCE_GROUP(core FILES ${CORE_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${CORE_SOURCES})


# The cpu directory
SET(CPU_DIR ${PROJECT_SOURCE_DIR}/source/cpu)
SET(CPU_SOURCES
  ${CPU_DIR}/cCodeLabel.cc
  ${CPU_DIR}/cCPUMemory.cc
  ${CPU_DIR}/cCPUStack.cc
  ${CPU_DIR}/cCPUTestInfo.cc
  ${CPU_DIR}/cHardwareBase.cc
  ${CPU_DIR}/cHardwareBCR.cc
  ${CPU_DIR}/cHardwareCPU.cc
  ${CPU_DIR}/cHardwareExperimental.cc
  ${CPU_DIR}/cHardwareGP8.cc
  ${CPU_DIR}/cHardwareManager.cc
  ${CPU_DIR}/cHardwareStatusPrinter.cc
  ${CPU_DIR}/cHardwareTransSMT.cc
  ${CPU_DIR}/cHeadCPU.cc
  ${CPU_DIR}/cInstSet.cc
  ${CPU_DIR}/cTestCPU.cc
  ${CPU_DIR}/cTestCPUInterface.cc
)
SOURCE_GROUP(cpu FILES ${CPU_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${CPU_SOURCES})
LIST(APPEND ALL_INC_DIRS ${CPU_DIR})


# The core directory
SET(DATA_DIR ${PROJECT_SOURCE_DIR}/source/data)
SET(DATA_SOURCES
  ${DATA_DIR}/Manager.cc
  ${DATA_DIR}/Package.cc
  ${DATA_DIR}/Provider.cc
  ${DATA_DIR}/Recorder.cc
  ${DATA_DIR}/TimeSeriesRecorder.cc
)
SOURCE_GROUP(data FILES ${DATA_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${DATA_SOURCES})


# The drivers directory
SET(DRIVERS_DIR ${PROJECT_SOURCE_DIR}/source/drivers)
LIST(APPEND ALL_INC_DIRS ${DRIVERS_DIR})


# The core directory
SET(ENVIRONMENT_DIR ${PROJECT_SOURCE_DIR}/source/environment)
SET(ENVIRONMENT_SOURCES
  ${ENVIRONMENT_DIR}/Manager.cc
)
SOURCE_GROUP(environment FILES ${ENVIRONMENT_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${ENVIRONMENT_SOURCES})

# The main directory
SET(MAIN_DIR ${PROJECT_SOURCE_DIR}/source/main)
SET(MAIN_SOURCES
  ${MAIN_DIR}/cAvidaConfig.cc
  ${MAIN_DIR}/cBirthChamber.cc
  ${MAIN_DIR}/cBirthDemeHandler.cc
  ${MAIN_DIR}/cBirthEntry.cc
  ${MAIN_DIR}/cBirthGenomeSizeHandler.cc
  ${MAIN_DIR}/cBirthGlobalHandler.cc
  ${MAIN_DIR}/cBirthGridLocalHandler.cc
  ${MAIN_DIR}/cBirthMateSelectHandler.cc
  ${MAIN_DIR}/cBirthNeighborhoodHandler.cc
  ${MAIN_DIR}/cBirthSelectionHandler.cc
  ${MAIN_DIR}/cBirthMatingTypeGlobalHandler.cc
  ${MAIN_DIR}/cContextPhenotype.cc
  ${MAIN_DIR}/cDeme.cc
  ${MAIN_DIR}/cDemeNetwork.cc
  ${MAIN_DIR}/cDemeCellEvent.cc
  ${MAIN_DIR}/cEnvironment.cc
  ${MAIN_DIR}/cEventList.cc
  ${MAIN_DIR}/cGenomeUtil.cc
  ${MAIN_DIR}/cGradientCount.cc
  ${MAIN_DIR}/cLandscape.cc
  ${MAIN_DIR}/cMigrationMatrix.cc
  ${MAIN_DIR}/cMutationRates.cc
  ${MAIN_DIR}/cOrganism.cc
  ${MAIN_DIR}/cOrgMessage.cc
  ${MAIN_DIR}/cOrgSensor.cc
  ${MAIN_DIR}/cParasite.cc
  ${MAIN_DIR}/cPhenotype.cc
  ${MAIN_DIR}/cPhenPlastGenotype.cc
  ${MAIN_DIR}/cPhenPlastUtil.cc
  ${MAIN_DIR}/cPlasticPhenotype.cc
  ${MAIN_DIR}/cPopulation.cc
  ${MAIN_DIR}/cPopulationCell.cc
  ${MAIN_DIR}/cPopulationInterface.cc
  ${MAIN_DIR}/cReaction.cc
  ${MAIN_DIR}/cReactionLib.cc
  ${MAIN_DIR}/cReactionResult.cc
  ${MAIN_DIR}/cResource.cc
  ${MAIN_DIR}/cResourceCount.cc
  ${MAIN_DIR}/cResourceHistory.cc
  ${MAIN_DIR}/cResourceLib.cc
  ${MAIN_DIR}/cSpatialCountElem.cc
  ${MAIN_DIR}/cSpatialResCount.cc
  ${MAIN_DIR}/cStats.cc
  ${MAIN_DIR}/cTaskLib.cc
  ${MAIN_DIR}/cWorld.cc
)
SOURCE_GROUP(main FILES ${MAIN_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${MAIN_SOURCES})
LIST(APPEND ALL_INC_DIRS ${MAIN_DIR})


# The output directory
SET(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/source/output)
SET(OUTPUT_SOURCES
  ${OUTPUT_DIR}/File.cc
  ${OUTPUT_DIR}/Manager.cc
  ${OUTPUT_DIR}/Socket.cc
)
SOURCE_GROUP(output FILES ${OUTPUT_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${OUTPUT_SOURCES})


# The systematics directory
SET(SYSTEMATICS_DIR ${PROJECT_SOURCE_DIR}/source/systematics)
SET(SYSTEMATICS_SOURCES
  ${SYSTEMATICS_DIR}/Arbiter.cc
  ${SYSTEMATICS_DIR}/Clade.cc
  ${SYSTEMATICS_DIR}/CladeArbiter.cc
  ${SYSTEMATICS_DIR}/GenomeTestMetrics.cc
  ${SYSTEMATICS_DIR}/Genotype.cc
  ${SYSTEMATICS_DIR}/GenotypeArbiter.cc
  ${SYSTEMATICS_DIR}/Group.cc
  ${SYSTEMATICS_DIR}/Manager.cc
  ${SYSTEMATICS_DIR}/SexualAncestry.cc
  ${SYSTEMATICS_DIR}/Unit.cc
)
SOURCE_GROUP(systematics FILES ${SYSTEMATICS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${SYSTEMATICS_SOURCES})


# The tools directory
SET(TOOLS_DIR ${PROJECT_SOURCE_DIR}/source/tools)
SET(TOOLS_SOURCES
  ${TOOLS_DIR}/cArgContainer.cc
  ${TOOLS_DIR}/cArgSchema.cc
  ${TOOLS_DIR}/cBitArray.cc
  ${TOOLS_DIR}/cDataManager_Base.cc
  ${TOOLS_DIR}/cFile.cc
  ${TOOLS_DIR}/cHistogram.cc
  ${TOOLS_DIR}/cInitFile.cc
  ${TOOLS_DIR}/cMerit.cc
  ${TOOLS_DIR}/cOrderedWeightedIndex.cc
  ${TOOLS_DIR}/cRunningAverage.cc
  ${TOOLS_DIR}/cString.cc
  ${TOOLS_DIR}/cStringIterator.cc
  ${TOOLS_DIR}/cStringList.cc
  ${TOOLS_DIR}/cStringUtil.cc
)
SOURCE_GROUP(tools FILES ${TOOLS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${TOOLS_SOURCES})
LIST(APPEND ALL_INC_DIRS ${TOOLS_DIR})


# The core viewer directory
SET(UTIL_DIR ${PROJECT_SOURCE_DIR}/source/util)
SET(UTIL_SOURCES
  ${UTIL_DIR}/CmdLine.cc
  ${UTIL_DIR}/GenomeLoader.cc
)
SOURCE_GROUP(util FILES ${UTIL_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${UTIL_SOURCES})


# The core viewer directory
SET(VIEWERCORE_DIR ${PROJECT_SOURCE_DIR}/source/viewer)
SET(VIEWERCORE_SOURCES
  ${VIEWERCORE_DIR}/ClassificationInfo.cc
  ${VIEWERCORE_DIR}/Color.cc
  ${VIEWERCORE_DIR}/Driver.cc
  ${VIEWERCORE_DIR}/Freezer.cc
  ${VIEWERCORE_DIR}/GraphicsContext.cc
  ${VIEWERCORE_DIR}/Listener.cc
  ${VIEWERCORE_DIR}/Map.cc
  ${VIEWERCORE_DIR}/OrganismTrace.cc
)
SOURCE_GROUP(viewer FILES ${VIEWERCORE_SOURCES})


LIST(APPEND ALL_INC_DIRS ${PROJECT_SOURCE_DIR}/include/public)
LIST(APPEND ALL_INC_DIRS ${PROJECT_SOURCE_DIR}/include/private)


# Locate Apto
FIND_PATH(APTO_INCLUDE_DIR apto/core.h PATHS ${PROJECT_SOURCE_DIR}/../libs/apto/include)
IF(NOT TARGET aptostatic)
  FIND_LIBRARY(APTO_LIBRARY apto PATHS ${PROJECT_SOURCE_DIR}/../cbuild/lib ${PROJECT_SOURCE_DIR}/../libs/apto/cbuild/lib)
  ADD_LIBRARY(aptostatic STATIC IMPORTED)
  SET_PROPERTY(TARGET aptostatic PROPERTY IMPORTED_LOCATION "${APTO_LIBRARY}")
ENDIF(NOT TARGET aptostatic)


# Create the static library from the master source list
INCLUDE_DIRECTORIES(${ALL_INC_DIRS} ${APTO_INCLUDE_DIR})
ADD_LIBRARY(avida-core ${AVIDA_CORE_SOURCES})
IF(WIN32)
  SET_TARGET_PROPERTIES(avida-core PROPERTIES COMPILE_DEFINITIONS BUILDING_DLL)
ENDIF(WIN32)
#ADD_LIBRARY(avida-coreshared SHARED ${AVIDA_CORE_SOURCES})
#SET_TARGET_PROPERTIES(avida-coreshared PROPERTIES OUTPUT_NAME avida-core)
#TARGET_LINK_LIBRARIES(avida-coreshared aptoshared)
#IF(WIN32)
#  SET_TARGET_PROPERTIES(avida-coreshared PROPERTIES COMPILE_DEFINITIONS BUILDING_DLL)
#ENDIF(WIN32)

ADD_LIBRARY(viewer ${VIEWERCORE_SOURCES})
IF(WIN32)
  SET_TARGET_PROPERTIES(viewer PROPERTIES COMPILE_DEFINITIONS BUILDING_DLL)
ENDIF(WIN32)
#ADD_LIBRARY(viewershared SHARED ${VIEWERCORE_SOURCES})
#SET_TARGET_PROPERTIES(viewershared PROPERTIES OUTPUT_NAME viewer)
#TARGET_LINK_LIBRARIES(viewershared aptoshared)
#IF(WIN32)
#  SET_TARGET_PROPERTIES(viewershared PROPERTIES COMPILE_DEFINITIONS BUILDING_DLL)
#ENDIF(WIN32)


# Locate TCMalloc
IF(NOT TARGET tcmalloc-1.4)
  FIND_LIBRARY(TCMALLOC_LIBRARY tcmalloc-1.4 PATHS ${PROJECT_SOURCE_DIR}/../cbuild/lib ${PROJECT_SOURCE_DIR}/../libs/tcmalloc-1.4/cbuild/lib)
  ADD_LIBRARY(tcmalloc-1.4 STATIC IMPORTED)
  SET_PROPERTY(TARGET tcmalloc-1.4 PROPERTY IMPORTED_LOCATION "${TCMALLOC_LIBRARY}")
ENDIF(NOT TARGET tcmalloc-1.4)


# Handle the TCMalloc library
# ------------------------------------------------------------------------------
IF(UNIX)
  IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(AVD_ENABLE_TCMALLOC FALSE)
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(AVD_ENABLE_TCMALLOC TRUE)
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
ELSE(UNIX)
  SET(AVD_ENABLE_TCMALLOC FALSE)
ENDIF(UNIX)
IF(AVD_ENABLE_TCMALLOC)
  # Locate TCMalloc
  IF(NOT TARGET tcmalloc-1.4)
    FIND_LIBRARY(TCMALLOC_LIBRARY tcmalloc-1.4 PATHS ${PROJECT_SOURCE_DIR}/../cbuild/lib ${PROJECT_SOURCE_DIR}/../libs/tcmalloc/cbuild/lib)
    ADD_LIBRARY(tcmalloc-1.4 STATIC IMPORTED)
    SET_PROPERTY(TARGET tcmalloc-1.4 PROPERTY IMPORTED_LOCATION "${TCMALLOC_LIBRARY}")
  ENDIF(NOT TARGET tcmalloc-1.4)
ENDIF(AVD_ENABLE_TCMALLOC)



# Target Processing
# - For each enabled target, process its build instructions.  Must occur after
# - avida-core has been defined.
# ------------------------------------------------------------------------------
OPTION(AVD_CMDLINE
  "Enable building standard command line Avida (fastest version)."
  ON
)
IF(AVD_CMDLINE)
  SET(AVIDA_CMDLINE_DIR source/targets/avida)
  SET(AVIDA_CMDLINE_SOURCES ${AVIDA_CMDLINE_DIR}/primitive.cc ${AVIDA_CMDLINE_DIR}/Avida2Driver.cc)
  SOURCE_GROUP(target\\avida FILES ${AVIDA_CMDLINE_SOURCES})
  ADD_EXECUTABLE(avida ${AVIDA_CMDLINE_SOURCES})

  SET(AVIDA_CMDLINE_LIBS aptostatic avida-core aptostatic)  
  IF(AVD_ENABLE_TCMALLOC)
    LIST(APPEND AVIDA_CMDLINE_LIBS tcmalloc-1.4)
  ENDIF(AVD_ENABLE_TCMALLOC)  
  IF(NOT MSVC)
    LIST(APPEND AVIDA_CMDLINE_LIBS pthread)
  ENDIF(NOT MSVC)
  TARGET_LINK_LIBRARIES(avida ${AVIDA_CMDLINE_LIBS})
  
  INSTALL_TARGETS(/work avida)
ENDIF(AVD_CMDLINE)


# By default, do not build the console interface to Avida.
OPTION(AVD_GUI_NCURSES
  "Enable building Avida console interface."
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_NCURSES)
  # Locate the ncurses screen handling package (for Avida's console
  # interface) and the Qt graphics API (for Avida's graphic interface).
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)
  IF(NOT NCURSES_INCLUDE_PATH)
    MESSAGE("Unable to locate header files for the ncurses CRT screen handling package.  Please set the advanced variable NCURSES_INCLUDE_PATH to their location.")
  ENDIF(NOT NCURSES_INCLUDE_PATH)
  IF(NOT NCURSES_LIBRARY)
    MESSAGE("Unable to locate 'libncurses'.  Please set the advanced variable NCURSES_LIBRARY to its location.")
  ENDIF(NOT NCURSES_LIBRARY)

  IF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)

    INCLUDE_DIRECTORIES(${NCURSES_INCLUDE_PATH})
    
    SET(AVIDA_VIEWER_DIR source/targets/avida-viewer)
    SET(AVIDA_VIEWER_SOURCES
      ${AVIDA_VIEWER_DIR}/cAnalyzeScreen.cc
      ${AVIDA_VIEWER_DIR}/cAnalyzeView.cc
      ${AVIDA_VIEWER_DIR}/cBarScreen.cc
      ${AVIDA_VIEWER_DIR}/cEnvironmentScreen.cc
      ${AVIDA_VIEWER_DIR}/cHistScreen.cc
      ${AVIDA_VIEWER_DIR}/cMapScreen.cc
      ${AVIDA_VIEWER_DIR}/cMenuWindow.cc
      ${AVIDA_VIEWER_DIR}/cOptionsScreen.cc
      ${AVIDA_VIEWER_DIR}/cStatsScreen.cc
      ${AVIDA_VIEWER_DIR}/cSymbolUtil.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerAnalyzeDriver.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerDriver.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerDriver_Base.cc
      ${AVIDA_VIEWER_DIR}/cTextWindow.cc
      ${AVIDA_VIEWER_DIR}/cView.cc
      ${AVIDA_VIEWER_DIR}/cViewInfo.cc
      ${AVIDA_VIEWER_DIR}/cZoomScreen.cc
      ${AVIDA_VIEWER_DIR}/viewer.cc
    )
    SOURCE_GROUP(targets\\avida-viewer FILES ${AVIDA_VIEWER_SOURCES})
    ADD_EXECUTABLE(avida-viewer ${AVIDA_VIEWER_SOURCES})
    
    SET(AVIDA_VIEWER_LIBS avida-core aptostatic ${NCURSES_LIBRARY})
    IF(NOT MSVC)
      LIST(APPEND AVIDA_VIEWER_LIBS pthread)
    ENDIF(NOT MSVC)
    IF(AVD_ENABLE_TCMALLOC)
      LIST(APPEND AVIDA_VIEWER_LIBS tcmalloc-1.4)
    ENDIF(AVD_ENABLE_TCMALLOC)
    TARGET_LINK_LIBRARIES(avida-viewer ${AVIDA_VIEWER_LIBS})
    
    INSTALL_TARGETS(/work avida-viewer)

  ENDIF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)
ENDIF(AVD_GUI_NCURSES)



# By default, do not build the console interface to Avida.
OPTION(AVD_GUI_PROTOTYPE_TEXT
  "Enable building new exprimental Avida text interface. THIS MAY NOT COMPILE!"
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_PROTOTYPE_TEXT)
  # Locate the ncurses screen handling package (for Avida's console
  # interface) and the Qt graphics API (for Avida's graphic interface).
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)
  IF(NOT NCURSES_INCLUDE_PATH)
    MESSAGE("Unable to locate header files for the ncurses CRT screen handling package.  Please set the advanced variable NCURSES_INCLUDE_PATH to their location.")
  ENDIF(NOT NCURSES_INCLUDE_PATH)
  IF(NOT NCURSES_LIBRARY)
    MESSAGE("Unable to locate 'libncurses'.  Please set the advanced variable NCURSES_LIBRARY to its location.")
  ENDIF(NOT NCURSES_LIBRARY)

  IF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)

    INCLUDE_DIRECTORIES(${NCURSES_INCLUDE_PATH})
    
    SET(AVIDA_TEXT_VIEWER_DIR source/targets/viewer-text)
    SET(AVIDA_TEXT_VIEWER_SOURCES
      ${AVIDA_TEXT_VIEWER_DIR}/cScreen_Map.cc
      ${AVIDA_TEXT_VIEWER_DIR}/cTextWindow.cc
      ${AVIDA_TEXT_VIEWER_DIR}/cDriver_TextViewer.cc
      ${AVIDA_TEXT_VIEWER_DIR}/viewer-text.cc
    )
    SOURCE_GROUP(targets\\viewer-text FILES ${AVIDA_TEXT_VIEWER_SOURCES})
    ADD_EXECUTABLE(avida-text ${AVIDA_TEXT_VIEWER_SOURCES} ${Viewer_SOURCES} ${COREGUI_SOURCES})
    
    SET(AVIDA_TEXT_VIEWER_LIBS avida-core ${NCURSES_LIBRARY})
    IF(NOT MSVC)
      LIST(APPEND AVIDA_TEXT_VIEWER_LIBS pthread)
    ENDIF(NOT MSVC)
    IF(AVD_ENABLE_TCMALLOC)
      LIST(APPEND AVIDA_TEXT_VIEWER_LIBS tcmalloc-1.4)
    ENDIF(AVD_ENABLE_TCMALLOC)
    TARGET_LINK_LIBRARIES(avida-text ${AVIDA_TEXT_VIEWER_LIBS})
    
    INSTALL_TARGETS(/work avida-text)

  ENDIF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)
ENDIF(AVD_GUI_PROTOTYPE_TEXT)



OPTION(AVD_TASK_EVENT_GEN
  "Enable building the task_event_gen utility"
  OFF
)
IF(AVD_TASK_EVENT_GEN)
  SET(UTILS_DIR source/utils)
  SET(TASK_EVENT_GEN_SOURCES
    ${TOOLS_DIR}/cFile.cc
    ${TOOLS_DIR}/cString.cc
    ${TOOLS_DIR}/cInitFIle.cc
    ${TOOLS_DIR}/cStringIterator.cc
    ${TOOLS_DIR}/cStringList.cc
    ${UTILS_DIR}/task_events/task_event_gen.cc
  )
  ADD_EXECUTABLE(task_event_gen ${TASK_EVENT_GEN_SOURCES})
  INSTALL_TARGETS(/work task_event_gen)
ENDIF(AVD_TASK_EVENT_GEN)


OPTION(AVD_UNIT_TESTS
  "Enable the unit-tests executable.  Running this target will test various low level functionality."
  OFF
)
IF(AVD_UNIT_TESTS)
  SET(UNIT_TESTS_DIR source/targets/unit-tests)
  SET(UNIT_TESTS_SOURCES
    ${UNIT_TESTS_DIR}/main.cc
    ${TOOLS_DIR}/cBitArray.cc
  )
  ADD_EXECUTABLE(unit-tests ${UNIT_TESTS_SOURCES})
  INSTALL_TARGETS(/work unit-tests)
ENDIF(AVD_UNIT_TESTS)


# Default Configuration Files
# - Installed into the work directory alongside selected targets
# ------------------------------------------------------------------------------
SET(CFG_FILES_DIR support/config)
SET(CFG_FILES
  ${CFG_FILES_DIR}/analyze.cfg
  ${CFG_FILES_DIR}/avida.cfg
  ${CFG_FILES_DIR}/environment.cfg
  ${CFG_FILES_DIR}/events.cfg
  ${CFG_FILES_DIR}/instset-heads.cfg
  ${CFG_FILES_DIR}/instset-heads-sex.cfg
  ${CFG_FILES_DIR}/instset-transsmt.cfg
  ${CFG_FILES_DIR}/default-heads.org
  ${CFG_FILES_DIR}/default-heads-sex.org
  ${CFG_FILES_DIR}/default-transsmt.org 
)
INSTALL_FILES(/work FILES ${CFG_FILES})
